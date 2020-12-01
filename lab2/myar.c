/*
THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
A TUTOR OR CODE WRITTEN BY OTHER STUDENTS – Yibo WANG
*/

#include <ar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <utime.h>
#include <search.h>

#define NAME_LIMIT 16
#define DATE_LIMIT 12
#define UID_LIMIT 6
#define GID_LIMIT 6
#define MODE_LIMIT 8
#define SIZE_LIMIT 10
#define ARFMAG_LIMIT 2

struct meta {
	char name[16]; //room for null
	int mode;
	int size;
	time_t mtime; // a time_t is a long
};

int my_isarchive(char *afile);
int my_openar(char *file, int flags);
int my_isheader(int ar_desc, int pos);
void format_char(char *string);
struct ar_hdr * my_getheader(int ar_desc, int pos);
void tv_print(int ar_desc, char key);
void t_print(char *afile);
void v_print(char *afile);
char ** get_files(int file_count, char **argv, int k);
void read_write(int w_desc, int r_desc, int size);
void extracto(int ar_desc, struct ar_hdr *header);
void extract(int ar_desc, struct ar_hdr *header);
int compare(const void *file1, const void *file2);
int create_copy(char *afile, int flags, int permissions);
void append_file_copy(char *afile, char *file);
void append_copy(char *afile, char **files, int file_count, char *self);
void delete(char *afile, char **files, int file_count);
void x_xo_extract(char *afile, char **files, int file_count,
		   void (extract_extracto) (int, struct ar_hdr *));
int open_file(char *file);
void init_ar(char *afile, int flags);
int create_ar(char *afile, int flags, int permissions);
off_t get_size(int file_desc);
int my_isfile_copy(char **files, int file_count);
int fill_ar_hdr(char *afile, struct ar_hdr *header);
void append_file(char *afile, char *file);
void append(char *afile, char **files, int file_count, char *self);


int fill_meta( struct ar_hdr *header, struct meta *meta){
	mode_t file_mode = strtoul(header->ar_mode, NULL, 8);
	time_t sec = (time_t) strtol(header->ar_date, NULL, 0);
	int size = atoi(header->ar_size);
	strcpy(meta->name, header->ar_name);
	meta->mode = file_mode;
	meta->size = size;
	meta->mtime = sec;
}


int my_isfile_copy(char **files, int file_count)
{
	struct stat buffer;
	int i;
	for(i = 0; i < file_count; i++) {
		if(access(files[i], F_OK) == -1) {
			return 0;
		} 
		else {
			stat(files[i], &buffer);
			if(!S_ISREG(buffer.st_mode)) {
				printf("ar: %s is not a regular file\n", files[i]);
				return 0;
			}
		}
	}
	return 1;
}


int my_isarchive_copy(char *afile)
{
	if(!my_isfile_copy(&afile, 1)) {
		return 0;
	}
	else {
		int file_desc = open(afile, O_RDONLY);
		if(file_desc != -1) {
			char buffer[SARMAG + 1];
			read(file_desc, buffer, SARMAG);
			buffer[SARMAG] = '\0';
			if(strcmp(buffer, ARMAG) != 0) {
				printf("ar: invalid format\n");
				exit(1);
			}
		}
		else {
			printf("ar: cannot open archive\n");
			exit(1);
		}
	}
	return 1;
}


int my_isarchive(char *afile)
{
	struct stat buffer;
	if(access(afile, F_OK) == -1) {
		printf("ar: %s: No such file or directory\n", afile);
		return 0;
	} 
	else {
		stat(afile, &buffer);
		if(!S_ISREG(buffer.st_mode)) {
			printf("ar: %s is not a regular file\n", afile);
			return 0;
		}
	}

	int file_desc = open(afile, O_RDONLY);
	if(file_desc != -1) {
		char buffer[SARMAG + 1];
		read(file_desc, buffer, SARMAG);
		buffer[SARMAG] = '\0';
		if(strcmp(buffer, ARMAG) != 0) {
			printf("ar: invalid format\n");
			exit(1);
		}
	}
	else {
		printf("ar: cannot open archive\n");
		exit(1);
	}
	return 1;
}


