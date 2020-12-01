/*

Honor Pledge:

This source code was developed independently on my own and not copied from open-source libraries or other electronic resources.
My submission reflects my understanding of the assignment and subject material and is my original work. 
I understand that I am responsible for course rules as well as Emory’s Laney Graduate School Honor Code.

09/02/2020 Yibo WANG
*/

#include "pbm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <malloc.h>
#include <math.h>


PPMImage * new_ppmimage( unsigned int width, unsigned int height, unsigned int max ){
	PPMImage * ppm = (PPMImage *) malloc(sizeof(PPMImage));
	if (ppm == NULL){
		exit(1);
	}
	ppm->width = width;
	ppm->height = height;
	ppm->max = max;

	int i, j;
	for (i=0; i < 3; i++){
		ppm->pixmap[i] = (unsigned int **) malloc(sizeof(unsigned int *) * height);
		if (ppm->pixmap[i] == NULL){
			exit(1);
		}

		for (j=0; j<height; j++){
			ppm->pixmap[i][j] = (unsigned int *) malloc(sizeof(unsigned int *) * width);
			if (ppm->pixmap[i][j] == NULL){
				exit(1);
			}
		}
	}
	return ppm;
}

PGMImage * new_pgmimage( unsigned int width, unsigned int height, unsigned int max ){
	PGMImage * pgm = (PGMImage *) malloc(sizeof(PGMImage));
	if (pgm == NULL){
		exit(1);
	}

	pgm->width = width;
	pgm->height = height;
	pgm->max = max;

	pgm->pixmap = (unsigned int **) malloc(sizeof(unsigned int *) * height);
	if (pgm->pixmap == NULL){
		exit(1);
	}

	int i;
	for (i=0; i < height; i++){
		pgm->pixmap[i] = (unsigned int *) malloc(sizeof(unsigned int *) * width);
		if (pgm->pixmap[i] == NULL){
			exit(1);
		}
	}
	return pgm;
}

PBMImage * new_pbmimage( unsigned int width, unsigned int height ){
	PBMImage * pbm =  (PBMImage *) malloc(sizeof(PBMImage));
	if (pbm == NULL){
		exit(1);
	}
	pbm->width = width;
	pbm->height = height;

	pbm->pixmap = (unsigned int **) malloc(sizeof(unsigned int *) * height);
	if (pbm->pixmap == NULL){
		exit(1);
	}

	int i;
	for (i=0; i < height; i++){
		pbm->pixmap[i] = (unsigned int *) malloc(sizeof(unsigned int *) * width);
		if (pbm->pixmap[i] == NULL){
			exit(1);
		}
	}
	return pbm;	
}

void del_ppmimage( PPMImage * ppm ){
	int i, j, height;
	height = ppm->height;

	for (i=0; i<3; i++){
		for (j=0; j<height; j++){
			free(ppm->pixmap[i][j]);
		}
		free(ppm->pixmap[i]);
	}
	free(ppm->pixmap);
	// free(ppm);
}

void del_pgmimage( PGMImage * pgm ){
	int i, height;
	height = pgm->height;
	for (i=0; i< height; i++){
		free(pgm->pixmap[i]);
	}
	free(pgm->pixmap);
	free(pgm);
}

void del_pbmimage( PBMImage * pbm ){
	int i, width, height;
	width = pbm->width;
	height = pbm->height;
	for (i=0; i< height; i++){
		free(pbm->pixmap[i]);
	}
	free(pbm->pixmap);
	free(pbm);
}

// -b
void ppm2pbm( const char * infilename, const char * outfilename ){
	FILE * infile;
	FILE * outfile;
	if ((infile = fopen( infilename, "r" )) == NULL){
		printf("Error: No input file specified\n");
		exit(1);
	}
	fclose( infile );
	if ((outfile = fopen( outfilename, "w" )) == NULL){
		printf("Error: No output file specified\n");
		exit(1);
	}
	fclose( outfile );
	PPMImage * ppm = read_ppmfile( infilename);
	int i, j, PPMMAX, width, height;
	PPMMAX = ppm->max;
	width = ppm->width;
	height = ppm->height;
	PBMImage * pbm = new_pbmimage( width, height );
	for (i=0; i<height; i++){
		for (j=0; j<width; j++){
			if ( (( (float) ppm->pixmap[0][i][j]+(float)ppm->pixmap[1][i][j]+(float)ppm->pixmap[2][i][j])/3.0) < ((float)PPMMAX/2.0) ){
				pbm->pixmap[i][j] = 1;
			}
			else{
				pbm->pixmap[i][j] = 0;
			}
		}
	}
	write_pbmfile( pbm, outfilename);
	del_ppmimage( ppm );
	del_pbmimage( pbm );
	// return pbm;
}

