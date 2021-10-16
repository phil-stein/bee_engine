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

			collision_info c;
			c.collision = BEE_FALSE;

			// same v same
			if (e1->collider.type == SPHERE_COLLIDER && e2->collider.type == SPHERE_COLLIDER)
			{
				c = collision_check_sphere_v_sphere(e1, e2);
			}
			else if (e1->collider.type == BOX_COLLIDER && e2->collider.type == BOX_COLLIDER)
			{
				c = collision_check_aabb_v_aabb(e1, e2);
			}
			// box v sphere
			else if (e1->collider.type == BOX_COLLIDER && e2->collider.type == SPHERE_COLLIDER)
			{
				c = collision_check_aabb_v_sphere(e1, e2);
			}
			else if (e1->collider.type == SPHERE_COLLIDER && e2->collider.type == BOX_COLLIDER)
			{
				c = collision_check_aabb_v_sphere(e2, e1);
			}

			// ---- collision response ----
			if (c.collision)
			{
				// printf("EUREKA :)\n");
				printf(" -> \"%s\" v. \"%s\"\n", e1->name, e2->name);
				printf(" -> normal: %.2f, %.2f, %.2f\n", c.normal[0], c.normal[1], c.normal[2]);
				printf(" -> depth: %.2f\n", c.penetration_depth);
				// submit_txt_console("EUREKA :)");

				collision_response(e1, e2, c);
			}

		}
	}
}

// ---- dynamics ----

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

// ---- collision checks ----

collision_info collision_check_sphere_v_sphere(entity* s1, entity* s2)
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
	collision_info info;
	info.collision = collision;
	info.penetration_depth = dist;

	vec3 normal;
	glm_vec3_sub(s1->pos, s2->pos, normal);
	glm_vec3_normalize(normal);
	glm_vec3_copy(normal, info.normal);
	return info;
}

collision_info collision_check_aabb_v_aabb(entity* b1, entity* b2)
{
	// add position & offset to min & max of both colliders
	vec3 a_min; vec3 a_max;
	glm_vec3_copy(b1->collider.box.aabb[0], a_min);
	glm_vec3_copy(b1->collider.box.aabb[1], a_max);
	glm_vec3_add(a_min, b1->pos, a_min);
	glm_vec3_add(a_max, b1->pos, a_max);
	glm_vec3_add(a_min, b1->collider.offset, a_min);
	glm_vec3_add(a_max, b1->collider.offset, a_max);
	vec3 b_min; vec3 b_max;
	glm_vec3_copy(b2->collider.box.aabb[0], b_min);
	glm_vec3_copy(b2->collider.box.aabb[1], b_max);
	glm_vec3_add(b_min, b2->pos, b_min);
	glm_vec3_add(b_max, b2->pos, b_max);
	glm_vec3_add(a_min, b2->collider.offset, a_min);
	glm_vec3_add(a_max, b2->collider.offset, a_max);

	bee_bool collision = (a_min[0] <= b_max[0] && a_max[0] >= b_min[0]) &&
						 (a_min[1] <= b_max[1] && a_max[1] >= b_min[1]) &&
						 (a_min[2] <= b_max[2] && a_max[2] >= b_min[2]);

	collision_info info;
	info.collision = collision;
	vec3 center1, center2;
	f32 dist_x = a_max[0] - b_min[0];
	f32 dist_y = a_max[1] - b_min[1];
	f32 dist_z = a_max[2] - b_min[2];
	f32 dist = dist_x >= dist_y && dist_x >= dist_z ? dist_x :
			   dist_y >= dist_x && dist_y >= dist_z ? dist_y :
			   dist_z >= dist_x && dist_z >= dist_y ? dist_z : 0;
	assert(dist);

	info.penetration_depth = dist * 0.001f;

	vec3 normal;
	vec3 x = { 1, 0, 0 };
	vec3 y = { 0, 1, 0 };
	vec3 z = { 0, 0, 1 };
	if (dist == dist_x) { glm_vec3_copy(y, normal); }
	if (dist == dist_y) { glm_vec3_copy(z, normal); }
	if (dist == dist_z) { glm_vec3_copy(x, normal); }
	glm_vec3_copy(normal, info.normal);
	
	return info;
}

collision_info collision_check_aabb_v_sphere(entity* b, entity* s)
{
	// @TODO
}

// ---- collision response ----

