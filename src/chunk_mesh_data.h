#ifndef CHUNK_MESH_DATA_H
#define CHUNK_MESH_DATA_H

#include "eon_std.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct _CHUNK_MESH_DATA
	{
		UINT* VertexPositionData;
		UINT* DirLightData;
		UINT* Indices;
		SIZE_T VertexDataSize;
		SIZE_T IndexSize;
	} CHUNK_MESH_DATA;

	void GetDataFromGlBufferHandles(CHUNK_MESH_DATA* MeshData, UINT VpBufferId, UINT DlBufferId, UINT IndicesBufferId);

	void ChunkMeshDataFree(CHUNK_MESH_DATA* MeshData);

#ifdef __cplusplus
}
#endif

#endif // CHUNK_MESH_DATA_H
