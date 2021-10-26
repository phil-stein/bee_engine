#include "phys_dynamics.h"

#include "CGLM/cglm.h"


const vec3 gravity = { 0.0f, -9.89f, 0.0f };

// ---- dynamics ----

void simulate_dynamics(entity* e, f32 dt)
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
	vec3 delta_t = { dt, dt, dt };
	glm_vec3_mul(fdm, delta_t, fdm);
	glm_vec3_add(e->rb.velocity, fdm, e->rb.velocity);

	// position += velocity * delta_t
	vec3 v;
	glm_vec3_mul(e->rb.velocity, delta_t, v);
	glm_vec3_add(e->pos, v, e->pos);

	// reset force to (0, 0, 0)
	glm_vec3_copy(GLM_VEC3_ZERO, e->rb.force);

	// debug-line
#ifdef EDITOR_ACT
	vec3 pos, rot, scale, v_scaled, v_pos;
	get_entity_global_transform(e->id, pos, rot, scale);
	glm_vec3_copy(e->rb.velocity, v_scaled);
	vec3_mul_f(v_scaled, 0.2f);
	glm_vec3_add(pos, v_scaled, v_pos);
	debug_draw_line(pos, v_pos);
#endif
}
