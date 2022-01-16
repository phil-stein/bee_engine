#include "object_data.h"

#include "GLAD/glad.h"
#include "stb/stb_ds.h" // STB_DS_IMPLEMENTATION is defined in renderer.c

#include "files/file_handler.h"
#include "framebuffer.h"
#include "entities.h"
#include "camera.h"
#include "renderer.h"
#include "shader.h"
#include "window.h"


// ---- material ----

material make_material(shader* shader, texture dif_tex, texture spec_tex, texture norm_tex, bee_bool is_transparent, f32 shininess, vec2 tile, vec3 tint, bee_bool draw_backfaces, int uniforms_len, uniform* uniforms, const char* name)
{
	material mat;
	mat.name		   = name;
	mat.shader		   = shader;
	mat.dif_tex		   = dif_tex;
	mat.spec_tex	   = spec_tex;
	mat.norm_tex	   = norm_tex;
	mat.is_transparent = is_transparent;
	mat.shininess	   = shininess;
	mat.draw_backfaces = draw_backfaces;
	glm_vec2_copy(tile, mat.tile);
	glm_vec3_copy(tint, mat.tint);
	for (int i = 0; i < uniforms_len; ++i)
	{ mat.uniforms[i] = uniforms[i]; }
	uniform u;
	u.assigned = false;
	u.def = NULL;
	u.int_val = 0;
	for (int i = uniforms_len; i < UNIFORMS_MAX; ++i)
	{ mat.uniforms[i] = u; }
	// mat.uniforms	 = uniforms;
	mat.uniforms_len = uniforms_len;

	return mat;
}

// ---- mesh ----

mesh make_mesh(f32* vertices, int vertices_len, u32* indices, int indices_len, const char* name, bee_bool simple) // simple: only 3 floats per vert, x,y,z coords
{
	mesh m;
	m.name = name;
	m.vertices_len   = vertices_len;
	m.vertices_elems = vertices_len / F32_PER_VERT;
	m.floats_per_vert = F32_PER_VERT;
	m.indices_len   = indices_len;	
	m.indices_elems = indices_len / 3;	
	m.vao = 0;
	m.vbo = 0;
	m.ebo = 0;

	m.indexed = BEE_FALSE;
	if (simple)
	{
		assert(indices_len == 0); // cant be indexed and simple

		m.floats_per_vert = 3;
		m.vertices_elems = vertices_len / 3;

		glGenVertexArrays(1, &m.vao);
		glGenBuffers(1, &m.vbo);
		glBindVertexArray(m.vao);

		glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices_len * sizeof(f32), vertices, GL_STATIC_DRAW);

		// vertex position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
		glEnableVertexAttribArray(0);

		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else if (indices_len == 0)
	{
		glGenVertexArrays(1, &m.vao);
		glGenBuffers(1, &m.vbo);
		glBindVertexArray(m.vao);

		glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices_len * sizeof(f32), vertices, GL_STATIC_DRAW);

		// vertex position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)0);
		glEnableVertexAttribArray(0);
		// vertex normals attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)(3 * sizeof(f32)));
		glEnableVertexAttribArray(1);
		// vertex texture-coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)(6 * sizeof(f32)));
		glEnableVertexAttribArray(2);
		// vertex tangent attribute
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)(8 * sizeof(f32)));
		glEnableVertexAttribArray(3);

		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else
	{
		m.indexed = BEE_TRUE;

		glGenVertexArrays(1, &m.vao);
		glGenBuffers(1, &m.vbo);
		glGenBuffers(1, &m.ebo);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(m.vao);

		glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices_len * sizeof(f32), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_len * sizeof(u32), indices, GL_STATIC_DRAW);

		// vertex position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)0);
		glEnableVertexAttribArray(0);
		// vertex normals attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)(3 * sizeof(f32)));
		glEnableVertexAttribArray(1);
		// vertex texture-coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)(6 * sizeof(f32)));
		glEnableVertexAttribArray(2);
		// vertex tangent attribute
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)(8 * sizeof(f32)));
		glEnableVertexAttribArray(3);

		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);


	return m;
}

