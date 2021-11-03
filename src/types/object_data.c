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

material make_material(shader* s, texture dif_tex, texture spec_tex, texture norm_tex, bee_bool is_transparent, f32 shininess, vec2 tile, vec3 tint, bee_bool draw_backfaces, int uniforms_len, uniform* uniforms, const char* name)
{
	material mat;
	mat.name		   = name;
	mat.shader		   = s;
	mat.dif_tex		   = dif_tex;
	mat.spec_tex	   = spec_tex;
	mat.norm_tex	   = norm_tex;
	mat.is_transparent = is_transparent;
	mat.shininess	   = shininess;
	mat.draw_backfaces = draw_backfaces;
	glm_vec2_copy(tile, mat.tile);
	glm_vec3_copy(tint, mat.tint);
	mat.uniforms	 = uniforms;
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

mesh make_grid_mesh(int x_verts, int z_verts)
{
	//make sure both xVerts and zVerts are at least 2
	x_verts = x_verts < 2 ? 2 : x_verts; 
	z_verts = z_verts < 2 ? 2 : z_verts;

	//generates the vertices
	int verts_len = (z_verts * x_verts * F32_PER_VERT);
	f32* verts;
	verts = malloc(verts_len * sizeof(f32));
	assert(verts != NULL);
	for (int z = 0; z < z_verts; z++)
	{
		//Debug.WriteLine("---Row0" + z.ToString() + "---");
		for (int x = 0; x < x_verts * 8;)
		{
			//xyz coords              amount of verts in row +1 * step size per vert to fit in -1 to 1 space | sizeX / 2 and sizeX&sizeZ are always 1
			verts[(z * (x_verts * F32_PER_VERT)) + x + 0] = (((x / 8) + 1) * (2.0f / x_verts)) - 1.0f;
			verts[(z * (x_verts * F32_PER_VERT)) + x + 1] = 0.0f;
			verts[(z * (x_verts * F32_PER_VERT)) + x + 2] = z * (2.0f / z_verts) - 1.0f;

			// normals
			verts[(z * (x_verts * F32_PER_VERT)) + x + 3] = 0.0f;
			verts[(z * (x_verts * F32_PER_VERT)) + x + 4] = 1.0f;
			verts[(z * (x_verts * F32_PER_VERT)) + x + 5] = 0.0f;

			//uv
			verts[(z * (x_verts * F32_PER_VERT)) + x + 6] = (f32)x / (x_verts * F32_PER_VERT);
			verts[(z * (x_verts * F32_PER_VERT)) + x + 7] = (f32)z / z_verts;

			// tangents
			verts[(z * (x_verts * F32_PER_VERT)) + x +  8] = 0.0f;
			verts[(z * (x_verts * F32_PER_VERT)) + x +  9] = 1.0f;
			verts[(z * (x_verts * F32_PER_VERT)) + x + 10] = 0.0f;
			x += F32_PER_VERT;
		}
	}

	//generates the triangles
	int tris_len = (x_verts * z_verts * 6);
	f32* tris;
	tris = malloc(tris_len * sizeof(f32));
	//Debug.WriteLine("xVerts: " + xVerts.ToString() + "; zVerts: " + zVerts.ToString() + "; Verts-Length: " + verts.Length.ToString() + "; Tris-Length: " + tris.Length.ToString() + "\n");
	f32 quad = 0.0f; int tri = 0; bee_bool firstW = BEE_TRUE;
	for (int w = 0, z = 0; z < z_verts - 1; z++)
	{
		if (firstW) { w = 1; }

		for (int x = 0; x < x_verts - 1; x++)
		{
			tris[tri + 0] = quad + (f32)(x_verts * (z / w)) + 1;
			tris[tri + 1] = quad + (f32)(x_verts * (z / w)) + 0;
			tris[tri + 2] = quad + (f32)(x_verts * (z / w) + x_verts);
			tris[tri + 3] = quad + (f32)(x_verts * (z / w) + x_verts);
			tris[tri + 4] = quad + (f32)(x_verts * (z / w) + x_verts + 1);
			tris[tri + 5] = quad + (f32)(x_verts * (z / w)) + 1;

			quad += 1;
			tri += 6;
		}
		if (firstW) { w = 0; firstW = BEE_FALSE; }
		quad++;
		w += 2;
	}

	mesh m = make_mesh(verts, verts_len, tris, tris_len, "grid", BEE_FALSE);
	// free(verts);
	// free(tris);
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

	if (s->uniform_defs != NULL)
	{ arrfree(s->uniform_defs); }

	if (s->has_error) { return; }

	glDeleteProgram(s->handle);
}