int my_isheader(int ar_desc, int pos)
{
	int orig_pos = lseek(ar_desc, 0, SEEK_CUR);
	int end_file = lseek(ar_desc, 0, SEEK_END);
	lseek(ar_desc, orig_pos, SEEK_SET);
	return !(orig_pos + pos >= (end_file - 1));
}


int my_openar(char *file, int flags)
{
	int ar_desc = open(file, flags);
	if(ar_desc == -1) {
		printf("ar: cannot read archive: %s\n", file);
		exit(1);
	}
	return ar_desc;
}


void format_char(char *string)
{
	int i;
	for(i = 0; string[i] == ' '; i++)
		string++;
	for(i = strlen(string) - 1; string[i] == ' '; i--)
		string[i] = '\0';
}


struct ar_hdr * my_getheader(int ar_desc, int pos)
{
	lseek(ar_desc, pos, SEEK_CUR);
	struct ar_hdr *header = (struct ar_hdr *) malloc(sizeof(struct ar_hdr));
	read(ar_desc, header, sizeof(struct ar_hdr));

	int i;
	for (i = 0; i < NAME_LIMIT; i++){
		if (header->ar_name[i] == '/'){
			break;
		}
	}
	header->ar_name[i] = '\0';
	header->ar_date[DATE_LIMIT - 1] = '\0';
	header->ar_uid[UID_LIMIT - 1] = '\0';
	header->ar_gid[GID_LIMIT - 1] = '\0';
	header->ar_mode[MODE_LIMIT - 1] = '\0';
	header->ar_size[SIZE_LIMIT - 1] = '\0';
	format_char(header->ar_date);
	format_char(header->ar_uid);
	format_char(header->ar_gid);
	format_char(header->ar_mode);

	return header;
}


void tv_print(int ar_desc, char key)
{
	lseek(ar_desc, SARMAG, SEEK_SET);
	int pos = 0;
	struct ar_hdr *header;
	struct meta *meta = (struct meta *) malloc(sizeof(struct meta));

	while(my_isheader(ar_desc, pos)) {
		header = my_getheader(ar_desc, pos);
		fill_meta(header, meta);

		if(strcmp(header->ar_name, "")) {
			if(key == 't')
				printf("%s\n", header->ar_name);
			if(key == 'v') {
				fill_meta(header, meta);
			int chars = sizeof("rwxrwxrwx");
			char *permissions = (char *) malloc(sizeof(char) * chars);
			snprintf(permissions, chars, "%c%c%c%c%c%c%c%c%c",
				 (meta->mode & S_IRUSR) ? 'r' : '-',
				 (meta->mode & S_IWUSR) ? 'w' : '-',
				 (meta->mode & S_IXUSR) ? 'x' : '-',
				 (meta->mode & S_IRGRP) ? 'r' : '-',
				 (meta->mode & S_IWGRP) ? 'w' : '-',
				 (meta->mode & S_IXGRP) ? 'x' : '-',
				 (meta->mode & S_IROTH) ? 'r' : '-',
				 (meta->mode & S_IWOTH) ? 'w' : '-',
				 (meta->mode & S_IXOTH) ? 'x' : '-');


				char buffer[1000];
				time_t sec = meta->mtime;
				struct tm *time = localtime(&sec);
				strftime(buffer, 1000, "%b %d %H:%M %Y", time);
				format_char(buffer);
				printf("%s %s/%s\t     ",
				       permissions, header->ar_uid, header->ar_gid);
				int size = meta->size;
				int i;
				int j = 1;
				for(i = 10; i <= size; i *= 10) {
					printf("\b");
					j++;
					if(j > 6) {
						printf(" ");
					}
				}
				printf("%s", header->ar_size);
				for(i = 8 - j; i > 0; i--)
					printf("\b");
				if(buffer[4] == '0')
					buffer[4] = ' ';
				printf("%s %s\n", buffer, meta->name);
			}
		}
		pos = atoi(header->ar_size);
		pos += pos % 2;
	}
}


