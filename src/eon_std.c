#include "eon_std.h"

BINARY_FILE* FileOpenBinary(CONST CHAR* FileName)
{
	BINARY_FILE* BinaryFile = calloc(1, sizeof(BINARY_FILE));

	if (BinaryFile == NULL)
	{
		return NULL;
	}

	BinaryFile->File = fopen(FileName, "wb");

	if (BinaryFile->File == NULL)
	{
		return NULL;
	}

	return BinaryFile;
}

VOID FileCloseBinary(BINARY_FILE* BinaryFile)
{
	fclose(BinaryFile->File);
}

VOID FileWriteBinary(BINARY_FILE* BinaryFile, CHAR* Data, SIZE_T Size)
{
	fwrite(Data, sizeof(CHAR), Size, BinaryFile->File);
}
