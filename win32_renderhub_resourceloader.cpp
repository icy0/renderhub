#include <stdio.h>
#include <string>
#include <assert.h>

#include "renderhub_types.h"
#include "renderhub_assert.h"
#include "renderhub_logging.h"

// TODO(paul):	only supports triangulated models. 
//				extend functionality to read quad models.

// TODO(paul):	maybe a function to triangulate quad models.

// TODO(paul):	function and data structures for reading materials
//				and linking them to their models.

fvec3 parse_vector(const char* line, int32 offset)
{
	fvec3 coordinates{0, 0, 0};

	const uint32 number_buffer_size = 256;
	char number_buffer[number_buffer_size];
	ZeroMemory(number_buffer, number_buffer_size);

	int32 number_buffer_index = 0;
	int32 coordinate_beginning = 0;
	int32 coordinate_iterator = 0;

	while (line[offset] != '\0')
	{
		coordinate_beginning = offset;
		while (line[offset] != ' ' && line[offset] != '\0')
		{
			rh_assert(number_buffer_index < number_buffer_size);
			number_buffer[number_buffer_index] = line[offset];
			offset++;
			number_buffer_index++;
		}
		rh_assert((offset - coordinate_beginning) < number_buffer_size);
		number_buffer[offset - coordinate_beginning] = '\0';
		offset++;

		coordinates.coords[coordinate_iterator] = std::stof(number_buffer);
		coordinate_iterator++;

		ZeroMemory(number_buffer, number_buffer_size);

		number_buffer_index = 0;
	}

	return coordinates;
}

OBJ_Face parse_face(const char* line)
{
	OBJ_Face face = {};

	const uint32 number_buffer_size = 256;
	char number_buffer[number_buffer_size];
	ZeroMemory(number_buffer, number_buffer_size);

	int32 offset = 2;
	int32 number_buffer_index = 0;
	int32 coordinate_beginning = 0;
	int32 component_identifier = 0;
	int32 group_identifier = 0;

	ivec3 components;

	// iterate over entire line
	while (line[offset] != '\0')
	{
		coordinate_beginning = offset;
		// iterate over v/vt/vn group
		while (line[offset] != ' ' && line[offset] != '\0')
		{
			// parse components of group
			while (line[offset] != '/' && line[offset] != ' ' && line[offset] != '\0')
			{
				rh_assert(number_buffer_index < number_buffer_size);
				number_buffer[number_buffer_index] = line[offset];
				offset++;
				number_buffer_index++;
			}
			if(number_buffer != "")
			{
				rh_assert((offset - coordinate_beginning) < number_buffer_size);
				number_buffer[offset - coordinate_beginning] = '\0';
				components.coords[component_identifier] = std::stoi(number_buffer);
			}
			if (line[offset] != ' ')
			{
				component_identifier++;
				offset++;

				ZeroMemory(number_buffer, number_buffer_size);
				number_buffer_index = 0;
			}
		}

		CopyMemory(face.vertices[group_identifier].coords, components.coords, sizeof(ivec3));
		group_identifier++;
		assert(group_identifier <= 3);

		component_identifier = 0;
		offset++;

		ZeroMemory(number_buffer, number_buffer_size);
		number_buffer_index = 0;
	}

	return face;
}