// -g
void ppm2pgm( const char * infilename, const char * outfilename, unsigned int PGMMAX ){
	FILE * infile;
	FILE * outfile;
	if ((infile = fopen( infilename, "r" )) == NULL){
		printf("Error: No input file specified\n");
		exit(1);
	}
	fclose( infile );
	if ((outfile = fopen( outfilename, "w" )) == NULL){
		printf("Error: No output file specified\n");
		exit(1);
	}
	fclose( outfile );
	if (PGMMAX >= 65536){
		printf("Error: Invalid max grayscale pixel value: %u; must be less than 65,536\n", PGMMAX);
		exit(1);
	}

	// printf("here!ppm2pgm\n");
	PPMImage * ppm = read_ppmfile( infilename );
	int i, j;
	int PPMMAX = ppm->max;
	int width = ppm->width;
	int height = ppm->height;

	PGMImage * pgm = new_pgmimage( width, height, PGMMAX );
	for (i=0; i<height; i++){
		for (j=0; j<width; j++){
			double average = (ppm->pixmap[0][i][j]+ppm->pixmap[1][i][j]+ppm->pixmap[2][i][j])/3.0;
			// float tmp = average / PPMMAX * PGMMAX;
			pgm->pixmap[i][j] = average / PPMMAX * PGMMAX;
		}
	}
	write_pgmfile( pgm, outfilename);
	del_ppmimage( ppm );
	del_pgmimage( pgm );
	// return pgm;
}


// -i
void ppm2isolate(const char * infilename, const char * outfilename, const char * color){
	FILE * infile;
	FILE * outfile;
	if ((infile = fopen( infilename, "r" )) == NULL){
		printf("Error: No input file specified\n");
		exit(1);
	}
	fclose( infile );
	if ((outfile = fopen( outfilename, "w" )) == NULL){
		printf("Error: No output file specified\n");
		exit(1);
	}
	fclose( outfile );
	if (strcmp(color, "red")!=0 && strcmp(color, "green") != 0 && strcmp(color, "blue") != 0){
		printf("Error: Invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", color);
		exit(1);
	}
	PPMImage * ppm = read_ppmfile( infilename );

	int i, j, max, width, height;
	max = ppm->max;
	width = ppm->width;
	height = ppm->height;
	PPMImage * new_ppm = new_ppmimage( width, height, max );
	if (strcmp(color, "red") == 0){
		for (i=0; i<height; i++){
			for (j=0; j<width; j++){
				new_ppm->pixmap[0][i][j] = ppm->pixmap[0][i][j];
				new_ppm->pixmap[1][i][j] = 0;
				new_ppm->pixmap[2][i][j] = 0;
			}
		}
	}
	if (strcmp(color, "green") == 0){
		for (i=0; i<height; i++){
			for (j=0; j<width; j++){
				new_ppm->pixmap[1][i][j] = ppm->pixmap[1][i][j];
				new_ppm->pixmap[0][i][j] = 0;
				new_ppm->pixmap[2][i][j] = 0;
			}
		}
	}	
	if (strcmp(color, "blue") == 0){
		for (i=0; i<height; i++){
			for (j=0; j<width; j++){
				new_ppm->pixmap[2][i][j] = ppm->pixmap[2][i][j];
				new_ppm->pixmap[1][i][j] = 0;
				new_ppm->pixmap[0][i][j] = 0;
			}
		}
	}
	write_ppmfile( new_ppm, outfilename );
	del_ppmimage( ppm );
	del_ppmimage( new_ppm );
}