void t_print(char *afile)
{
	int ar_desc = my_openar(afile, O_RDONLY);
	tv_print(ar_desc, 't');
	if(close(ar_desc == -1)) {
		printf("ar: Cannot close archive");
		exit(1);
	}
	exit(0);
}


void v_print(char *afile)
{
	int ar_desc = my_openar(afile, O_RDONLY);
	tv_print(ar_desc, 'v');
	if(close(ar_desc == -1)) {
		printf("ar: Cannot close archive");
		exit(1);
	}
	exit(0);
}

void A_append(char *afile, char *self, int N)
{
	DIR *cwd = opendir(".");
	int file_count = 0;
	struct stat buffer;
	struct dirent *dir_entry = readdir(cwd);
	while(dir_entry != NULL) {
		stat(dir_entry->d_name, &buffer);
		time_t sec = (time_t) buffer.st_mtime;
		time_t now = time(NULL);
		if(  S_ISREG(buffer.st_mode) &&
			((now-sec)/86400 >= N )
			)
		{
			file_count++;
		}
		dir_entry = readdir(cwd);
	}
	char **files = (char **) malloc(sizeof(char *) * file_count);
	cwd = opendir(".");
	dir_entry = readdir(cwd);
	int i = 0;
	while(dir_entry != NULL) {
		stat(dir_entry->d_name, &buffer);
		time_t sec = (time_t) buffer.st_mtime;
		time_t now = time(NULL);
		if(S_ISREG(buffer.st_mode) &&
			((now-sec)/86400 >= N )
			){
			files[i] = dir_entry->d_name;
			i++;
		}
		dir_entry = readdir(cwd);
	}
	if(file_count == 0)
		exit(1);
	append_copy(afile, files, file_count, self);
}


char ** get_files(int file_count, char **argv, int k)
{
	char **files = (char **) malloc(sizeof(char *) * file_count);
	int i;
	for(i = 0; i < file_count; i++){
		files[i] = argv[i + k];
	}
	return files;
}


void read_write(int w_desc, int r_desc, int size)
{
	char buffer[1];
	off_t total = 0;
	int wrote = 0;
	while(total < size) {
		if(read(r_desc, buffer, 1) < 0){
			printf("ar: read error\n");
			exit(1);
		}
		if((wrote = write(w_desc, buffer, 1)) < 0){
			printf("ar: write error\n");
			exit(1);
		}
		total += wrote;
		lseek(r_desc, 0, SEEK_CUR);
	}
	if(size % 2 == 1) {
		buffer[0] = '\n';
		write(w_desc, buffer, 1);
	}
}


void extracto(int ar_desc, struct ar_hdr *header)
{
	int orig_pos = lseek(ar_desc, 0L, SEEK_CUR);
	int flags = O_CREAT | O_WRONLY;
	struct meta *meta = (struct meta *) malloc(sizeof(struct meta));
	fill_meta(header, meta);

	int file_desc = open(header->ar_name, flags, meta->mode);
	read_write(file_desc, ar_desc, meta->size);
	struct stat buf;
	struct utimbuf o_buf;
	if(stat(header->ar_name, &buf) == -1){
		printf("ar: conversion error\n");
		exit(1);
	}
	o_buf.actime = o_buf.modtime = meta->mtime;
	if(utime(header->ar_name, &o_buf) == -1){
		printf("ar: conversion error\n");
		exit(1);
	}
	long uid = strtol(header->ar_uid, NULL, 0);
	long gid = strtol(header->ar_gid, NULL, 0);
	chown(header->ar_name, uid, gid);
	chmod(header->ar_name, meta->mode);
	close(file_desc);
	lseek(ar_desc, orig_pos, SEEK_SET);
}