mesh make_plane_mesh()
{
	// f32 verts[] = {
	// 	// positions		// normals		// texture coords
	// 	 0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // top right
	// 	 0.5f,-0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	// 	-0.5f,-0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
	// 	-0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	// };
	f32 verts[] = {
		// positions		// normals		// texture coords  // tangents
		 0.5f, 0.0f, 0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,    1.0f, 0.0f, 0.0f,	// top right
		 0.5f, 0.0f,-0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,    1.0f, 0.0f, 0.0f,	// bottom right
		-0.5f, 0.0f,-0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,    1.0f, 0.0f, 0.0f,	// bottom left
		-0.5f, 0.0f, 0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,    1.0f, 0.0f, 0.0f		// top left 
	};
	u32 indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	int vert_len = sizeof(verts) / sizeof(verts[0]);
	int ind_len = sizeof(indices) / sizeof(indices[0]);
	// printf("vert_len: %d, ind_len: %d\n", vert_len, ind_len);
	return make_mesh(verts, vert_len, indices, ind_len, "plane", BEE_FALSE);
}

mesh make_cube_mesh()
{
	assert(0); // meshes have tangets now

	f32 vertices[] = {
		// positions			// normals			  // texture coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
			0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
			0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

			0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
			0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
			0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
			0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
			0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
			0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
			0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f
	};
	int vert_len = sizeof(vertices) / sizeof(vertices[0]);

	return make_mesh(vertices, vert_len, NULL, 0, "cube", BEE_FALSE);
}

mesh make_grid_mesh(int x_quads, int z_quads)
{
	//make sure both x_verts and z_verts are at least 1
	x_quads = x_quads < 1 ? 1 : x_quads;
	z_quads = z_quads < 1 ? 1 : z_quads;

	int verts_total = (x_quads + 1) * (z_quads + 1);
	verts_total = (x_quads * z_quads) * 6 * F32_PER_VERT;		// tmp
	f32* verts = malloc(verts_total * sizeof(f32));
	ASSERT(verts != NULL);
	int offset = 0; // vert offset

	int indices_total = (x_quads * z_quads * 6);
	u32* indices = malloc(indices_total * sizeof(u32));
	ASSERT(indices != NULL);
	int offs = 0; // tri offset

 
	// add a vert to the array by giving the x, z coordinates
	#define ADD_VERT(x, z)		verts[offset + 0] = x; verts[offset + 1] = 0; verts[offset + 2] = z;							\
								/* normals  */																					\
								verts[offset + 3] = 0.0f; verts[offset + 4] = 1.0f; verts[offset + 5] = 0.0f;  					\
								/* @TODO: uv's */																				\
								verts[offset + 6] = 0; verts[offset + 7] = 0;													\
								/* tangents */																					\
								verts[offset +  8] = 0.0f; verts[offset +  9] = 1.0f; verts[offset + 10] = 0.0f;				\
								offset += F32_PER_VERT;

	for (int z = 0; z < z_quads; ++z)
	{
		for (int x = 0; x < x_quads; ++x)
		{
			ADD_VERT(0 + x, 1 + z);		// left top
			ADD_VERT(1 + x, 1 + z);		// right top
			ADD_VERT(1 + x, 0 + z);		// right bottom
			ADD_VERT(1 + x, 0 + z);		// right bottom
			ADD_VERT(0 + x, 0 + z);		// left bottom
			ADD_VERT(0 + x, 1 + z);		// left top
			/*
			if (x == 0 && z == 0)	// first quad
			{
				ADD_VERT(0, 1);		// left top
				ADD_VERT(1, 1);		// right top
				ADD_VERT(1, 0);		// right bottom
				ADD_VERT(0, 0);		// left bottom

				indices[0] = 0;		// left top
				indices[1] = 1;		// right top
				indices[2] = 2;		// right bottom
				indices[3] = 2;		// right bottom
				indices[4] = 3;		// left bottom
				indices[5] = 0;		// left top
				offs += 6;
			}
			else if (z == 0)		// bottom x row
			{
				ADD_VERT(1 + x, 1);	// right top
				ADD_VERT(1 + x, 0); // right bottom

				int x_off = x * 2;
				indices[0] = 0;		// left top
				indices[1] = 1;		// right top
				indices[2] = 2;		// right bottom
				indices[3] = 2;		// right bottom
				indices[4] = 3;		// left bottom
				indices[5] = 0;		// left top
				offs += 6;
			}
			else if (x == 0)		// first z row
			{
				int x_off = x * 4 + (z == 0 ? 2 : x_quads + 1);
				ADD_VERT(0, 1 + z); // left top
				ADD_VERT(0, 0 + z); // left bottom
			}
			else					// middle quads
			{
				ADD_VERT(1+x, 1+z);	// right top
			}
			*/
		}
	}

	// ERR_CHECK(offset / F32_PER_VERT != verts_total, 
	// 		  "verts added werent the amount that the grid would need");

	ERR_CHECK(offs / 6 != indices_total,
		"indices added werent the amount that the grid would need");

	mesh m = make_mesh(verts, verts_total, NULL, 0, "grid", BEE_FALSE);

	// ---- cleanup ----
	free(verts);
	free(indices);
	#undef ADD_VERT(x, z)

	return m;
}

