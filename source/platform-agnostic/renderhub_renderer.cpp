#include "win32_renderhub_globals.h"

#include "renderhub_types.h"
#include "renderhub_renderer.h"

void render_world()
{
	for (uint64 entity_index = 0; entity_index < g_world->entity_count; entity_index++)
	{
		render_entity(&g_world->entities[entity_index]);
	}
}

void render_entity(Entity*)
{

}