// -r
void ppm2remove(const char * infilename, const char * outfilename, const char * color){
	FILE * infile;
	FILE * outfile;
	if ((infile = fopen( infilename, "r" )) == NULL){
		printf("Error: No input file specified\n");
		exit(1);
	}
	fclose( infile );
	if ((outfile = fopen( outfilename, "w" )) == NULL){
		printf("Error: No output file specified\n");
		exit(1);
	}
	fclose( outfile );
	if (strcmp(color, "red")!=0 && strcmp(color, "green") != 0 && strcmp(color, "blue") != 0){
		printf("Error: Invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", color);
		exit(1);
	}
	PPMImage * ppm = read_ppmfile( infilename );
	int i, j, max, width, height;
	max = ppm->max;
	width = ppm->width;
	height = ppm->height;
	PPMImage * new_ppm = new_ppmimage( width, height, max );
	if (strcmp(color, "red") == 0){
		for (i=0; i<height; i++){
			for (j=0; j<width; j++){
				new_ppm->pixmap[1][i][j] = ppm->pixmap[1][i][j];
				new_ppm->pixmap[2][i][j] = ppm->pixmap[2][i][j];
				new_ppm->pixmap[0][i][j] = 0;
			}
		}
	}
	if (strcmp(color, "green") == 0){
		for (i=0; i<height; i++){
			for (j=0; j<width; j++){
				new_ppm->pixmap[0][i][j] = ppm->pixmap[0][i][j];
				new_ppm->pixmap[2][i][j] = ppm->pixmap[2][i][j];
				new_ppm->pixmap[1][i][j] = 0;
			}
		}
	}	
	if (strcmp(color, "blue") == 0){
		for (i=0; i<height; i++){
			for (j=0; j<width; j++){
				new_ppm->pixmap[0][i][j] = ppm->pixmap[0][i][j];
				new_ppm->pixmap[1][i][j] = ppm->pixmap[1][i][j];
				new_ppm->pixmap[2][i][j] = 0;
			}
		}
	}
	write_ppmfile( new_ppm, outfilename );
	del_ppmimage( ppm );
	del_ppmimage( new_ppm );
}

// -s
void ppm2Sepia( const char * infilename, const char * outfilename ){
	FILE * infile;
	FILE * outfile;
	if ((infile = fopen( infilename, "r" )) == NULL){
		printf("Error: No input file specified\n");
		exit(1);
	}
	fclose( infile );
	if ((outfile = fopen( outfilename, "w" )) == NULL){
		printf("Error: No output file specified\n");
		exit(1);
	}
	fclose( outfile );
	PPMImage * ppm = read_ppmfile( infilename );
	int i, j, max, width, height;
	max = ppm->max;
	width = ppm->width;
	height = ppm->height;
	PPMImage * new_ppm = new_ppmimage( width, height, max );
	for (i=0; i<height; i++){
		for (j=0; j<width; j++){
			new_ppm->pixmap[0][i][j] = max<(0.393* ppm->pixmap[0][i][j])+ (0.769* ppm->pixmap[1][i][j])+ (0.189* ppm->pixmap[2][i][j])?max:(0.393* ppm->pixmap[0][i][j])+ (0.769* ppm->pixmap[1][i][j])+ (0.189* ppm->pixmap[2][i][j]);
			new_ppm->pixmap[1][i][j] = max<(0.349* ppm->pixmap[0][i][j])+ (0.686* ppm->pixmap[1][i][j])+ (0.168* ppm->pixmap[2][i][j])?max:(0.349*  ppm->pixmap[0][i][j])+ (0.686*  ppm->pixmap[1][i][j])+ (0.168*  ppm->pixmap[2][i][j]);
			new_ppm->pixmap[2][i][j] = max<(0.272* ppm->pixmap[0][i][j])+ (0.534* ppm->pixmap[1][i][j])+ (0.131* ppm->pixmap[2][i][j])?max:(0.272*  ppm->pixmap[0][i][j])+ (0.534*  ppm->pixmap[1][i][j])+ (0.131*  ppm->pixmap[2][i][j]);
		}
	}
	write_ppmfile( new_ppm, outfilename );
	del_ppmimage( ppm );
	del_ppmimage( new_ppm );
}

// -m
void ppm2mirror( const char * infilename, const char * outfilename ){
	FILE * infile;
	FILE * outfile;
	if ((infile = fopen( infilename, "r" )) == NULL){
		printf("Error: No input file specified\n");
		exit(1);
	}
	fclose( infile );
	if ((outfile = fopen( outfilename, "w" )) == NULL){
		printf("Error: No output file specified\n");
		exit(1);
	}
	fclose( outfile );
	PPMImage * ppm = read_ppmfile( infilename );
	int i, j, max, width, height;
	max = ppm->max;
	width = ppm->width;
	height = ppm->height;
	// PPMImage * new_ppm = new_pbmimage( width, height, max );
	for (i=0; i<height; i++){
		for (j=0; j<width/2; j++){
			ppm->pixmap[0][i][width-j-1] = ppm->pixmap[0][i][j];
			ppm->pixmap[1][i][width-j-1] = ppm->pixmap[1][i][j];
			ppm->pixmap[2][i][width-j-1] = ppm->pixmap[2][i][j];
		}
	}
	write_ppmfile( ppm, outfilename );
	del_ppmimage( ppm );
	// del_ppmimage( new_ppm );
}

