#include "phys_response.h"

#include "CGLM/cglm.h"

// ---- collision response ----

void collision_response(entity* e1, entity* e2, collision_info info)
{
	if (!e1->has_rb) { return; }


	// move the objects out of one another
	collision_response_resolve_position(e1, e2, info);

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