mesh make_grid_mesh_indexed(int x_quads, int z_quads, bee_bool centered, bee_bool uniform_uv)
{
	//make sure both x_verts and z_verts are at least 1
	x_quads = x_quads < 1 ? 1 : x_quads;
	z_quads = z_quads < 1 ? 1 : z_quads;

	int verts_total = (x_quads + 1) * (z_quads + 1) * F32_PER_VERT;
	f32* verts = malloc(verts_total * sizeof(f32));
	ASSERT(verts != NULL);
	int offset = 0; // vert offset

	int indices_total = (x_quads * z_quads * 6); // two tris with 3 indices per face
	u32* indices = malloc(indices_total * sizeof(u32));
	ASSERT(indices != NULL);
	int offs = 0; // tri offset

	// verts
	for (int z = 0; z < z_quads +1; ++z)
	{
		for (int x = 0; x < x_quads +1; ++x)
		{
			// if centered move back by half the total size
			f32 _x = centered ? (f32)x - ((f32)x_quads * 0.5f) : (f32)x;
			f32 _z = centered ? (f32)z - ((f32)z_quads * 0.5f) : (f32)z;
			// x, y, z coords
			verts[offset + 0] = _x; verts[offset + 1] = 0; verts[offset + 2] = _z;		
			// normals
			verts[offset + 3] = 0.0f; verts[offset + 4] = 1.0f; verts[offset + 5] = 0.0f;  
			// uv's 
			// use the bigger of the quad amounts to map to, so the uv arent stretched
			f32 uv_x = (f32)(uniform_uv ? ( x_quads >= z_quads ? x_quads : z_quads ) : x_quads);
			f32 uv_z = (f32)(uniform_uv ? ( x_quads >= z_quads ? x_quads : z_quads ) : z_quads);
			verts[offset + 6] = (f32)x / uv_x; verts[offset + 7] = (f32)z / uv_z;
			// tangents 
			verts[offset + 8] = 0.0f; verts[offset + 9] = 1.0f; verts[offset + 10] = 0.0f;
			offset += F32_PER_VERT;
		}
	}
	// indices
	for (int z = 0; z < z_quads; ++z)
	{
		for (int x = 0; x < x_quads; ++x)
		{
			int off_top = x + ((x_quads +1) * (z + 1));	// draw a grid to understand this
			int off_bot = x + ((x_quads +1) * z);		// draw a grid to understand this
			// printf("bot: %d, x: %d, x_verts: %d, z: %d, x_verts * z: %d  \n", off_bot, x, x_quads +1, z, (x_quads + 1) * z);
			// printf("top: %d, x: %d, x_verts: %d, z: %d, x_verts * z: %d\n\n", off_top, x, x_quads +1, z +1, (x_quads + 1) * (z + 1));
			indices[offs + 0] = off_top;			// left top
			indices[offs + 1] = off_top + 1;		// right top
			indices[offs + 2] = off_bot + 1;		// right bottom
			indices[offs + 3] = off_bot + 1;		// right bottom
			indices[offs + 4] = off_bot;			// left bottom
			indices[offs + 5] = off_top;			// left top
			offs += 6;
		}
	}


	ERR_CHECK(offset == verts_total, 
			  "verts added werent the amount that the grid would need");

	ERR_CHECK(offs == indices_total,
			  "indices added werent the amount that the grid would need");

	mesh m = make_mesh(verts, verts_total, indices, indices_total, "grid", BEE_FALSE);

	// ---- cleanup ----
	free(verts);
	free(indices);

	return m;
}


