#include "physics.h"

#include "stb/stb_ds.h"

#include "entities.h"
#include "renderer.h"
#include "game_time.h"
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
	for (int n = 0; n < entity_ids_len; ++n) // array of rigidbodies
	{
		entity* e1 = get_entity(entity_ids[n]);
		
		// if (!e1->has_rb) { continue; }
		if (!e1->has_collider) { continue; }
		
		// ---- dynamics ----
		simulate_dynamics(e1);

		
		// ---- collision ----
		// test with all other colliders
		for (int i = 0; i < entity_ids_len; ++i) // array of colliders
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
				vec3 scale;  
				glm_vec3_fill(scale, 0.2f);
				vec3 pos; 
				glm_vec3_copy(e1->pos, pos);
				// pos[1] += e1->scale[1] * 0.5f;
				debug_draw_sphere(pos, scale);

				// notify objects of collision

				c.trigger = e1->collider.is_trigger || e2->collider.is_trigger;

				c.entity_id = e2->id;
				arrput(e1->collider.infos, c);
				e1->collider.infos_len++;

				c.entity_id = e1->id;
				arrput(e2->collider.infos, c);
				e2->collider.infos_len++;

				if (!c.trigger && e1->has_rb) // no response on trigger collisions
				{
					// printf(" -> \"%s\" v. \"%s\"\n", e1->name, e2->name);
					// printf(" -> dir: %.2f, %.2f, %.2f\n", c.direction[0], c.direction[1], c.direction[2]);

					collision_response(e1, e2, c);
				}
			}

		}
	}
}

// ---- dynamics ----

