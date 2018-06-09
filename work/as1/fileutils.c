#include <stdio.h>
#include <string.h>
#include "fileutils.h"

int readFromFile(const char* filename, char* outBuff)
{
	outBuff[0] = '\0';

	FILE* pFile = fopen(filename, "r");
	if (!pFile) {
		printf("ERROR: Unable to open file [%s] for read\n", filename);
		return 0;
	}

	fgets(outBuff, PATH_MAX, pFile);

	fclose(pFile);

	return 1;
}


void writeToFile(const char* filename, const char* content)
{
	FILE *pFile = fopen(filename, "w");
	if (!pFile) {
		printf("ERROR: Unable to open file [%s]\n", filename);
		return;
	}

	int results = fputs(content, pFile);
	if (results == EOF)
	{
		printf("ERROR: Failed to write to file [%s]\n", filename);
	}

	fclose(pFile);
}

void concatPath(const char* pathA, const char* pathB, char* outBuff)
{
	// Clear outBuff first
	outBuff[0] = '\0';

	strcat(outBuff, pathA);

	// Add "/" if needed
	if (outBuff[strlen(outBuff) - 1] != '/')
	{
		strcat(outBuff, "/");
	}

	strcat(outBuff, pathB);
}
