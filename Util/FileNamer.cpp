#include "FileNamer.h"

FileNamer::FileNamer(char* base, char * ext, int start, int inc){
	current = start;
	increment=inc;
	currentName = new char[strlen(base)+strlen(ext)+7];
	previewName= new char[strlen(base)+strlen(ext)+7];
	basename = new char[strlen(base)+1];
	extension=new char[strlen(ext)+1];
	strcpy(basename,base);
	strcpy(extension,ext);
}


FileNamer::FileNamer(char* base, char* ext) {
	current = 0;
	increment=1;
	currentName = new char[strlen(base)+strlen(ext)+7];
	previewName= new char[strlen(base)+strlen(ext)+7];
	basename = new char[strlen(base)+1];
//	printf("Created string length base=%d",strlen(base));
	extension=new char[strlen(ext)+1];
//	printf("Doing the copy localer.\n");
	strcpy(basename,base);
//	printf("Doing the copy localer.\n");
	strcpy(extension,ext);
//	printf("Done..create.\n");
}

FileNamer::FileNamer(char* base, char* ext, int start){
	current = start;
	increment=1;
	currentName = new char[strlen(base)+strlen(ext)+7];
	previewName= new char[strlen(base)+strlen(ext)+7];
	basename = new char[strlen(base)+1];
	extension=new char[strlen(ext)+1];
	strcpy(basename,base);
	strcpy(extension,ext);
}

char* FileNamer::nextFile() {
	sprintf(currentName,"%s%04d.%s",basename,current,extension);
	current++;
// 	printf("%s%05d.%s",basename,current,extension);
	return currentName;
}

char* FileNamer::previewNext(){
	sprintf(previewName,"%s%04d.%s",basename,current,extension);
	return previewName;
}

const char* FileNamer::fileByNumber(int number) {
	sprintf(currentName,"%s%04d.%s",basename,number,extension);
	return currentName;
}

FileNamer::~FileNamer(){
	delete [] currentName;
	delete [] basename;
	delete extension;
}

bool FileNamer::moreExist() {
	FILE *testfile = fopen(previewNext(),"r");
	if (testfile==NULL) {
		return false;
	} else  {
		fclose(testfile);
		return true;
	}


}