// ---- camera ----

camera make_camera(f32 perspective, f32 near_plane, f32 far_plane)
{
	camera cam;
	cam.perspective = perspective;
	cam.near_plane  = near_plane;
	cam.far_plane   = far_plane;
	vec3 front  = { 0.0f, 0.0f, -1.0f }; // { 0.0f, -0.5f, -0.85f };
	vec3 up     = { 0.0f, 1.0f, 0.0f  };
	vec3 target = { 0.0f, 0.0f, 0.0f  };
	glm_vec3_copy(front, cam.front);
	glm_vec3_copy(up, cam.up);
	glm_vec3_copy(target, cam.target);
	return cam;
}

// ---- light ----

light make_point_light(vec3 ambient, vec3 diffuse, vec3 specular, f32 constant, f32 linear, f32 quadratic)
{
	light _light;
	_light.enabled = BEE_TRUE;
	_light.cast_shadow = BEE_TRUE;
	_light.shadow_map_x = 2048;
	_light.shadow_map_y = 2048;
	_light.type = POINT_LIGHT;
	
	glm_vec3_copy(ambient, _light.ambient);
	glm_vec3_copy(diffuse, _light.diffuse);
	glm_vec3_copy(specular, _light.specular);
	_light.dif_intensity = 1.0f;

	_light.constant = constant;
	_light.linear = linear;
	_light.quadratic = quadratic;

	vec3 dir = { 0.2f, -1.0f, 0.2f };
	glm_vec3_copy(dir, _light.direction);

	return _light;
}
light make_dir_light(vec3 ambient, vec3 diffuse, vec3 specular, vec3 direction)
{
	light _light;
	_light.enabled = BEE_TRUE;
	_light.cast_shadow = BEE_TRUE;
	_light.shadow_map_x = 2048;
	_light.shadow_map_y = 2048;
	_light.type = DIR_LIGHT;

	glm_vec3_copy(ambient, _light.ambient);
	glm_vec3_copy(diffuse, _light.diffuse);
	glm_vec3_copy(specular, _light.specular);
	_light.dif_intensity = 1.0f;
		
	glm_vec3_copy(direction, _light.direction);
	
	_light.constant	 = 1.0f;
	_light.linear	 = 0.14f;
	_light.quadratic = 0.13;

	create_framebuffer_shadowmap(&_light.fb_shadow.buffer, &_light.fb_shadow.fbo, _light.shadow_map_x, _light.shadow_map_y);
	return _light;
}
light make_spot_light(vec3 ambient, vec3 diffuse, vec3 specular, vec3 direction, f32 constant, f32 linear, f32 quadratic, f32 cut_off, f32 outer_cut_off)
{
	light _light;
	_light.enabled = BEE_TRUE;
	_light.cast_shadow = BEE_TRUE;
	_light.shadow_map_x = 2048;
	_light.shadow_map_y = 2048;
	_light.type = SPOT_LIGHT;

	glm_vec3_copy(ambient, _light.ambient);
	glm_vec3_copy(diffuse, _light.diffuse);
	glm_vec3_copy(specular, _light.specular);
	_light.dif_intensity = 1.0f;
	
	glm_vec3_copy(direction, _light.direction);
	
	_light.constant = constant;
	_light.linear = linear;
	_light.quadratic = quadratic;
	_light.cut_off = cut_off;
	_light.outer_cut_off = outer_cut_off;

	return _light;
}