void simulate_dynamics(entity* e)
{
	if (!e->has_rb) { return; }
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

#ifdef EDITOR_ACT
	vec3 pos, rot, scale, v_scaled, v_pos;
	get_entity_global_transform(e->id, pos, rot, scale);
	glm_vec3_copy(e->rb.velocity, v_scaled);  
	vec3_mul_f(v_scaled, 0.2f);
	glm_vec3_add(pos, v_scaled, v_pos);
	debug_draw_line(pos, v_pos);
#endif
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
	dist = dist - (s1->collider.sphere.radius + s2->collider.sphere.radius);

	collision_info info;
	info.collision = dist < 0;
	if (!info.collision) { return info; }

	vec3 dir;
	glm_vec3_sub(s2->pos, s1->pos, dir);
	f32 len = vec3_magnitude(dir);
	glm_vec3_normalize(dir);
	vec3_mul_f(dir, len);
	//vec3 d = { fabsf(dist), fabsf(dist), fabsf(dist) };
	//glm_vec3_mul(dir, d, dir);
	glm_vec3_copy(dir, info.direction);
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
	glm_vec3_add(b_min, b2->collider.offset, b_min);
	glm_vec3_add(b_max, b2->collider.offset, b_max);

	bee_bool collision = (a_min[0] <= b_max[0] && a_max[0] >= b_min[0]) &&
						 (a_min[1] <= b_max[1] && a_max[1] >= b_min[1]) &&
						 (a_min[2] <= b_max[2] && a_max[2] >= b_min[2]);

	collision_info info;
	info.collision = collision;
	if (!collision) { return info; }

	/*
	vec3 center1, center2;
	f32 dist_x = a_max[0] - b_min[0];
	f32 dist_y = a_max[1] - b_min[1];
	f32 dist_z = a_max[2] - b_min[2];
	f32 dist = dist_x > dist_y && dist_x > dist_z ? dist_x :
			   dist_y > dist_x && dist_y > dist_z ? dist_y :
			   dist_z > dist_x && dist_z > dist_y ? dist_z : 0;
	// assert(dist);

	f32 depth = dist * 0.1f;
	depth	  = depth < 0.01f ? 0.0f : depth; // cut off if too small
	info.penetration_depth = depth;

	vec3 normal;
	const vec3 x   = { 1, 0, 0 };
	const vec3 y   = { 0, 1, 0 };
	const vec3 z   = { 0, 0, 1 };
	const vec3 all = { 1, 1, 1 };
	if (b1->pos[0] >= b2->pos[0])
	{ 
		if (a_max[0] > b_max[0])
		{
			glm_vec3_copy(x, normal); 
			glm_vec3_negate(normal);
		}
	}
	else if (b2->pos[0] >= b1->pos[0])
	{
		if (b_max[0] > a_max[0])
		{
			glm_vec3_copy(x, normal);
		}
	}
	if (b1->pos[1] >= b2->pos[1])
	{
		if (a_max[1] > b_max[1])
		{
			glm_vec3_copy(y, normal);
			glm_vec3_negate(normal);
		}
	}
	else if (b2->pos[1] >= b1->pos[1])
	{
		if (b_max[1] > a_max[1])
		{
			glm_vec3_copy(y, normal);
		}
	}
	if (b1->pos[2] >= b2->pos[2])
	{
		if (a_max[2] > b_max[2])
		{
			glm_vec3_copy(z, normal);
			glm_vec3_negate(normal);
		}
	}
	else if (b2->pos[2] >= b1->pos[2])
	{
		if (b_max[2] > a_max[2])
		{
			glm_vec3_copy(z, normal);
		}
	}
	// else if (a_max[1] > b_max[1])
	// { 
	// 	glm_vec3_copy(y, normal); 
	// 	if (b1->pos[1] >= b2->pos[1]) { glm_vec3_negate(normal); }
	// }
	// else if (a_max[2] > b_max[2])
	// { 
	// 	glm_vec3_copy(z, normal); 
	// 	if (b1->pos[2] >= b2->pos[2]) { glm_vec3_negate(normal); }
	// }

	if (b1->has_rb && b2->has_rb)
	{
		// use velocity to figure out what the normal is
		// reflect velocity
	}
	else if (b1->has_rb)
	{

	}
	else if (b2->has_rb)
	{

	}

	if (dist == 0) { glm_vec3_copy(all, normal); }
	*/

	// calc the distance both ways
	vec3 dist1, dist2; 
	glm_vec3_sub(b_min, a_max, dist1);
	glm_vec3_sub(a_min, b_max, dist2);
	
	// get the larges distances for each axis
	vec3 dist = { dist1[0] >= dist2[0] ? dist1[0] : dist2[0],
				  dist1[1] >= dist2[1] ? dist1[1] : dist2[1],
				  dist1[2] >= dist2[2] ? dist1[2] : dist2[2] };
	// @TODO: collision could now also be max(dist) > 0

	vec3 dir;
	glm_vec3_sub(a_max, b_min, dir);
	

	// get axis by prior velocity
	vec3 axis = GLM_VEC3_ZERO_INIT;
	if (b1->has_rb)
	{
		// get biggest axis
		// int a = fabsf(b1->rb.velocity[0]) >= fabsf(b1->rb.velocity[1]) && fabsf(b1->rb.velocity[0]) >= fabsf(b1->rb.velocity[2]) ? 1 :
		// 		fabsf(b1->rb.velocity[1]) >= fabsf(b1->rb.velocity[0]) && fabsf(b1->rb.velocity[1]) >= fabsf(b1->rb.velocity[2]) ? 2 :
		// 		fabsf(b1->rb.velocity[2]) >= fabsf(b1->rb.velocity[0]) && fabsf(b1->rb.velocity[2]) >= fabsf(b1->rb.velocity[1]) ? 3 : 0;
		
		f32 x = fabsf(b1->rb.velocity[0]);
		f32 y = fabsf(b1->rb.velocity[1]);
		f32 z = fabsf(b1->rb.velocity[2]);

		int a = x >= y && x >= z ? 1 :
				y >= x && y >= z ? 2 :
				z >= x && z >= y ? 3 : 0;


		if (a == 1) // x
		{
			f32 dist_x = b_max[0] - a_min[0];
			vec3 v;
			glm_vec3_copy(b1->rb.velocity, v);
			glm_vec3_normalize(v);
			v[0] *= -1; // invert x axis
			vec3_mul_f(v, dist_x);
			glm_vec3_copy(v, axis);
		}
		else if (a == 2) // y
		{
			f32 dist_y = b_max[1] - a_min[1];
			vec3 v;
			glm_vec3_copy(b1->rb.velocity, v);
			glm_vec3_normalize(v);
			v[1] *= -1; // invert y axis
			vec3_mul_f(v, dist_y);
			glm_vec3_copy(v, axis);
			// printf(" -> dist_y: %.2f, b.y: %.2f\n", dist_y, b[1]);
		}
		else if (a == 3) // z
		{
			f32 dist_z = b_max[2] - a_min[2];
			vec3 v;
			glm_vec3_copy(b1->rb.velocity, v);
			glm_vec3_normalize(v);
			v[2] *= -1; // invert z axis
			vec3_mul_f(v, dist_z);
			glm_vec3_copy(v, axis);
		}
		
		// printf(" -> velocity check, x: %.2f, y: %.2f, z: %.2f, a; %d, v.x: %.2f, v.y: %.2f, v.z: %.2f\n", axis[0], axis[1], axis[2], a, b1->rb.velocity[0], b1->rb.velocity[1], b1->rb.velocity[2]);

	}
	else if (b2->has_rb && 0)
	{
		// // get biggest axis
		// glm_vec3_copy(fabsf(b2->rb.velocity[0]) >= fabsf(b2->rb.velocity[1]) && fabsf(b2->rb.velocity[0]) >= fabsf(b2->rb.velocity[2]) ? x :
		// 			  fabsf(b2->rb.velocity[1]) >= fabsf(b2->rb.velocity[0]) && fabsf(b2->rb.velocity[1]) >= fabsf(b2->rb.velocity[2]) ? y :
		// 			  fabsf(b2->rb.velocity[2]) >= fabsf(b2->rb.velocity[0]) && fabsf(b2->rb.velocity[2]) >= fabsf(b2->rb.velocity[1]) ? z : GLM_VEC3_ZERO, axis);
		// 
		// glm_vec3_negate(axis);
	}
	// else { printf(" -> velocity wasnt checked\n"); }


	glm_vec3_copy(axis, info.direction); // dist
	
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


	// move the objects out of one another
	collision_response_resolve_position(e1,e2, info);

	// adjust velocity accordingly
	collision_response_resolve_velocity(e1, e2, info);

}

