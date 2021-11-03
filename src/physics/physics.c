#include "physics.h"

// #include "CGLM/cglm.h"
#include "stb/stb_ds.h"

#include "types/entities.h"
#include "game_time.h"
#include "renderer.h"

#include "phys_dynamics.h"
#include "phys_response.h"
#include "phys_collision.h"


void physics_init()
{
	// ...
}

void physics_update(f32 dt)
{

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
		simulate_dynamics(e1, dt);

		
		// ---- collision ----
		// test with all other colliders
		for (int i = 0; i < entity_ids_len; ++i) // array of colliders
		{
			if (i == n) { continue; }
			entity* e2 = get_entity(entity_ids[i]);
			if (!e2->has_collider) { continue; }

			collision_info c = collision_check(e1, e2);


			// ---- collision response ----
			if (c.collision)
			{
				#ifdef EDITOR_ACT
				vec3 scale;  
				glm_vec3_fill(scale, 0.2f);
				debug_draw_sphere(e1->pos, scale);
				#endif
				
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

