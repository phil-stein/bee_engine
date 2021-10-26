#include "phys_collision.h"

#include "CGLM/cglm.h"


// ---- collision checks ----

collision_info collision_check(entity* e1, entity* e2)
{
	if (!e1->has_collider || !e2->has_collider) { collision_info c; c.collision = BEE_FALSE; return c; }
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

	return c;
}


collision_info collision_check_sphere_v_sphere(entity* s1, entity* s2)
{
	vec3 pos01 = VEC3_ZERO_INIT;
	vec3 pos02 = VEC3_ZERO_INIT;
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