void collision_response_resolve_position(entity* e1, entity* e2, collision_info info)
{
	// magnitude of vec3, didnt find a glm function
	// f32 dist = sqrtf((info.direction[0] * info.direction[0]) + 
	// 				 (info.direction[1] * info.direction[1]) + 
	// 				 (info.direction[2] * info.direction[2]));

	const f32 scalar = 0.005f; // @TODO: should not have to do this
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
		// f32 s1_dist = dist * s1_ratio;
		// vec3 offset;
		// vec3 s1_dist_vec = { s1_dist, s1_dist, s1_dist };
		// glm_vec3_mul(info.normal, s1_dist_vec, offset);
		
		glm_vec3_add(info.direction, e1->pos, e1->pos);

		// vec3 norm_inv;
		// glm_vec3_copy(info.direction, norm_inv);
		// glm_vec3_negate(norm_inv);
		// vec3_mul_f(norm_inv, scalar);
		// glm_vec3_add(norm_inv, e1->pos, e1->pos);

		// f32 s2_dist = dist * s2_ratio;
		// vec3 s2_dist_vec = { s2_dist, s2_dist, s2_dist };
		// glm_vec3_mul(info.normal, s2_dist_vec, offset);
		
		vec3 norm_inv;
		glm_vec3_copy(info.direction, norm_inv);
		glm_vec3_negate(norm_inv);
		glm_vec3_add(norm_inv, e2->pos, e2->pos);

		// glm_vec3_add(info.direction, e2->pos, e2->pos);

	}
	else if (!e1->has_rb && e2->has_rb)
	{
		// only move back the rb
		// vec3 offset;
		// vec3 norm_inv;
		// glm_vec3_copy(info.normal, norm_inv);
		// glm_vec3_negate(norm_inv);
		// vec3 dist_vec = { dist, dist, dist };
		// glm_vec3_mul(info.normal, dist_vec, offset);
		// glm_vec3_add(offset, e2->pos, e2->pos);

		glm_vec3_add(info.direction, e2->pos, e2->pos);

		vec3 norm_inv;
		glm_vec3_copy(info.direction, norm_inv);
		glm_vec3_negate(norm_inv);
		glm_vec3_add(norm_inv, e2->pos, e2->pos);
	}
	else if (e1->has_rb && !e2->has_rb)
	{
		// only move back the rb
		// vec3 offset;
		// vec3 dist_vec = { dist, dist, dist };
		// glm_vec3_mul(info.normal, dist_vec, offset);
		// glm_vec3_add(offset, e1->pos, e1->pos);

		// vec3 norm_inv;
		// glm_vec3_copy(info.direction, norm_inv);
		// glm_vec3_negate(norm_inv);
		// vec3_mul_f(norm_inv, scalar);
		// glm_vec3_add(norm_inv, e1->pos, e1->pos);

		glm_vec3_add(info.direction, e1->pos, e1->pos);
	}
}

