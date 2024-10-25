#ifndef CHUNK_MESH_DATA_H
#define CHUNK_MESH_DATA_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

	struct eon_chunk_mesh_data
	{
		unsigned int* vertex_position_data;
		unsigned int* dir_light_data;
		unsigned int* indices;
		size_t vertex_data_size;
		size_t index_size;
	};

	//vp_buffer_id: the opengl buffer handle for the vertex position buffer
	//dl_buffer_id: the opengl buffer handle for the dir light buffer
	void eon_get_data_from_gl_buffer_handles(struct eon_chunk_mesh_data* mesh_data, unsigned int vp_buffer_id, unsigned int dl_buffer_id, unsigned int indices_buffer_id);

	void eon_chunk_mesh_data_free(struct eon_chunk_mesh_data* mesh_data);

#ifdef __cplusplus
}
#endif

#endif // CHUNK_MESH_DATA_H