void extract(int ar_desc, struct ar_hdr *header)
{
	int orig_pos = lseek(ar_desc, 0L, SEEK_CUR);
	int flags = O_CREAT | O_WRONLY;
	struct meta *meta = (struct meta *) malloc(sizeof(struct meta));
	fill_meta(header, meta);

	int file_desc = open(header->ar_name, flags, meta->mode);
	read_write(file_desc, ar_desc, meta->size);
	struct stat buf;
	if(stat(header->ar_name, &buf) == -1){
		printf("ar: conversion error\n");
		exit(1);
	}
	long uid = strtol(header->ar_uid, NULL, 0);
	long gid = strtol(header->ar_gid, NULL, 0);
	chown(header->ar_name, uid, gid);
	chmod(header->ar_name, meta->mode);
	close(file_desc);
	lseek(ar_desc, orig_pos, SEEK_SET);
}


int compare(const void *file1, const void *file2)
{
	char *a = (char *) file1;
	char *b = (char *) file2;
	return (!strcmp(a, b));
}


int create_copy(char *afile, int flags, int permissions)
{
	int ar_desc = open(afile, flags, permissions);
	if(ar_desc == -1) {
		printf("ar: cannot create archive: %s\n", afile);
		exit(1);
	}
	if(close(ar_desc == -1)) {
		printf("ar: cannot close archive");
		exit(1);
	}
	return ar_desc;
}


void write_header(int ar_desc, struct ar_hdr *header)
{
	write(ar_desc, header->ar_name, NAME_LIMIT);
	write(ar_desc, header->ar_date, DATE_LIMIT);
	write(ar_desc, header->ar_uid, UID_LIMIT);
	write(ar_desc, header->ar_gid, GID_LIMIT);
	write(ar_desc, header->ar_mode, MODE_LIMIT);
	write(ar_desc, header->ar_size, SIZE_LIMIT);
	write(ar_desc, header->ar_fmag, ARFMAG_LIMIT);
}



void append_file_copy(char *afile, char *file)
{
	int ar_desc;
	int flags = O_RDONLY | O_WRONLY | O_APPEND;
	if(!my_isarchive_copy(afile)) {
		if(access(afile, F_OK) != -1) {
		    printf("ar: invalid archive\n");
		    exit(1);
		} else {
			flags = O_CREAT | O_RDONLY | O_WRONLY | O_APPEND;
			ar_desc = create_copy(afile, flags, 0666);
			flags = O_RDONLY | O_WRONLY | O_APPEND;
			init_ar(afile, flags);
		}
	}
	ar_desc = my_openar(afile, flags);

	int file_desc = open_file(file);
	struct ar_hdr *header = (struct ar_hdr *) malloc(sizeof(struct ar_hdr));
	fill_ar_hdr(file, header);
	write_header(ar_desc, header);
	read_write(ar_desc, file_desc, get_size(file_desc));
	if(close(file_desc == -1)) {
		printf("ar: Cannot close file");
		exit(1);
	}
}


void append_copy(char *afile, char **files, int file_count, char *self)
{
	int i;
	for(i = 0; i < file_count; i++) {
		append_file_copy(afile, files[i]);
	}
	exit(0);
}


void delete(char *afile, char **files, int file_count)
{
	int i;
	struct ar_hdr *header;
	int ar_desc = my_openar(afile, O_RDONLY);
	lseek(ar_desc, SARMAG, SEEK_SET);
	int pos = 0;

	int flags = O_CREAT | O_RDONLY | O_WRONLY | O_APPEND;
	int ar_copy = create_copy(afile, flags, 0666);

	unlink(afile);

	while(my_isheader(ar_desc, pos)) {
		header = my_getheader(ar_desc, pos);
		int flag = 0;
		for(i = 0; i < file_count; i++){
			if(compare(files[i], header->ar_name)) {
				flag = 1;
				files[i] = "";
				break;
			}
		}
		if (flag == 0){
			append_file_copy(afile, header->ar_name);
		}

		pos = atoi(header->ar_size);
		pos += pos % 2;
	}
	exit(0);
}


