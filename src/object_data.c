#include "GLAD/glad.h"

#include "object_data.h"
#include "shader.h"
#include "camera.h"
#include "window.h"


//
// !!! the free_ functions could potentially be called multiple times, by different entitys 
// !!! so safe-guard the buffered data with a if (data != NULL)
//

// ---- material ----

material make_material(u32 shader, texture dif_tex, texture spec_tex, bee_bool is_transparent, f32 shininess, vec2 tile)
{
	material mat;
	mat.shader = shader;
	mat.dif_tex = dif_tex;
	mat.spec_tex = spec_tex;
	mat.is_transparent = is_transparent;
	mat.shininess = shininess;
	glm_vec2_copy(tile, mat.tile);

	vec3 tint = { 1.0f, 1.0f, 1.0f };
	glm_vec3_copy(tint, mat.tint);

	return mat;
}
material make_material_tint(u32 shader, texture dif_tex, texture spec_tex, bee_bool is_transparent, f32 shininess, vec2 tile, vec3 tint)
{
	material mat = make_material(shader, dif_tex, spec_tex, shininess, is_transparent, tile);
	glm_vec3_copy(tint, mat.tint);

	return mat;
}

void free_material(material* mat)
{
	glDeleteProgram(mat->shader);

	// @UNCLEAR: do i need to tell opengl to unload the texture off the gpu
	glDeleteTextures(1, &mat->dif_tex.handle);
	glDeleteTextures(1, &mat->spec_tex.handle);
}

// ---- mesh ----

mesh make_mesh(f32* vertices, int vertices_len, u32* indices, int indices_len) // , u32* indices[]
{
	mesh m;
	m.visible = BEE_TRUE;
	// m.vertices = NULL;
	m.vertices_len = vertices_len;
	// m.indices = NULL;
	m.indices_len = indices_len;
	m.vao = 0;
	m.vbo = 0;
	m.ebo = 0;

	m.indexed = BEE_FALSE;
	if (indices == NULL || indices_len == 0)
	{
		glGenVertexArrays(1, &m.vao);
		glGenBuffers(1, &m.vbo);
		glBindVertexArray(m.vao);

		glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices_len * sizeof(f32), vertices, GL_STATIC_DRAW);

		// vertex position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)0);
		glEnableVertexAttribArray(0);
		// vertex normals attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(3 * sizeof(f32)));
		glEnableVertexAttribArray(1);
		// vertex texture-coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(6 * sizeof(f32)));
		glEnableVertexAttribArray(2);

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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)0);
		glEnableVertexAttribArray(0);
		// vertex normals attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(3 * sizeof(f32)));
		glEnableVertexAttribArray(1);
		// vertex texture-coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(6 * sizeof(f32)));
		glEnableVertexAttribArray(2);

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
		// positions		// normals		// texture coords
		 0.5f, 0.0f, 0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, 0.0f,-0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, 0.0f,-0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
		-0.5f, 0.0f, 0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	u32 indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	int vert_len = sizeof(verts) / sizeof(verts[0]);
	int ind_len = sizeof(indices) / sizeof(indices[0]);
	printf("vert_len: %d, ind_len: %d\n", vert_len, ind_len);
	return make_mesh(verts, vert_len, indices, ind_len);
}

mesh make_cube_mesh()
{
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

	return make_mesh(vertices, vert_len, NULL, 0);
}

