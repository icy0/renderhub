#include "renderhub_types.h"
#include "renderhub_assert.h"

mesh* convert_to_mesh(obj_model* model)
{
	mesh* new_mesh = new mesh();
	rh_assert(new_mesh);

	rh_assert(/*actual vertex count*/ (model->face_count * 3) <= 
		/*max vertex count*/ ((uint64)1) << ((sizeof(new_mesh->vertex_count) * 8) - 1));

	new_mesh->vertex_count = model->face_count * (uint64)3;
	new_mesh->vertices = new vertex[new_mesh->vertex_count];
	rh_assert(new_mesh->vertices);

	// note(paul): for now, these flags are just assumed to be set, therefore they are hardcoded.
	// I might demand every .obj for its faces to be triangulated, to be determined.
	new_mesh->flags |= mesh::TRIANGULATED | mesh::BACKFACE_CULLING | mesh::TRIANGLE_LIST;

	uint32 vertex_counter = 0;
	
	// note(paul): This introduces duplicates (triangle list).
	for (int32 face_index = 0; face_index < model->face_count; face_index++)
	{
		for (int32 vertex_of_face_index = 0; vertex_of_face_index < 3; vertex_of_face_index++)
		{
			ivec3 indices = model->faces[face_index].vertices[vertex_of_face_index];

			new_mesh->vertices[vertex_counter].position = model->vertex_positions[indices.x];
			new_mesh->vertices[vertex_counter].texcoords = { model->vertex_texcoords[indices.y].x, model->vertex_texcoords[indices.y].y };
			new_mesh->vertices[vertex_counter].normal = model->vertex_normals[indices.z];
			vertex_counter++;
			rh_assert(vertex_counter <= new_mesh->vertex_count);
		}
	}

	return new_mesh;
}