void x_xo_extract(char *afile, char **files, int file_count, void (extract_extracto) (int, struct ar_hdr *))
{
	int i;
	struct ar_hdr *header;
	int ar_desc = my_openar(afile, O_RDONLY);
	lseek(ar_desc, SARMAG, SEEK_SET);
	int pos = 0;
	while(my_isheader(ar_desc, pos)) {
		header = my_getheader(ar_desc, pos);
		for(i = 0; i < file_count; i++){
			if(compare(files[i], header->ar_name)) {
				extract_extracto(ar_desc, header);
				files[i] = "";
			}
		}
		pos = atoi(header->ar_size);
		pos += pos % 2;
	}
	if(close(ar_desc == -1)) {
		printf("ar: cannot close archive");
		exit(1);
	}
	exit(0);
}


int open_file(char *file)
{
	int file_desc = open(file, O_RDONLY);
	if(file_desc == -1) {
		printf("ar: cannot read file: %s\n", file);
		exit(1);
	}
	return file_desc;
}


void init_ar(char *afile, int flags)
{
	int ar_desc = my_openar(afile, flags);
	if(lseek(ar_desc, 0, SEEK_SET) == lseek(ar_desc, 0, SEEK_END)) {
		write(ar_desc, ARMAG, SARMAG);
	} else {
		printf("ar: Unable to initialize archive\n");
		exit(1);
	}
}


int create_ar(char *afile, int flags, int permissions)
{
	int ar_desc = open(afile, flags, permissions);
	if(ar_desc == -1) {
		printf("Exception: Cannot create archive: %s\n", afile);
		exit(1);
	}
	printf("myar: creating %s\n", afile);
	if(close(ar_desc == -1)) {
		printf("ar: Cannot close archive");
		exit(1);
	}
	return ar_desc;
}


off_t get_size(int file_desc)
{
	off_t size;
	int pos = lseek(file_desc, 0L, SEEK_CUR);
	size = lseek(file_desc, 0, SEEK_END);
	lseek(file_desc, pos, SEEK_SET);
	return size;
}


int fill_ar_hdr(char *file, struct ar_hdr *header)
{
	struct stat buffer;
	int file_desc = open_file(file);
	stat(file, &buffer);
	char header_name[16];
	strcpy(header_name, file);
	strcat(header_name, "/");
	snprintf(header->ar_name, 60, "%-16s%-12ld%-6ld%-6ld%-8lo%-10lld",
		 header_name, buffer.st_mtime, (long) buffer.st_uid,
		 (long) buffer.st_gid, (unsigned long) buffer.st_mode,
		 (long long) buffer.st_size);
	strcpy(header->ar_fmag, ARFMAG);
	lseek(file_desc, 0, SEEK_SET);

}


void append_file(char *afile, char *file)
{
	int ar_desc;
	int flags = O_RDONLY | O_WRONLY | O_APPEND;
	if(!my_isarchive(afile)) {
		if(access(afile, F_OK) != -1) {
		    printf("ar: invalid archive \n");
		    exit(1);
		} else {
			flags = O_CREAT | O_RDONLY | O_WRONLY | O_APPEND;
			ar_desc = create_ar(afile, flags, 0666);
			flags = O_RDONLY | O_WRONLY | O_APPEND;
			init_ar(afile, flags);
		}
	}
	ar_desc = my_openar(afile, flags);

	int file_desc = open_file(file);
	struct ar_hdr *header = (struct ar_hdr *) malloc(sizeof(struct ar_hdr));
	fill_ar_hdr(file, header);
	write_header(ar_desc, header);
	read_write(ar_desc, file_desc, get_size(file_desc));
	if(close(file_desc == -1)) {
		printf("ar: cannot close file");
		exit(1);
	}
}