mesh make_grid_mesh(int x_verts, int z_verts)
{
	//make sure both xVerts and zVerts are at least 2
	x_verts = x_verts < 2 ? 2 : x_verts; z_verts = z_verts < 2 ? 2 : z_verts;

	//generates the vertices
	int verts_len = (z_verts * x_verts * 8);
	f32* verts;
	verts = malloc(verts_len * sizeof(f32));
	assert(verts != NULL);
	for (int z = 0; z < z_verts; z++)
	{
		//Debug.WriteLine("---Row0" + z.ToString() + "---");
		for (int x = 0; x < x_verts * 8;)
		{
			//xyz coords              amount of verts in row +1 * step size per vert to fit in -1 to 1 space | sizeX / 2 and sizeX&sizeZ are always 1
			verts[(z * (x_verts * 8)) + x + 0] = (((x / 8) + 1) * (2.0f / x_verts)) - 1.0f;//(float)Math.Clamp((double) , -1, 1);
			verts[(z * (x_verts * 8)) + x + 1] = 0.0f;
			verts[(z * (x_verts * 8)) + x + 2] = z * (2.0f / z_verts) - 1.0f; //(float)Math.Clamp((double) , -1, 1);
			//Debug.WriteLine("Vert0" + (x / 8).ToString() + ": X: " + ((((x / 8) + 1) * (2f / xVerts)) - 1f).ToString() + ", Z: " + (z * (2f / zVerts) - 1f).ToString());

			// normals for lighting
			verts[(z * (x_verts * 8)) + x + 3] = 0.0f;
			verts[(z * (x_verts * 8)) + x + 4] = 1.0f;
			verts[(z * (x_verts * 8)) + x + 5] = 0.0f;

			//uv
			verts[(z * (x_verts * 8)) + x + 6] = (f32)x / (x_verts * 8);
			verts[(z * (x_verts * 8)) + x + 7] = (f32)z / z_verts;
			//Debug.WriteLine(" |U: " + verts[(z * (xVerts * 8)) + x + 6].ToString() + ", V: " + verts[(z * (xVerts * 8)) + x + 7].ToString() + ", x: " + x.ToString() + ", xVerts: " + xVerts.ToString() + ", z: " + z.ToString() + ", zVerts: " + zVerts.ToString());
			//Debug.WriteLine("Vert: " + (z * (xVerts * 8) + x).ToString() + ", x: " + x.ToString() + ", Coords: " + ((((x / 8) + 1) * (2f / xVerts)) - 1f).ToString() + "f, 0f ," + (z * (2f / zVerts) - 1f).ToString() + "f");
			x += 8;
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
		//Debug.WriteLine("---Row0" + z.ToString() + "---");
		for (int x = 0; x < x_verts - 1; x++)
		{
			tris[tri + 0] = quad + (f32)(x_verts * (z / w)) + 1;
			tris[tri + 1] = quad + (f32)(x_verts * (z / w)) + 0;
			tris[tri + 2] = quad + (f32)(x_verts * (z / w) + x_verts);
			tris[tri + 3] = quad + (f32)(x_verts * (z / w) + x_verts);
			tris[tri + 4] = quad + (f32)(x_verts * (z / w) + x_verts + 1);
			tris[tri + 5] = quad + (f32)(x_verts * (z / w)) + 1;

			//Debug.WriteLine("Quad: " + quad.ToString() + ", Indices: " + tris[tri + 0].ToString() + ", " + tris[tri + 1].ToString() + ", " + tris[tri + 2].ToString() + ", " + tris[tri + 3].ToString() + ", " + tris[tri + 4].ToString() + ", " + tris[tri + 5].ToString());

			quad += 1;
			tri += 6;
		}
		if (firstW) { w = 0; firstW = BEE_FALSE; }
		quad++;
		w += 2;
	}

	mesh m = make_mesh(verts, verts_len, tris, tris_len);
	free(verts);
	free(tris);
	return m;
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


// ---- model ----
/*
model make_model(mesh* _mesh, material* _material)
{
	model mod;
	mod._mesh = *_mesh;
	mod._material = *_material;

	return mod;
}

// draw_model()
void draw_model(model* _model, vec3 pos, vec3 rot, vec3 scale, enum bee_bool rotate_global)
{
	draw_mesh(&_model->_mesh, &_model->_material, pos, rot, scale, rotate_global);
}

void free_model(model* _model)
{
	free_material(&_model->_material);
	free_mesh(&_model->_mesh);
}
*/

// ---- light ----

light make_point_light(vec3 ambient, vec3 diffuse, vec3 specular, f32 constant, f32 linear, f32 quadratic)
{
	light _light;
	_light.type = POINT_LIGHT;
	
	glm_vec3_copy(ambient, _light.ambient);
	glm_vec3_copy(diffuse, _light.diffuse);
	glm_vec3_copy(specular, _light.specular);

	_light.constant = constant;
	_light.linear = linear;
	_light.quadratic = quadratic;

	return _light;
}
light make_dir_light(vec3 ambient, vec3 diffuse, vec3 specular, vec3 direction)
{
	light _light;
	_light.type = DIR_LIGHT;

	glm_vec3_copy(ambient, _light.ambient);
	glm_vec3_copy(diffuse, _light.diffuse);
	glm_vec3_copy(specular, _light.specular);
		
	glm_vec3_copy(direction, _light.direction);

	return _light;
}
light make_spot_light(vec3 ambient, vec3 diffuse, vec3 specular, vec3 direction, f32 constant, f32 linear, f32 quadratic, f32 cut_off, f32 outer_cut_off)
{
	light _light;
	_light.type = SPOT_LIGHT;

	glm_vec3_copy(ambient, _light.ambient);
	glm_vec3_copy(diffuse, _light.diffuse);
	glm_vec3_copy(specular, _light.specular);
	
	glm_vec3_copy(direction, _light.direction);
	
	_light.constant = constant;
	_light.linear = linear;
	_light.quadratic = quadratic;
	_light.cut_off = cut_off;
	_light.outer_cut_off = outer_cut_off;

	return _light;
}

// ---- entity ----

entity make_entity(vec3 pos, vec3 rot, vec3 scale, mesh* _mesh, material* mat, light* _light, char* _name)
{
	entity ent;
	ent.name = _name;
	ent.rotate_global = BEE_TRUE;

	glm_vec3_copy(pos, ent.pos);
	glm_vec3_copy(rot, ent.rot);
	glm_vec3_copy(scale, ent.scale);

	ent.has_model = _mesh != NULL && mat != NULL ? BEE_TRUE : BEE_FALSE;
	if (ent.has_model)
	{
		ent._mesh	  = *_mesh;
		ent._material = *mat;
	}
	ent.has_light = _light != NULL ? BEE_TRUE : BEE_FALSE;
	if (_light != NULL)
	{
		ent._light = *_light;
	}

	return ent;
}

// for script, collider, etc. components
void update_entity(entity* ent)
{
	if (0 == 1)
	{
		// draw_model(&ent->_model, ent->pos, ent->rot, ent->scale, ent->rotate_global);
	}
}

void free_entity(entity* ent)
{
	if (ent->has_model)
	{
		free_mesh(&ent->_mesh);
		free_material(&ent->_material);
	}
}