void collision_response(entity* e1, entity* e2, collision_info info)
{
	if (!e1->has_rb && !e2->has_rb) { return; }


	// calc the force each collider enacted on the other

	// fill the vars according to rb or static
	f32  mass1 = 0;
	f32  mass2 = 0;
	vec3 velocity1;
	vec3 velocity2;
	if (e1->has_rb)
	{
		mass1 = e1->rb.mass;
		glm_vec3_copy(e1->rb.velocity, velocity1);
	}
	else // static
	{
		mass1 = e2->rb.mass;
		glm_vec3_copy(GLM_VEC3_ZERO, velocity1);
	}
	if (e2->has_rb)
	{
		mass2 = e2->rb.mass;
		glm_vec3_copy(e2->rb.velocity, velocity2);
	}
	else // static
	{
		mass2 = mass1;
		glm_vec3_copy(GLM_VEC3_ZERO, velocity2);
	}
	// printf("mass1: %.2f, mass2: %.2f\n", mass1, mass2);
	// printf("v1: x: %.2f, y: %.2f, z: %.2f\n", velocity1[0], velocity1[1], velocity1[2]);
	// printf("v2: x: %.2f, y: %.2f, z: %.2f\n", velocity2[0], velocity2[1], velocity2[2]);

	// see: elastic collisions, one-dimensional newtonian

	// as long as both masses are 1 / equal you can swap velocities
	// vec3 s1_v;
	// glm_vec3_copy(s1->rb.velocity, s1_v);
	// glm_vec3_copy(s2->rb.velocity, s1->rb.velocity);
	// glm_vec3_copy(s1_v, s2->rb.velocity);


	// velocity1 formula
	// v1,2 = current velocity, m1,2 = mass
	// ((m1 - m2) / (m1 + m2)) * v1
	// +
	// ((2 * m2) / (m1 + m2)) * v2
	f32 m1_m_m2 = mass1 - mass2;
	f32 m1_p_m2 = mass1 + mass2;
	f32 a1_f = (m1_m_m2 / m1_p_m2);
	vec3 a1 = { a1_f, a1_f, a1_f };
	glm_vec3_mul(velocity1, a1, a1);
	f32 b1_f = (2 * mass2) / m1_p_m2;
	vec3 b1 = { b1_f, b1_f, b1_f };
	glm_vec3_mul(velocity2, b1, b1);

	vec3 v1;
	glm_vec3_add(a1, b1, v1);

	// velocity2 formula
	// ((2 * m1) / (m1 + m2)) * v1
	// +
	// ((m2 - m1) / (m1 + m2)) * v2
	f32 a2_f = (2 * mass1) / m1_p_m2;
	vec3 a2 = { a2_f, a2_f, a2_f };
	glm_vec3_mul(velocity1, a2, a2);
	f32 b2_f = (mass2 - mass1) / m1_p_m2;
	vec3 b2 = { b2_f, b2_f, b2_f };
	glm_vec3_mul(velocity2, b2, b2);

	vec3 v2;
	glm_vec3_add(a2, b2, v2);

	// set velocity
	if (e1->has_rb)
	{
		glm_vec3_copy(v1, e1->rb.velocity);
		// printf("s1 v: x: %.2f, y: %.2f, z: %.2f\n", e1->rb.velocity[0], e1->rb.velocity[1], e1->rb.velocity[2]);
	}
	if (e2->has_rb)
	{
		glm_vec3_copy(v2, e2->rb.velocity);
		// printf("s2 v: x: %.2f, y: %.2f, z: %.2f\n", e2->rb.velocity[0], e2->rb.velocity[1], e2->rb.velocity[2]);
	}

	f32 dist = info.penetration_depth;

	if (e1->has_rb && e2->has_rb)
	{
		// move back according to mass
		f32 s1_ratio = 0;
		f32 s2_ratio = 0;

		if (e1->rb.mass > e2->rb.mass)
		{
			f32 ratio = e2->rb.mass / e1->rb.mass;
			s1_ratio = ratio;
			s2_ratio = 1 - ratio;
		}
		else if (e1->rb.mass < e2->rb.mass)
		{
			f32 ratio = e2->rb.mass / e2->rb.mass;
			s1_ratio = 1 - ratio;
			s2_ratio = ratio;
		}
		else if (e1->rb.mass == e2->rb.mass)
		{
			s1_ratio = 0.5;
			s2_ratio = 0.5;
		}
		f32 s1_dist = dist * s1_ratio;
		vec3 offset;
		vec3 s1_dist_vec = { s1_dist, s1_dist, s1_dist };
		glm_vec3_mul(info.normal, s1_dist_vec, offset);
		glm_vec3_add(offset, e1->pos, e1->pos);

		f32 s2_dist = dist * s2_ratio;
		vec3 s2_dist_vec = { s2_dist, s2_dist, s2_dist };
		vec3 norm_inv;
		glm_vec3_copy(info.normal, norm_inv);
		glm_vec3_negate(norm_inv);
		glm_vec3_mul(info.normal, s2_dist_vec, offset);
		glm_vec3_add(offset, e2->pos, e2->pos);

	}
	else if (!e1->has_rb && e2->has_rb)
	{
		// only move back the rb
		vec3 offset;
		vec3 norm_inv;
		glm_vec3_copy(info.normal, norm_inv);
		glm_vec3_negate(norm_inv);
		vec3 dist_vec = { dist, dist, dist };
		glm_vec3_mul(info.normal, dist_vec, offset);
		glm_vec3_add(offset, e2->pos, e2->pos);

	}
	else if (e1->has_rb && !e2->has_rb)
	{
		// only move back the rb
		vec3 offset;
		vec3 dist_vec = { dist, dist, dist };
		glm_vec3_mul(info.normal, dist_vec, offset);
		glm_vec3_add(offset, e1->pos, e1->pos);
	}


	// // same v same
	// if (e1->collider.type == SPHERE_COLLIDER && e2->collider.type == SPHERE_COLLIDER)
	// {
	// 	collision_response_sphere_v_sphere(e1, e2);
	// }
	// else if (e1->collider.type == BOX_COLLIDER && e2->collider.type == BOX_COLLIDER)
	// {
	// }
	// // box v sphere
	// else if (e1->collider.type == BOX_COLLIDER && e2->collider.type == SPHERE_COLLIDER)
	// {
	// }
	// else if (e1->collider.type == SPHERE_COLLIDER && e2->collider.type == BOX_COLLIDER)
	// {
	// }
}

