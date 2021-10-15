#include "physics.h"

#include "entities.h"
#include "game_time.h"
#include "renderer.h"
#include "editor_ui.h" // tmp


// smaller as a scalar on speed
const vec3 gravity = { 0.0f, -9.89f, 0.0f }; // look this up, i aint no physisyscsky man

void physics_init()
{

}

void physics_update()
{
	if (!get_gamestate()) { return; }

	int  entity_ids_len = 0;
	int* entity_ids = get_entity_ids(&entity_ids_len);

	// go through all objs
	// @TODO: make array like for lights
	for (int n = 0; n < entity_ids_len; ++n)
	{
		entity* e1 = get_entity(entity_ids[n]);
		
		// ---- dynamics ----
		if (e1->has_rb)
		{
			simulate_dynamics(e1);
		}

		if (!e1->has_collider) { continue; }
		
		// ---- collision ----
		// test with all other colliders
		for (int i = 0; i < entity_ids_len; ++i)
		{
			if (i == n) { continue; }
			entity* e2 = get_entity(entity_ids[i]);
			if (!e2->has_collider) { continue; }

			bee_bool c = collision_check_sphere_v_sphere(e1, e2);
			if (c)
			{
				printf("EUREKA :)\n");
				printf(" -> \"%s\" v. \"%s\"\n", e1->name, e2->name);
				submit_txt_console("EUREKA :)");
			}

		}
	}

}

void simulate_dynamics(entity* e)
{
	// add mass * gravity to force
	vec3 mass = { e->rb.mass, e->rb.mass , e->rb.mass };
	vec3 mg;
	glm_vec3_mul(mass, gravity, mg);
	glm_vec3_add(e->rb.force, mg, e->rb.force);

	// add (force / mass) * delta_t to velocity
	vec3 fdm; 
	glm_vec3_div(e->rb.force, mass, fdm);
	f32 dt = get_delta_time();
	vec3 delta_t = { dt, dt, dt };
	glm_vec3_mul(fdm, delta_t, fdm);
	glm_vec3_add(e->rb.velocity, fdm, e->rb.velocity);

	// position += velocity * delta_t
	vec3 v;
	glm_vec3_mul(e->rb.velocity, delta_t, v);
	glm_vec3_add(e->pos, v, e->pos);

	// reset force to (0, 0, 0)
	glm_vec3_copy(GLM_VEC3_ZERO, e->rb.force);
}

bee_bool collision_check_sphere_v_sphere(entity* s1, entity* s2)
{
	vec3 pos01 = GLM_VEC3_ZERO_INIT;
	vec3 pos02 = GLM_VEC3_ZERO_INIT;
	vec3 rot, scale;
	get_entity_global_transform(s1->id, pos01, rot, scale);
	get_entity_global_transform(s2->id, pos02, rot, scale);
	glm_vec3_add(pos01, s1->collider.offset, pos01);
	glm_vec3_add(pos02, s2->collider.offset, pos02);
	f32 dist = glm_vec3_distance(pos01, pos02);

	bee_bool collision = dist < s1->collider.sphere.radius + s2->collider.sphere.radius;
	return collision;
}
