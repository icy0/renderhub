#include <stdio.h>
#include <string>

#include "renderhub_types.h"

ivec3 parse_vector(const char* line, int32 offset)
{
	ivec3 coordinates{0, 0, 0};

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
			number_buffer[number_buffer_index] = line[offset];
			offset++;
			number_buffer_index++;
		}
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

	int32 components[3];

	// TODO keep track of v/vt/vn groups per group_identifier
	// TODO copy component info into OBJ_Face face

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
				number_buffer[number_buffer_index] = line[offset];
				offset++;
				number_buffer_index++;
			}
			if(number_buffer != "")
			{
				number_buffer[offset - coordinate_beginning] = '\0';
				components[component_identifier] = std::stoi(number_buffer);
			}
			if (line[offset] != ' ')
			{
				component_identifier++;
				offset++;

				ZeroMemory(number_buffer, number_buffer_size);
				number_buffer_index = 0;
			}

		}
		component_identifier = 0;
		offset++;

		ZeroMemory(number_buffer, number_buffer_size);
		number_buffer_index = 0;
	}

	return face;
}

void win32_read_obj(const char* filename)
{
	HANDLE file_handle;
	ZeroMemory(&file_handle, sizeof(HANDLE));
	char* file_buffer;
	LARGE_INTEGER file_size_in_bytes;
	if (file_handle = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
	{
		GetFileSizeEx(file_handle, &file_size_in_bytes);

		file_buffer = new char[file_size_in_bytes.QuadPart];
		DWORD bytes_read;
		ReadFile(file_handle, file_buffer, file_size_in_bytes.QuadPart, &bytes_read, NULL);

		// OutputDebugStringA(file_buffer);

		uint64 vpos_count = 0;
		uint64 vnormal_count = 0;
		uint64 vtexcoords_count = 0;
		uint64 face_count = 0;

		const int32 line_length = 256;
		char line[line_length];
		char* incr_file_pointer = file_buffer;
		char* incr_line_pointer = line;
		
		char* end_of_file_pointer = file_buffer + file_size_in_bytes.QuadPart;

		while (incr_file_pointer < end_of_file_pointer)
		{
			ZeroMemory(line, line_length);

			while (*incr_file_pointer != '\n')
			{
				*incr_line_pointer = *incr_file_pointer;
				incr_file_pointer++;
				incr_line_pointer++;
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
					vpos_count++;
					break;
				}
				case('t'): // line holds texture vertex
				{
					vtexcoords_count++;
					break;
				}
				case('n'): // line holds vertex normals
				{
					vnormal_count++;
					break;
				}
				}
				break;
			}
			case('f'): // face
			{
				face_count++;
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

		char msg[256];
		sprintf_s(msg, "vertices: %d, vertex normals: %d, vertex texture coordinates: %d, faces: %d", 
			vpos_count, vnormal_count, vtexcoords_count, face_count);
		OutputDebugStringA(msg);

		fvec3* vertex_positions = new fvec3[vpos_count];
		fvec3* vertex_normals = new fvec3[vnormal_count];
		fvec2* vertex_texcoords = new fvec2[vtexcoords_count];
		fvec3* faces = new fvec3[face_count];

		incr_file_pointer = file_buffer;
		incr_line_pointer = line;

		while (incr_file_pointer < end_of_file_pointer)
		{
			ZeroMemory(line, line_length);

			while (*incr_file_pointer != '\n')
			{
				*incr_line_pointer = *incr_file_pointer;
				incr_file_pointer++;
				incr_line_pointer++;
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
					ivec3 coords = parse_vector(line, sizeof("v ") - 1);
					break;
				}
				case('t'): // line holds texture vertex
				{
					parse_vector(line, sizeof("vt ") - 1);
					break;
				}
				case('n'): // line holds vertex normals
				{
					parse_vector(line, sizeof("vn ") - 1);
					break;
				}
				}
				break;
			}
			case('f'): // face
			{
				parse_face(line);
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
	}
}