OBJ_Model* win32_read_obj(const char* filename)
{
	HANDLE file_handle;
	ZeroMemory(&file_handle, sizeof(HANDLE));

	char* file_buffer;
	LARGE_INTEGER file_size_in_bytes = {};
	file_handle = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	rh_assert(file_handle);

	GetFileSizeEx(file_handle, &file_size_in_bytes);
	rh_assert(file_size_in_bytes.QuadPart);

	file_buffer = new char[file_size_in_bytes.QuadPart];
	rh_assert(file_buffer);

	DWORD bytes_read; // note(paul): necessary for function call, won't be used afterwards.
	rh_assert(ReadFile(file_handle, file_buffer, file_size_in_bytes.QuadPart, &bytes_read, NULL));

	OBJ_Model* model = new OBJ_Model{};
	rh_assert(model);

	const int32 line_length = 256;
	char line[line_length];
	char* incr_file_pointer = file_buffer;
	char* incr_line_pointer = line;
	
	char* end_of_file_pointer = file_buffer + file_size_in_bytes.QuadPart;
	rh_assert(end_of_file_pointer > file_buffer);

	// TODO(paul): compress these identical while loops somehow
	while (incr_file_pointer < end_of_file_pointer)
	{
		ZeroMemory(line, line_length);

		// note(paul): writing current line into line-array
		while (*incr_file_pointer != '\n')
		{
			*incr_line_pointer = *incr_file_pointer;
			incr_file_pointer++;
			incr_line_pointer++;
			rh_assert((incr_line_pointer - line) < line_length);
		}

		*(incr_line_pointer - 1) = '\0';

		switch (*line)
		{
		//case('o'): // object name
		//{
		//	break;
		//}
		case('v'):
		{
			switch (*(line + 1))
			{
			case(' '): // line holds geometric vertex
			{
				model->vertex_positions_count++;
				break;
			}
			case('t'): // line holds texture vertex
			{
				model->vertex_texcoords_count++;
				break;
			}
			case('n'): // line holds vertex normals
			{
				model->vertex_normals_count++;
				break;
			}
			}
			break;
		}
		case('f'): // face
		{
			model->face_count++;
			break;
		}
		case('m'):
		{
			const char* material_library_identifier = "mtllib";
			bool8 is_mtllib_line = true;
			for (uint8 character = 0; character < 6; character++)
			{
				if (*(line + character) != *(material_library_identifier + character))
				{
					is_mtllib_line = false;
					break;
				}
			}
			if (is_mtllib_line)
			{
				model->flags |= OBJ_Model::MTLLIB_PRESENT;
			}
			else
			{
				// can only be "mg" for merging group
			}

			break;
		}
		}

		incr_file_pointer++;
		incr_line_pointer = line;
	}

	char msg[256];
	sprintf_s(msg, "model \"%s\" statistics:\n\tvertices: %lld\n\tvertex normals: %lld\n\tvertex texcoords: %lld\n\tfaces: %lld",
		filename, 
		model->vertex_positions_count, 
		model->vertex_normals_count, 
		model->vertex_texcoords_count, 
		model->face_count);
	rh_log_message(msg);

	model->vertex_positions = new fvec3[model->vertex_positions_count];
	rh_assert(model->vertex_positions);

	model->vertex_texcoords = new fvec3[model->vertex_texcoords_count];
	rh_assert(model->vertex_texcoords);

	model->vertex_normals = new fvec3[model->vertex_normals_count];
	rh_assert(model->vertex_normals);

	model->faces = new OBJ_Face[model->face_count];
	rh_assert(model->faces);

	incr_file_pointer = file_buffer;
	incr_line_pointer = line;

	uint32 position_index = 0;
	uint32 texcoord_index = 0;
	uint32 normal_index = 0;
	uint32 face_index = 0;

	while (incr_file_pointer < end_of_file_pointer)
	{
		ZeroMemory(line, line_length);

		while (*incr_file_pointer != '\n')
		{
			*incr_line_pointer = *incr_file_pointer;
			incr_file_pointer++;
			incr_line_pointer++;
			rh_assert((incr_line_pointer - line) < line_length);
		}

		*(incr_line_pointer - 1) = '\0';

		switch (*line)
		{
			//case('o'): // object name
			//{
			//	break;
			//}
		case('v'):
		{
			switch (*(line + 1))
			{
			case(' '): // line holds geometric vertex
			{
				model->vertex_positions[position_index] = parse_vector(line, sizeof("v ") - 1);
				position_index++;
				break;
			}
			case('t'): // line holds texture vertex
			{
				model->vertex_texcoords[texcoord_index] = parse_vector(line, sizeof("vt ") - 1);
				texcoord_index++;
				break;
			}
			case('n'): // line holds vertex normals
			{
				model->vertex_normals[normal_index] = parse_vector(line, sizeof("vn ") - 1);
				normal_index++;
				break;
			}
			}
			break;
		}
		case('f'): // face
		{
			model->faces[face_index] = parse_face(line);
			face_index++;
			break;
		}
		//case('m'):
		//{
		//	const char* material_library_identifier = "mtllib";
		//	bool8 is_mtllib_line = true;
		//	for (uint8 character = 0; character < 6; character++)
		//	{
		//		if (*(line + character) != *(material_library_identifier + character))
		//		{
		//			is_mtllib_line = false;
		//			break;
		//		}
		//	}
		//	if (is_mtllib_line)
		//	{
		//		// handle material library reading here
		//	}
		//	else
		//	{
		//		// can only be "mg" for merging group
		//	}

		//	break;
		//}
		}

		incr_file_pointer++;
		incr_line_pointer = line;
	}

	delete[] file_buffer;
	return model;
}