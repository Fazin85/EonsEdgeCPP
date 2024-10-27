#ifndef EON_STD_H
#define EON_STD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "Windows.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define VOID void
#define CONST const
#define INTERNAL static
#define EON_NULL (void*)0
#define EON_TRUE 1
#define EON_FALSE 0

	// data types

	typedef char EON_CHAR;
	typedef uint8_t EON_UCHAR;
	typedef int16_t EON_SHORT;
	typedef uint16_t EON_USHORT;
	typedef int32_t EON_INT;
	typedef uint32_t EON_UINT;
	typedef int64_t EON_LONG;
	typedef uint64_t EON_ULONG;
	typedef size_t EON_SIZE_T;
	typedef float EON_FLOAT;
	typedef double EON_DOUBLE;
	typedef uint8_t EON_BOOL;

	// pointer types

	typedef char* EON_PCHAR;
	typedef uint8_t* EON_PUCHAR;
	typedef int16_t* EON_PSHORT;
	typedef uint16_t* EON_PUSHORT;
	typedef int32_t* EON_PINT;
	typedef uint32_t* EON_PUINT;
	typedef int64_t* EON_PLONG;
	typedef uint64_t* EON_PULONG;
	typedef size_t* EON_PSIZE_T;
	typedef float* EON_PFLOAT;
	typedef double* EON_PDOUBLE;
	typedef uint8_t* EON_PBOOL;
	typedef void* EON_PVOID;
	typedef const char* EON_PCSTR;

	EON_PVOID EonMalloc(EON_SIZE_T SizeBytes);
	EON_PVOID EonCalloc(EON_SIZE_T NumElements, EON_SIZE_T ElementSizeBytes);
	EON_PVOID EonRealloc(EON_PVOID Block, EON_SIZE_T SizeBytes);
	VOID EonFree(EON_PVOID Block);

	VOID EonMemset(EON_PVOID Buffer, EON_INT Value, EON_SIZE_T SizeBytes);

	typedef struct _BINARY_FILE
	{
		FILE* File;
		EON_BOOL Open;
	} BINARY_FILE, * PBINARY_FILE;

	PBINARY_FILE BinaryFileOpen(EON_PCSTR FileName);

	EON_BOOL BinaryFileWrite(PBINARY_FILE BinaryFile,
		EON_PCHAR Data,
		EON_SIZE_T SizeBytes);

	EON_BOOL BinaryFileRead(PBINARY_FILE BinaryFile,
		EON_PVOID Buffer,
		EON_SIZE_T SizeBytes);

	VOID BinaryFileClose(PBINARY_FILE BinaryFile);

#ifdef __cplusplus
}
#endif

#endif // !EON_STD_H