void append(char *afile, char **files, int file_count, char *self)
{
	int i;
	for(i = 0; i < file_count; i++) {
		append_file(afile, files[i]);
	}
	exit(0);
}


int main(int argc, char **argv)
{
	char *key = argv[1];
	char *afile;
	int N;

	if(argc <= 2){
		printf("Usage: myar [qxotvd:A:] archive-file [file1 .....]\n");
		return 1;
	}

	int kk = 1;
	if(key[0] != '-'){
		kk = 0;
	}

	if(strchr("qxtdAvo", key[kk]) == NULL){
		printf("ar: no operation specified\n");
		return 1;
	}

	if(strchr("A", key[kk]) != NULL){
		if (argc <= 3){
			printf("ar: args loss\n");
			exit(1);
		}
		afile = argv[2];
		N = atoi(argv[3]);
		A_append(afile, argv[0], N);
	}
	else if(strstr(key, "t") != NULL | strstr(key, "v")!=NULL){
		if (argc == 3){
			afile = argv[2];
			if(!my_isarchive(afile)){
				return 1;
			}
			if(strstr(key, "v")!= NULL && strstr(key, "t")!=NULL){
				v_print(afile);
			}
			else if (strstr(key, "t")!=NULL){
				t_print(afile);
			}
			else if (strchr("qxdAo", key[kk]) != NULL){
				printf("ar: two different operation options specified\n");
				return 1;
			}
		}
		else if (argc == 4){
			afile = argv[3];
			if(!my_isarchive(afile)){
				return 1;
			}
			if( (strstr(key, "v")!= NULL && strstr(argv[2], "t")!=NULL) | (strstr(key, "t")!= NULL && strstr(argv[2], "v")!=NULL)){
				v_print(afile);
			}
			else if ((strstr(argv[2], "."))!=NULL){
				printf("no entry %s in archive\n", argv[3]);
				return 1;
			}
			else{
				printf("ar: two different operation options specified\n");
				return 1;
			}
		}
		else{
			printf("no entry %s in archive\n", argv[4]);
			return 1;
		}
	}
	else 
	{
		int file_count;
		char **files;

		if(key[kk] == 'q') {
			afile = argv[2];
			file_count = argc-3;
			files = get_files(file_count, argv, 3);
			append(afile, files, file_count, argv[0]);
		}

		else if(key[kk] == 'd'){
			afile = argv[2];
			file_count = argc-3;
			files = get_files(file_count, argv, 3);
			delete(afile, files, file_count);
		}

		else if (my_isarchive_copy(argv[2])){
			// printf("2%s\n", argv[2]);
			afile = argv[2];
			file_count = argc-3;
			files = get_files(file_count, argv, 3);
			if(strstr(key, "x")!=NULL | strstr(key, "o")!=NULL ){
				if (strstr(key, "o")!=NULL && strstr(key, "x")!=NULL){
					// printf("xo\n");
	  				x_xo_extract(afile, files, file_count, &extracto);
				}
				else if (strstr(key, "x")!=NULL){
					// printf("x\n");
					x_xo_extract(afile, files, file_count, &extract);	
				}
				else if (strchr("qtdAv", key[kk]) != NULL){
					printf("ar: two different operation options specified\n");
					return 1;
				}
			}
		}
		else if (my_isarchive_copy(argv[3])){
			// printf("3%s\n", argv[3]);
			afile = argv[3];
			file_count = argc-4;
			files = get_files(file_count, argv, 4);
			if ((strstr(key, "x")!= NULL && strstr(argv[2], "o")!=NULL) | (strstr(key, "o")!= NULL && strstr(argv[2], "x")!=NULL)){
				// printf("-x-o\n");
				x_xo_extract(afile, files, file_count, &extracto);
			}
			else{
				printf("ar: two different operation options specified\n");
				return 1;
			}
		}
		else{
			printf("ar: two different operation options specified\n");
			return 1;
		}

	}
	return 0;
}