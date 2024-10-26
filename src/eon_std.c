#include "eon_std.h"

EON_PVOID EonMalloc(EON_SIZE_T SizeBytes)
{
	EON_PVOID Memory = malloc(SizeBytes);

	if (Memory == NULL)
	{
		abort();
	}

	return Memory;
}

EON_PVOID EonCalloc(EON_SIZE_T NumElements, EON_SIZE_T ElementSizeBytes)
{
	EON_PVOID Memory = calloc(NumElements, ElementSizeBytes);

	if (Memory == EON_NULL)
	{
		abort();
	}

	return Memory;
}

VOID EonFree(EON_PVOID Block)
{
	free(Block);
}

VOID EonMemset(EON_PVOID Buffer, EON_INT Value, EON_SIZE_T SizeBytes)
{
	EON_PVOID Result = memset(Buffer, Value, SizeBytes);

	if (Result != Buffer)
	{
		abort();
	}
}

PBINARY_FILE BinaryFileOpen(EON_PCSTR FileName)
{
	PBINARY_FILE BinaryFile = EonCalloc(1, sizeof(BINARY_FILE));

	BinaryFile->File = fopen(FileName, "wb");

	if (BinaryFile->File == EON_NULL)
	{
		return EON_NULL;
	}

	return BinaryFile;
}

EON_BOOL BinaryFileWrite(PBINARY_FILE BinaryFile, EON_PCHAR Data, EON_SIZE_T SizeBytes)
{
	EON_SIZE_T BytesWritten = fwrite(Data, 1, SizeBytes, BinaryFile->File);

	if (BytesWritten != SizeBytes)
	{
		return EON_FALSE;
	}

	return EON_TRUE;
}

EON_BOOL BinaryFileRead(PBINARY_FILE BinaryFile, EON_PVOID Buffer, EON_SIZE_T SizeBytes)
{
	EON_SIZE_T BytesRead = fread(Buffer, 1, SizeBytes, BinaryFile->File);

	if (BytesRead != SizeBytes)
	{

		return EON_FALSE;
	}

	return EON_TRUE;
}

VOID BinaryFileClose(PBINARY_FILE BinaryFile)
{
	EON_INT Result = fclose(BinaryFile->File);

	if (Result == EOF)
	{
		abort();
	}

	BinaryFile->Open = EON_FALSE;
}