// -t
void ppm2Thumbnail( const char * infilename, const char * outfilename, unsigned int n ){
	FILE * infile;
	FILE * outfile;
	if ((infile = fopen( infilename, "r" )) == NULL){
		printf("Error: No input file specified\n");
		exit(1);
	}
	fclose( infile );
	if ((outfile = fopen( outfilename, "w" )) == NULL){
		printf("Error: No output file specified\n");
		exit(1);
	}
	fclose( outfile );
	if (n >=9 || n <= 0){
		printf("Error: Invalid scale factor: %d; must be 1-8\n", n);
		exit(1);
	}
	PPMImage * ppm = read_ppmfile( infilename );
	int i, j, max, width, height, new_height, new_width;
	max = ppm->max;
	width = ppm->width;
	height = ppm->height;
	if (width%n == 0){
		new_width = width/n;
	}
	else{
		new_width = width/n + 1;
	}
	if (height%n == 0){
		new_height = height/n;
	}
	else{
		new_height = height/n + 1;
	}
	PPMImage * new_ppm = new_ppmimage( new_width, new_height, max );
	for (i=0; i<height; i+=n){
		for (j=0; j<width; j+=n){
			new_ppm->pixmap[0][i/n][j/n] = ppm->pixmap[0][i][j];
			new_ppm->pixmap[1][i/n][j/n] = ppm->pixmap[1][i][j];
			new_ppm->pixmap[2][i/n][j/n] = ppm->pixmap[2][i][j];
		}
	}
	write_ppmfile( new_ppm, outfilename );
	del_ppmimage( ppm );
	del_ppmimage( new_ppm );
}

// -n
void ppm2nup( const char * infilename, const char * outfilename, unsigned int n ){
	FILE * infile;
	FILE * outfile;
	if ((infile = fopen( infilename, "r" )) == NULL){
		printf("Error: No input file specified\n");
		exit(1);
	}
	fclose( infile );
	if ((outfile = fopen( outfilename, "w" )) == NULL){
		printf("Error: No output file specified\n");
		exit(1);
	}
	fclose( outfile );
	if (n >=9 || n <= 0){
		printf("Error: Invalid scale factor: %d; must be 1-8\n", n);
		exit(1);
	}
	// printf("here is before read\n");
	PPMImage * ppm = read_ppmfile( infilename );
	// printf("here is after read\n");
	int i, j, max, width, height, new_height, new_width;
	max = ppm->max;
	width = ppm->width;
	height = ppm->height;
	if (width%n == 0){
		new_width = width/n;
	}
	else{
		new_width = width/n + 1;
	}
	if (height%n == 0){
		new_height = height/n;
	}
	else{
		new_height = height/n + 1;
	}
	PPMImage * new_ppm1 = new_ppmimage( new_width, new_height, max );
	for (i=0; i<height; i+=n){
		for (j=0; j<width; j+=n){
			new_ppm1->pixmap[0][i/n][j/n] = ppm->pixmap[0][i][j];
			new_ppm1->pixmap[1][i/n][j/n] = ppm->pixmap[1][i][j];
			new_ppm1->pixmap[2][i/n][j/n] = ppm->pixmap[2][i][j];
		}
	}

	PPMImage * new_ppm = new_ppmimage( width, height, max );
	for (i=0; i<height; i++){
		for (j=0; j<width; j++){
			new_ppm->pixmap[0][i][j] = new_ppm1->pixmap[0][i%new_height][j%new_width];
			new_ppm->pixmap[1][i][j] = new_ppm1->pixmap[1][i%new_height][j%new_width];
			new_ppm->pixmap[2][i][j] = new_ppm1->pixmap[2][i%new_height][j%new_width];
		}
	}
	write_ppmfile( new_ppm, outfilename );
	del_ppmimage( ppm );
	del_ppmimage( new_ppm1 );
	del_ppmimage( new_ppm );
}

// -o

