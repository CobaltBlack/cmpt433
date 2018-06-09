// This module provides some file operations for convenience

#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_

#define PATH_MAX 255

// Returns contents of file in outBuff
int readFromFile(const char* filename, char* outBuff);

// Writes content into file
void writeToFile(const char* filename, const char* content);

// Concatenates pathA and pathB and stores it in outBuff
void concatPath(const char* pathA, const char* pathB, char* outBuff);

#endif
