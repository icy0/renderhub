#include "renderhub_types.h"
#include "renderhub_assert.h"

Mesh* convert_to_mesh(OBJ_Model* model)
{
	Mesh* mesh = new Mesh();
	rh_assert(mesh);

	rh_assert(/*actual vertex count*/ (model->face_count * 3) <= 
		/*max vertex count*/ ((uint64)1) << ((sizeof(mesh->vertex_count) * 8) - 1));

	mesh->vertex_count = model->face_count * (uint64)3;
	mesh->vertices = new vertex[mesh->vertex_count];
	rh_assert(mesh->vertices);

	// note(paul): for now, these flags are just assumed to be set, therefore they are hardcoded.
	// I might demand every .obj for its faces to be triangulated, to be determined.
	mesh->flags |= Mesh::TRIANGULATED | Mesh::BACKFACE_CULLING | Mesh::TRIANGLE_LIST;

	uint32 vertex_counter = 0;
	
	// note(paul): This introduces duplicates (triangle list).
	for (int32 face_index = 0; face_index < model->face_count; face_index++)
	{
		for (int32 vertex_of_face_index = 0; vertex_of_face_index < 3; vertex_of_face_index++)
		{
			ivec3 indices = model->faces[face_index].vertices[vertex_of_face_index];

			mesh->vertices[vertex_counter].position = model->vertex_positions[indices.x];
			mesh->vertices[vertex_counter].texcoords = { model->vertex_texcoords[indices.y].x, model->vertex_texcoords[indices.y].y };
			mesh->vertices[vertex_counter].normal = model->vertex_normals[indices.z];
			vertex_counter++;
			rh_assert(vertex_counter <= mesh->vertex_count);
		}
	}

	return mesh;
}