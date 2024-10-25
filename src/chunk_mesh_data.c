#include "chunk_mesh_data.h"
#include <glad/glad.h>
#include <stdlib.h>

void GetDataFromGlBufferHandles(CHUNK_MESH_DATA* MeshData, UINT VpBufferId, UINT DlBufferId, UINT IndicesBufferId)
{
	if (MeshData->IndexSize == 0 || MeshData->VertexDataSize == 0)
	{
		return;
	}

	UINT* vp_data = malloc(MeshData->VertexDataSize * sizeof(UINT));
	UINT* dl_data = malloc(MeshData->VertexDataSize * sizeof(UINT));
	UINT* index_data = malloc(MeshData->IndexSize * sizeof(UINT));

	glBindBuffer(GL_ARRAY_BUFFER, VpBufferId);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, MeshData->VertexDataSize * sizeof(UINT), vp_data);

	glBindBuffer(GL_ARRAY_BUFFER, DlBufferId);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, MeshData->VertexDataSize * sizeof(UINT), dl_data);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesBufferId);
	glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, MeshData->IndexSize * sizeof(UINT), index_data);

	MeshData->VertexPositionData = vp_data;
	MeshData->DirLightData = dl_data;
	MeshData->Indices = index_data;
}

void ChunkMeshDataFree(CHUNK_MESH_DATA* MeshData)
{
	free(MeshData->VertexPositionData);
	MeshData->VertexPositionData = NULL;
	free(MeshData->DirLightData);
	MeshData->DirLightData = NULL;
	free(MeshData->Indices);
	MeshData->Indices = NULL;

	MeshData->VertexDataSize = 0;
	MeshData->IndexSize = 0;
}
