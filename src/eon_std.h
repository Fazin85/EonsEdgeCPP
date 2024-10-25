#ifndef EON_STD_H
#define EON_STD_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INTERNAL static
#define VOID void
#define CONST const

	typedef unsigned char UCHAR;
	typedef char CHAR;
	typedef short SHORT;
	typedef unsigned short USHORT;
	typedef unsigned int UINT;
	typedef int INT;
	typedef unsigned long ULONG;
	typedef long LONG;
	typedef unsigned long long ULONGLONG;
	typedef long long LONGLONG;
	typedef size_t SIZE_T;

	typedef struct _BINARY_FILE
	{
		FILE* File;
	} BINARY_FILE;

	BINARY_FILE* FileOpenBinary(CONST CHAR* FileName);
	VOID FileCloseBinary(BINARY_FILE* BinaryFile);
	VOID FileWriteBinary(BINARY_FILE* File, CHAR* Data, SIZE_T Size);

#ifdef __cplusplus
}
#endif

#endif