int main (int argc, char *argv[]) {
	int opt;
	int flag, flag2;
	char *func=NULL;
	const char *color;
	unsigned int PGMMAX;
	unsigned int n;
	const char * infilename;
	const char * outfilename;
	char *ptr;
	FILE * infile;
	FILE * outfile;

	flag = 0;
	flag2 = 0;
	while ((opt = getopt(argc, argv, "bg:i:r:smt:n:o:")) != -1) {
		if (argc < 4){
			printf("Usage: ppmcvt [-bgirsmtno] [FILE]\n");
		}
		switch (opt) {
		case 'b': 
			if ( flag== 1){
				printf("problem is b\n");
				printf("Error: Multiple transformations specified\n");
				exit(1);
			}
			func = "b";
			flag = 1;
			break;
		case 'g':
			if ( flag == 1){
				printf("problem is g\n");
				printf("Error: Multiple transformations specified\n");
				exit(1);
			}
			PGMMAX = strtol(optarg, &ptr, 10);
			// printf("%d\n", PGMMAX);
			func = "g";
			flag = 1;
			break;
		case 'i':
			if ( flag== 1){
				printf("problem is i\n");
				printf("Error: Multiple transformations specified\n");
				exit(1);
			}
			color = optarg;
			func = "i";
			flag = 1;
			break;
		case 'r':
			if ( flag== 1){
				printf("problem is r\n");
				printf("Error: Multiple transformations specified\n");
				exit(1);
			}
			color = optarg;
			func = "r";
			flag = 1;
			break;
		case 's':
			if ( flag== 1){
				printf("problem is s\n");
				printf("Error: Multiple transformations specified\n");
				exit(1);
			}
			func = "s";
			flag = 1;
			break;
		case 'm':
			if ( flag== 1){
				printf("problem is m\n");
				printf("Error: Multiple transformations specified\n");
				exit(1);
			}
			func = "m";
			flag = 1;
			break;
		case 't':
			if ( flag== 1){
				printf("problem is t\n");
				printf("Error: Multiple transformations specified\n");
				exit(1);
			}
			n = strtol(optarg, &ptr, 10);
			func = "t";
			flag = 1;
			break;
		case 'n':
			if ( flag== 1){
				printf("problem is n\n");
				printf("Error: Multiple transformations specified\n");
				exit(1);
			}
			n = strtol(optarg, &ptr, 10);
			func = "n";
			flag = 1;
			break;
		case 'o':
			if (flag2 == 1){
				printf("problem is o\n");
				printf("Error: Multiple transformations specified\n");
			}
			if (flag == 0){
				// printf("Usage: ppmcvt [-bgirsmtno] [FILE]\n");
				func = "b";
			}
			flag2 = 1;
			outfilename = optarg;
			if ((outfile = fopen( outfilename, "w" )) == NULL){
				printf("Error: No output file specified\n");
				exit(1);
			}
			fclose( outfile );
			if (optind == argc-1){
				infilename = argv[argc-1];
			}
			else{
				printf("Error: No input file specified\n");
				exit(1);
			}
			break;
		}

	}


	// infilename = argv[argc-1];
	// outfilename = argv[argc-2];
	// if ((infile = fopen( infilename, "r" )) == NULL){
	// 	printf("Error: No input file specified\n");
	// 	exit(1);
	// }
	if (flag2 == 0){
		printf("Error: No output file specified\n");
		exit(1);
	}
	if (strcmp( func, "b") == 0){
		// printf("here!\n");
		ppm2pbm( infilename, outfilename);
	}
	// outfilename = argv[argc-2];
	if (strcmp( func, "g") == 0){
		// printf("here main\n");
		ppm2pgm( infilename, outfilename, PGMMAX);
	}
	if (strcmp( func, "i") == 0){
		ppm2isolate( infilename, outfilename, color);
	}
	if (strcmp( func, "r") == 0){
		ppm2remove( infilename, outfilename, color);
	}
	if (strcmp(func, "s")==0){
		ppm2Sepia(infilename, outfilename);
	}
	if ( strcmp( func, "m") == 0){
		ppm2mirror( infilename, outfilename);
	}
	if (strcmp( func, "t") == 0){
		ppm2Thumbnail( infilename, outfilename, n);
	}
	if (strcmp( func, "n") == 0){
		// printf("here is n\n");
		ppm2nup( infilename, outfilename, n);
	}
	exit(0);
}
