#include "chunk_mesh_data.h"
#include <glad/glad.h>
#include <stdlib.h>

void eon_get_data_from_gl_buffer_handles(struct eon_chunk_mesh_data* mesh_data, unsigned int vp_buffer_id, unsigned int dl_buffer_id, unsigned int indices_buffer_id)
{
	if (mesh_data->index_size == 0 || mesh_data->vertex_data_size == 0)
	{
		return;
	}

	unsigned int* vp_data = malloc(mesh_data->vertex_data_size * sizeof(unsigned int));
	unsigned int* dl_data = malloc(mesh_data->vertex_data_size * sizeof(unsigned int));
	unsigned int* index_data = malloc(mesh_data->index_size * sizeof(unsigned int));

	glBindBuffer(GL_ARRAY_BUFFER, vp_buffer_id);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, mesh_data->vertex_data_size * sizeof(unsigned int), vp_data);

	glBindBuffer(GL_ARRAY_BUFFER, dl_buffer_id);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, mesh_data->vertex_data_size * sizeof(unsigned int), dl_data);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer_id);
	glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, mesh_data->index_size * sizeof(unsigned int), index_data);

	mesh_data->vertex_position_data = vp_data;
	mesh_data->dir_light_data = dl_data;
	mesh_data->indices = index_data;
}

void eon_chunk_mesh_data_free(struct eon_chunk_mesh_data* mesh_data)
{
	free(mesh_data->vertex_position_data);
	mesh_data->vertex_position_data = NULL;
	free(mesh_data->dir_light_data);
	mesh_data->dir_light_data = NULL;
	free(mesh_data->indices);
	mesh_data->indices = NULL;

	mesh_data->vertex_data_size = 0;
	mesh_data->index_size = 0;
}