// ---- physics ----

rigidbody make_rigidbody(f32 mass)
{
	rigidbody rb;
	rb.mass = mass;
	glm_vec3_copy(GLM_VEC3_ZERO, rb.velocity);
	glm_vec3_copy(GLM_VEC3_ZERO, rb.force);
	return rb;
}

collider make_sphere_collider(f32 radius, bee_bool is_trigger)
{
	collider c;
	c.type = SPHERE_COLLIDER;
	glm_vec3_copy(GLM_VEC3_ZERO, c.offset);
	c.infos = NULL;
	c.infos_len = 0;
	c.is_trigger = is_trigger;
	sphere_collider s;
	s.radius = radius;
	c.sphere = s;
	return c;
}

collider make_box_collider(vec3 size, bee_bool is_trigger)
{
	collider c;
	c.type = BOX_COLLIDER;
	glm_vec3_copy(GLM_VEC3_ZERO, c.offset);
	c.infos = NULL;
	c.infos_len = 0;
	c.is_trigger = is_trigger;
	box_collider b;
	vec3 min = { -size[0] * 0.5f, -size[1] * 0.5f, -size[2] * 0.5f };
	glm_vec3_copy(min, b.aabb[0]);
	vec3 max = { size[0] * 0.5f, size[1] * 0.5f, size[2] * 0.5f };
	glm_vec3_copy(max, b.aabb[1]);
	c.box = b;
	return c;
}

// ---- entity ----

entity make_entity(vec3 pos, vec3 rot, vec3 scale, mesh* _mesh, material* _mat, camera* _cam, light* _light, rigidbody* rb, collider* col, char* _name)
{
	entity ent;
	ent.name = _name;
	ent.visible = BEE_TRUE;
	ent.rotate_global = BEE_TRUE;

	ent.has_trans = (pos == NULL || rot == NULL || scale == NULL) ? BEE_FALSE : BEE_TRUE;
	if (ent.has_trans)
	{
		glm_vec3_copy(pos,	 ent.pos);
		glm_vec3_copy(rot,	 ent.rot);
		glm_vec3_copy(scale, ent.scale);
	}

	ent.has_model = (_mesh != NULL && _mat != NULL) ? BEE_TRUE : BEE_FALSE;
	if (ent.has_model)
	{
		ent._mesh	  = _mesh;
		ent._material = _mat;
	}
	ent.has_cam = _cam != NULL ? BEE_TRUE : BEE_FALSE;
	if (ent.has_cam)
	{
		ent._camera = *_cam;
	}
	ent.has_light = _light != NULL ? BEE_TRUE : BEE_FALSE;
	if (_light != NULL)
	{
		ent._light = *_light;
	}

	ent.has_rb = rb != NULL ? BEE_TRUE : BEE_FALSE;
	if (ent.has_rb)
	{
		ent.rb = *rb;
	}
	ent.has_collider = col != NULL ? BEE_TRUE : BEE_FALSE;
	if (ent.has_collider)
	{
		ent.collider = *col;
	}

	ent.scripts_len = 0;
	ent.scripts		= NULL; // needs to be null-pointer for stb_ds

	ent.parent		 = 9999;
	ent.children	 = NULL; // needs to be null-pointer for stb_ds
	ent.children_len = 0;

	return ent;
}