void collision_response_resolve_velocity(entity* e1, entity* e2, collision_info info)
{
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
	if (e1->has_rb && !e2->has_rb)
	{
		// glm_vec3_copy(v1, e1->rb.velocity);
		 
		glm_vec3_copy(info.direction, v1);
		vec3_mul_f(v1, 10);
		glm_vec3_copy(v1, e1->rb.velocity);
		// printf("s1 v: x: %.2f, y: %.2f, z: %.2f\n", e1->rb.velocity[0], e1->rb.velocity[1], e1->rb.velocity[2]);
	}
	else if (e2->has_rb && !e1->has_rb) // never gets called currently
	{
		// glm_vec3_copy(v2, e2->rb.velocity);

		glm_vec3_copy(info.direction, v2);
		vec3_mul_f(v2, 10);
		glm_vec3_copy(v2, e2->rb.velocity);
		// printf("s2 v: x: %.2f, y: %.2f, z: %.2f\n", e2->rb.velocity[0], e2->rb.velocity[1], e2->rb.velocity[2]);
	}
	else if (e1->has_rb && e2->has_rb)
	{
		// glm_vec3_add(v1, e1->rb.velocity, e1->rb.velocity);
		// glm_vec3_add(v2, e2->rb.velocity, e2->rb.velocity);

		
		glm_vec3_copy(info.direction, v1);
		
		vec3 dir;
		glm_vec3_copy(info.direction, dir);
		glm_vec3_normalize(dir);

		glm_vec3_mul(v1, dir, v1);
		// vec3_mul_f(v1, 10);
		glm_vec3_copy(v1, e1->rb.velocity);


		glm_vec3_copy(info.direction, v2);

		glm_vec3_negate(dir);
		glm_vec3_mul(v2, dir, v2);
		// vec3_mul_f(v2, 10);
		// glm_vec3_negate(v2); // ???
		glm_vec3_copy(v2, e2->rb.velocity);

		printf("rb1: \"%s\", v: x: %.2f, y: %.2f, z: %.2f\n", e1->name, e1->rb.velocity[0], e1->rb.velocity[1], e1->rb.velocity[2]);
		printf("rb2: \"%s\", v: x: %.2f, y: %.2f, z: %.2f\n", e2->name, e2->rb.velocity[0], e2->rb.velocity[1], e2->rb.velocity[2]);
	}
}

