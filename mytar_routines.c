#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;


/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
	int TotalBytes = 0;
	int SalidaBytes =0;
	int EntradaBytes = 0;

	if(origin == NULL)return -1;

	//La EntradaBytes se iguala a getc(origin) que
	while(TotalBytes < nBytes && (EntradaBytes = getc(origin)) != EOF){
		if(ferror(origin)!=0)return -1;
		SalidaBytes = putc(EntradaBytes,destination);
		if(SalidaBytes==EOF) return -1;
		TotalBytes++;
	}
	return TotalBytes;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
	// Complete the function
	int tamNombre=0;
	char* nombre;
	char bit;

	while((bit=getc(file))!='\0') tamNombre++;

	nombre = malloc(sizeof(char)*(tamNombre+1));

	fseek(file,-(tamNombre+1),SEEK_CUR);

	for(int i = 0; i < tamNombre + 1; i++){
		nombre[i]=getc(file);

	}
	return nombre;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	int NumFicheros =0, tam=0;
	stHeaderEntry *cabecera=NULL;

	if(fread(&NumFicheros,sizeof(int),1,tarFile)==0){
		return NULL;
	}

	cabecera=malloc(sizeof(stHeaderEntry)*NumFicheros);

	for(int i =0; i <NumFicheros;i++)
	{
		if (loadstr(tarFile) != cabecera[i].name){
			return NULL;
		}
		fread(&tam,sizeof(unsigned int),1,tarFile);

		cabecera[i].size = tam;
	}

	(*nFiles) = NumFicheros;

	return cabecera;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	FILE * inputFile;
	FILE * outputFile;

	int bytesCopiados = 0;
	int bytesCabecera = 0;
	stHeaderEntry *cabecera;

	cabecera = malloc(sizeof(stHeaderEntry)*nFiles);
	bytesCabecera += sizeof(int);
	bytesCabecera += nFiles*sizeof(unsigned int);

	for(int i = 0; i < nFiles; i++){
	bytesCabecera += strlen(fileNames[i])+1;

	}
	outputFile = fopen(tarName,"w");
	fseek(outputFile, bytesCabecera, SEEK_SET);

	for(int i = 0; i < nFiles; i++){
		if((inputFile = fopen(fileNames[i], "r"))== NULL){
			return(EXIT_FAILURE);		}

	bytesCopiados = copynFiles(inputFile,outputFile, INT_MAX);
	if(bytesCopiados == -1){
		return EXIT_FAILURE;
	}
	else{
		cabecera[i].size = bytesCopiados;
		cabecera[i].name = malloc(sizeof(fileNames[i])+1);
		strcpy(cabecera[i].name,fileNames[i]);
	}
	if (fclose(inputFile)== EOF)return EXIT_FAILURE;
	}

	if(fseek(outputFile,0,SEEK_SET)!= 0) return EXIT_FAILURE;
	else fwrite(&nFiles, sizeof(int),1,outputFile);

	for(int i = 0; i < nFiles; i++){
		fwrite(cabecera[i].name , strlen(cabecera [i].name)+1,1,outputFile);
		fwrite(&cabecera[i].size, sizeof(unsigned int),1,outputFile);
	}
	for(int i = 0; i < nFiles; i++){
		free(cabecera[i].name);
	}
	free(cabecera);

	if(fclose(outputFile)==EOF) return EXIT_FAILURE;


	return EXIT_SUCCESS;

}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
	FILE *tarFile = NULL;
	FILE *destinationFile = NULL;
	stHeaderEntry *cabecera;
	int nfiles = 0, bytesCopiados = 0;

	if((tarFile = fopen(tarName,"r"))==NULL)return EXIT_FAILURE;

	if(readHeader(tarFile, &nfiles) == NULL){
		return EXIT_FAILURE;
	}

	for ( int i = 0; i < nfiles; i++){
		if((destinationFile = fopen(cabecera[i].name, "w")) == NULL)
			return EXIT_FAILURE;
		else{
			bytesCopiados = copynFile(tarFile,destinationFile, cabecera[i].size);
			if(bytesCopiados == -1) return EXIT_FAILURE;
		}
		if(fclose(destinationFile)!=0)return EXIT_FAILURE;
	}
	return EXIT_FAILURE;
	for (int i = 0; i < nfiles;i++) free(cabecera[i].name);

	free(cabecera);
	if(fclose(tarFile) == EOF) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
