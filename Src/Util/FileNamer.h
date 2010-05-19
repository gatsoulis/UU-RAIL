#ifndef FILENAMER_H_
#define FILENAMER_H_
#include <string.h>
#include <stdio.h>

class FileNamer
{
public:
	char* basename;
	char* extension;
	int current;
	char* currentName;
	int increment;

	FileNamer(char* base, char* extension);
	FileNamer(char* base, char* extension, int start);
	FileNamer(char* base, char* extension, int start, int increment);
	char* nextFile();
	char* previewNext();
	const char* fileByNumber(int number);
	bool moreExist();

	operator char* () {
		return nextFile();
	}

	~FileNamer();

private:
	char* previewName;
};

#endif