// for script, collider, etc. components
void update_entity(entity* ent)
{
	// scripts
	for (int i = 0; i < ent->scripts_len; ++i)
	{
		if (ent->scripts[i]->active == BEE_FALSE || ent->scripts[i]->path_valid == BEE_FALSE )
		{
			continue;
		}
		if (ent->scripts[i]->has_error == BEE_TRUE)
		{
			ent->scripts[i]->has_error = BEE_FALSE;
			ent->scripts[i]->init_closure_assigned		= BEE_FALSE;
			ent->scripts[i]->update_closure_assigned    = BEE_FALSE;
			ent->scripts[i]->trigger_closure_assigned   = BEE_FALSE;
			ent->scripts[i]->collision_closure_assigned = BEE_FALSE;
		}

		// source not yet read
		if (ent->scripts[i]->init_closure_assigned == BEE_FALSE) // ent->scripts[i]->source == NULL | && ent->scripts[i]->closure == NULL
		{
			printf("[started run_init] for %s\n", ent->scripts[i]->name);
			ent->scripts[i]->source = read_text_file(ent->scripts[i]->path);
			assert(ent->scripts[i]->source != NULL);
			gravity_run_init(ent->scripts[i], ent->scripts[i]->source, ent->id);
		}
		else 
		{
			assert(ent->scripts[i]->vm != NULL);
			gravity_run_update(ent->scripts[i], ent->id);
		}
		if (ent->has_collider && ent->collider.infos_len > 0)
		{
			assert(ent->scripts[i]->vm != NULL);

			for (int n = 0; n < ent->collider.infos_len; ++n)
			{
				collision_info info = ent->collider.infos[n];
				if (info.trigger)
				{
					gravity_run_on_trigger(ent->scripts[i], ent->id, ent->collider.infos[n].entity_id);
				}
				else 
				{
					gravity_run_on_collision(ent->scripts[i], ent->id, ent->collider.infos[n].entity_id);
				}
			}
			arrfree(ent->collider.infos);
			ent->collider.infos = NULL;
			ent->collider.infos_len = 0;
		}

	}
}

// ---- structure ----

structure make_structure(entity* top_entity)
{
	structure s = NULL;
	arrput(s, top_entity); // first elem is top entity

	// recursion to get a tree structure
	// with each entity being followed by their children

	for (int i = 0; i < top_entity->children_len; ++i)
	{
		add_entity_to_structure(s, get_entity(top_entity->children[i]));
	}

	return s;

}
void add_entity_to_structure(structure s, entity* e)
{
	arrput(s, e);
	for (int i = 0; i < e->children_len; ++i)
	{
		add_entity_to_structure(s, get_entity(e->children[i]));
	}
}

// ---- free ----

void free_entity(entity* ent)
{
	if (ent->has_model)
	{
		// free_mesh(ent->_mesh);
		// free_material(ent->_material);
	}

	if (ent->scripts_len > 0)
	{
		// scripts
		// scripts get freed in asset-manager as the same script might be on multiple entites
		// arrfree(ent->scripts);
	}
}
void free_mesh(mesh* _mesh)
{
	glDeleteVertexArrays(1, &_mesh->vao);
	glDeleteBuffers(1, &_mesh->vbo);

	if (_mesh->indexed == BEE_TRUE)
	{
		glDeleteBuffers(1, &_mesh->ebo);
	}
}
void free_texture(texture* tex)
{
	glDeleteTextures(1, &tex->handle);
#ifdef EDITOR_ACT
	glDeleteTextures(1, &tex->icon_handle);
#endif
}
void free_shader(shader* s)
{
	free(s->name);

	if (s->has_error) { return; }

	glDeleteProgram(s->handle);
}
