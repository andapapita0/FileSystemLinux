#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>

#define MAX_PATH_LEN 256
#define MAX 1024

#define __DEBUG

#ifdef __DEBUG
void debug_info (const char *file, const char *function, const int line)
{
        fprintf(stderr, "DEBUG. ERROR PLACE: File=\"%s\", Function=\"%s\", Line=\"%d\"\n", file, function, line);
}

#define ERR_MSG(DBG_MSG) { \
        perror(DBG_MSG); \
        debug_info(__FILE__, __FUNCTION__, __LINE__); \
}

#else                   // with no __DEBUG just displays the error message

#define ERR_MSG(DBG_MSG) { \
        perror(DBG_MSG); \
}

#endif

void listDir(char *dirName)
{
	DIR* dir;
	struct dirent *dirEntry;
	char name[MAX_PATH_LEN];

	dir = opendir(dirName);
	if (dir == 0) {
		ERR_MSG ("Error opening directory");
		exit(4);
	}
    // iterate the directory contents
	while ((dirEntry=readdir(dir)) != 0) {
		if( !strcmp(dirEntry->d_name,".") || !strcmp(dirEntry->d_name,"..")){
		}
		else{
            snprintf(name, MAX_PATH_LEN+2, "%s/%s", dirName, dirEntry->d_name);
            printf("%s\n", name);
		}
	}
	closedir(dir);
}

void list_rec(char *dirName){
    char path[MAX_PATH_LEN];
    struct dirent *dp;
    DIR *dir = opendir(dirName);
    // Unable to open directory stream
    if (!dir)
        return;
    while ((dp = readdir(dir)) != NULL){
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
            snprintf(path, sizeof(path)+1, "%s/%s", dirName, dp->d_name);
            printf("%s\n", path);
            // Construct new path from our base path
            strcpy(path, dirName);
            strcat(path, "/");
            strcat(path, dp->d_name);
            list_rec(path);
        }
    }
    closedir(dir);
}

void list_bysize(char *basePath, long int val, int rec)
{
    char path[1000];
    struct dirent *dp;
    struct stat inode;
    DIR *dir = opendir(basePath);
    if (!dir)
        return;
    while ((dp = readdir(dir)) != NULL){
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
            snprintf(path, sizeof(path)+1, "%s/%s", basePath, dp->d_name);
            lstat(path, &inode);
            if((S_ISREG(inode.st_mode)) && (inode.st_size < val)){
                printf("%s\n", path);
            }
            else;
            if(rec == 0)
                list_bysize(path, val, rec);
            else;
        }
    }
    closedir(dir);
}

void list_byperm(char *basePath, int rec)
{
    char path[MAX_PATH_LEN];
    struct dirent *dp;
    struct stat inode;
    DIR *dir = opendir(basePath);
    if (!dir)
        return;
    while ((dp = readdir(dir)) != NULL){
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
            snprintf(path, sizeof(path)+1, "%s/%s", basePath, dp->d_name);
            lstat(path, &inode);
            if (inode.st_mode & S_IWUSR) {
                printf("%s\n", path);
            }
            else;
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            if(rec == 0)
                list_byperm(path, rec);
            else;
        }
    }
    closedir(dir);
}

int find_str(char const *str, char const *substr){
    char *pos = strstr(str, substr);
    if(pos) return 1;
    else return 0;
}

int parse_err(char *basePath){
   // struct stat inode;
    int fd;
	char magic[2];
	char section[18];
	int bytesRead;
	int VERSION = 0, no_of_sections = 0, HEADER_SIZE = 0, SECT_TYPE = 0, SECT_SIZE = 0, SECT_OFFSET = 0;
	if ( (fd = open(basePath, O_RDONLY)) < 0) {
		ERR_MSG("ERROR (opening the file)");
		exit(2);
	}
	/*lstat(basePath, &inode);
	if(S_ISREG(inode.st_mode))
        return 1;
	else return 0;*/
	lseek(fd, -1, SEEK_END);
	if ((bytesRead = read(fd, magic, 1)) < 0) {
		ERR_MSG("ERROR (reading from inside the file)");
		exit(2);
	}
	magic[bytesRead] = 0;
	if(strcmp(magic, "g") != 0){
        //printf("wrong magic");
        return 0;
        exit(3);
	}
	lseek(fd, -3, SEEK_END);
	if ((bytesRead = read(fd, &HEADER_SIZE, 2)) < 0) {
		ERR_MSG("ERROR (reading from inside the file)");
		exit(3);
	}
	lseek(fd, -HEADER_SIZE, SEEK_END);
	if ((bytesRead = read(fd, &VERSION, 2)) < 0) {
		ERR_MSG("ERROR (reading from inside the file)");
		exit(3);
	}
	if( (VERSION >= 60) && (VERSION <= 144)){
        //printf("version=%d\n", VERSION);
	}
	else {
        //printf("wrong version");
        return 0;
        }

	if((bytesRead = read(fd, &no_of_sections,1)) <0){
	}
	if((no_of_sections >= 2 ) && (no_of_sections <= 18)){
        //printf("nr_sections=%d\n", no_of_sections);
	}
	else {
        //printf("wrong sect_nr");
        return 0;
        }
	for(int i = 0; i < no_of_sections; i++){
        if((bytesRead = read(fd, section, 18)) < 0){
            printf("error");
            exit(4);
        }
        section[bytesRead] = 0;
        if((bytesRead = read(fd, &SECT_TYPE, 2)) < 0){
        }
        if((SECT_TYPE == 83) || (SECT_TYPE==51) || (SECT_TYPE == 61) || (SECT_TYPE == 19) || (SECT_TYPE == 65)){}
            //ok = 1;
        else{
           // ok=0;
            //printf("wrong sect_types");
            return 0;
            }
        if((bytesRead = read(fd, &SECT_OFFSET, 4)) < 0){
        }
        if((bytesRead = read(fd, &SECT_SIZE, 4)) < 0){
        }
    }
    return 1;
}
void parse(char *basePath){
    int ok = 0;
    int fd;
	char magic[2];
	int bytesRead;
	int VERSION = 0, no_of_sections = 0, HEADER_SIZE = 0, SECT_TYPE = 0, SECT_SIZE = 0, SECT_OFFSET = 0;
	char section[19];
	if ( (fd = open(basePath, O_RDONLY)) < 0) {
		ERR_MSG("ERROR (opening the file)");
		exit(2);
	}
	lseek(fd, -1, SEEK_END);
	if ((bytesRead = read(fd, magic, 1)) < 0) {
		ERR_MSG("ERROR (reading from inside the file)");
		exit(2);
	}
	magic[bytesRead] = 0;
	if(strcmp(magic, "g") != 0){
        printf("wrong magic\n");
        exit(3);
	}
	lseek(fd, -3, SEEK_END);
	if ((bytesRead = read(fd, &HEADER_SIZE, 2)) < 0) {
		ERR_MSG("ERROR (reading from inside the file)");
		exit(3);
	}
	lseek(fd, -HEADER_SIZE, SEEK_END);
	if ((bytesRead = read(fd, &VERSION, 2)) < 0) {
		ERR_MSG("ERROR (reading from inside the file)");
		exit(3);
	}
	if( (VERSION >= 60) && (VERSION <= 144)){
        //printf("version=%d\n", VERSION);
        ok=1;
	}
	else {
        printf("wrong version\n");
        exit(4);
        }
	if((bytesRead = read(fd, &no_of_sections,1)) <0){
	}
	if((no_of_sections >= 2 ) && (no_of_sections <= 18)){
        //printf("nr_sections=%d\n", no_of_sections);
        ok=1;
	}
	else {printf("wrong sect_nr\n");
        exit(4);
        }
    for(int j = 0; j < no_of_sections; j++){
        read(fd, section, 18);
        section[bytesRead] = 0;
        read(fd, &SECT_TYPE, 2);
        if((SECT_TYPE == 83) || (SECT_TYPE==51) || (SECT_TYPE == 61) || (SECT_TYPE == 19) || (SECT_TYPE == 65))
            ok = 1;
        else{
            printf("wrong sect_types\n");
            exit(5);
            }
        read(fd, &SECT_OFFSET, 4);
        read(fd, &SECT_SIZE, 4);
    }
    if(ok == 1){
        printf("version=%d\n", VERSION);
        printf("nr_sections=%d\n", no_of_sections);
    }
    lseek(fd, -HEADER_SIZE+3, SEEK_END);
	for(int i = 0; i < no_of_sections; i++){
        if((bytesRead = read(fd, section, 18)) < 0){
            printf("error");
            exit(4);
        }
        section[bytesRead] = 0;
        if((bytesRead = read(fd, &SECT_TYPE, 2)) < 0){
        }
        if((bytesRead = read(fd, &SECT_OFFSET, 4)) < 0){
        }
        if((bytesRead = read(fd, &SECT_SIZE, 4)) < 0){
        }
        printf("section%d: %s %d %d\n", i+1, section, SECT_TYPE, SECT_SIZE);
    }
}

void extract(char *basePath, int section_nr, int line_nr){
    int fd;
	char *line, *buf;
	//strcpy(line, "");
	int bytesRead;
	int size = 0, count =1, off=0, k =0;
	int VERSION = 0, no_of_sections = 0, HEADER_SIZE = 0, SECT_TYPE = 0, SECT_SIZE = 0, SECT_OFFSET = 0;
	char section[19];
	if ( (fd = open(basePath, O_RDONLY)) < 0) {
		ERR_MSG("ERROR (opening the file)");
		exit(2);
	}
	lseek(fd, -3, SEEK_END);
	if ((bytesRead = read(fd, &HEADER_SIZE, 2)) < 0) {
		ERR_MSG("ERROR (reading from inside the file)");
		exit(3);
	}
	lseek(fd, -HEADER_SIZE, SEEK_END);
	read(fd, &VERSION, 2);
    read(fd, &no_of_sections,1);
	if(section_nr > no_of_sections){
        printf("ERROR\n");
        printf("invalid section\n");
	}
	for(int i = 1; i <= no_of_sections; i++){
        read(fd, section, 18);
        section[bytesRead] = 0;
        read(fd, &SECT_TYPE, 2);

        read(fd, &SECT_OFFSET, 4);
       // printf("sectoff %d\n", SECT_OFFSET);
        if(i== section_nr) off=SECT_OFFSET;
        read(fd, &SECT_SIZE, 4);
        if(i == section_nr) size = SECT_SIZE;
        }

	lseek(fd, 0, SEEK_SET);
	lseek(fd, off, SEEK_SET);
	//lseek(fd, off+size, SEEK_SET);
	//lseek(fd, -size, SEEK_CUR);
	buf = malloc(sizeof(char)*size);
	bytesRead=read(fd, buf, size);//in buf e toata sectiunea si suntem la sfarsit
	buf[bytesRead]=0;
    line = malloc(sizeof(char)*size+1);
	for(int j = strlen(buf) - 1 ; j >= 0 ; j--){
        if(buf[j] == '\n')
            count++;
        if(count == line_nr){
            line[k] = buf[j];
            k++;
        }
        //line[k+1]=0;

	}
	line[k+1]=0;
	//printf("%d nr linii\n", count);
	if(strcmp(line,"") == 0){
        printf("ERROR\n");
        printf("invalid line\n");
	}
	else{
        printf("SUCCESS");
        printf("%s\n", line);
    }
    free(buf);
    free(line);

}
int check_sec_size(char *basePath){
    int ok = 0;
    int fd;
	int bytesRead;
	int VERSION = 0, no_of_sections = 0, HEADER_SIZE = 0, SECT_TYPE = 0, SECT_SIZE = 0, SECT_OFFSET = 0;
	char section[18];
	if ( (fd = open(basePath, O_RDONLY)) < 0) {
		ERR_MSG("ERROR (opening the file)");
		exit(2);
	}
	lseek(fd, -3, SEEK_END);
	if ((bytesRead = read(fd, &HEADER_SIZE, 2)) < 0) {
		ERR_MSG("ERROR (reading from inside the file)");
		exit(3);
	}
	lseek(fd, -HEADER_SIZE, SEEK_END);
	read(fd, &VERSION, 2);
    read(fd, &no_of_sections,1);
	for(int i = 1; i <= no_of_sections; i++){
        read(fd, section, 18);
        section[bytesRead] = 0;
        read(fd, &SECT_TYPE, 2);
        read(fd, &SECT_OFFSET, 4);
        read(fd, &SECT_SIZE, 4);
        if(SECT_SIZE > 1455) ok = 1;
    }
    if(ok==1) return 0;
    else return 1;
}
void findall(char *dirName){
    char path[MAX_PATH_LEN];
    struct dirent *dp;
    struct stat inode;
    DIR *dir = opendir(dirName);
    if (!dir)
        return;
    while ((dp = readdir(dir)) != NULL){
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
            snprintf(path, sizeof(path)+1, "%s/%s", dirName, dp->d_name);
            lstat(path, &inode);
            if(S_ISREG(inode.st_mode)){
                if((parse_err(path) != 0) && (check_sec_size(path) == 1))
                    printf("%s\n", path);
            }

            strcpy(path, dirName);
            strcat(path, "/");
            strcat(path, dp->d_name);
            findall(path);

        }
    }
    closedir(dir);
}

int main(int argc, char **argv){
    char path[MAX_PATH_LEN]; //for path
    char fpath[MAX_PATH_LEN]; //for filter by size
    char ffpath[MAX_PATH_LEN];
    int filter1, filter2, filter3;
    int rec = 1, fbs = 1, fbp = 1;
    int a,b,c;
    struct stat fileMetadata;
	long int value, value2; //for filter by size

    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0){
            printf("25096\n");
        }
        //LIST!!!!!!
	    if(strcmp(argv[1], "list") == 0) {
            for(int i = 2; i < argc; i++){
                if(strcmp(argv[i], "recursive") == 0){
                    rec = 0;
                }
                filter1 = find_str(argv[i], "size_smaller=");
                if(filter1 == 1){
                    fbs = 0;
                    b = i;
                }
                filter2 = find_str(argv[i], "path=");
                if(filter2 == 1){
                    c = i;
                }
                filter3 = find_str(argv[i], "has_perm_write");
                if(filter3 == 1){
                    fbp = 0;
                    a = i;
                }
            }
            if(fbs == 0){
                strcpy(path, argv[c]);
                sscanf(path,"path=%s", argv[c]);
                strcpy(fpath, argv[b]);
                sscanf(fpath,"size_smaller=%s", argv[b]);
                value = atoi(argv[b]);
                if (stat(argv[c], &fileMetadata) < 0) {
                    ERR_MSG("ERROR (getting info about the file)");
                    exit(2);
                }
                if (S_ISDIR(fileMetadata.st_mode)) { // it is a directory
                    printf("SUCCESS\n");
                    list_bysize(argv[c], value, rec);
                } else {
                    printf("invalid directory path");
                    exit(3);
                }
            }
            else{
                if(fbp == 0){
                    strcpy(path, argv[c]);
                    sscanf(path,"path=%s", argv[c]);
                    strcpy(fpath, argv[a]);
                    if(strcmp(fpath, "has_perm_write") == 0){
                        if (stat(argv[c], &fileMetadata) < 0) {  // get info
                            ERR_MSG("ERROR (getting info about the file)");
                            exit(2);
                        }
                        if (S_ISDIR(fileMetadata.st_mode)) { // it is a directory
                            printf("SUCCESS\n");
                            list_byperm(argv[c], rec);
                        } else {
                            printf("invalid directory path");
                            exit(3);
                        }
                    }
                }
                else{
                    if(rec == 0){
                        strcpy(path, argv[c]);
                        sscanf(path,"path=%s", argv[c]);
                        if (stat(argv[c], &fileMetadata) < 0) {  // get info
                            ERR_MSG("ERROR (getting info about the file)");
                            exit(2);
                        }
                        if (S_ISDIR(fileMetadata.st_mode)) {
                            printf("SUCCESS\n");
                            list_rec(argv[c]);
                        } else {
                            printf("invalid directory path");
                            exit(3);
                        }
                    }
                    else{
                        strcpy(path, argv[c]);
                        sscanf(path,"path=%s", argv[c]);
                        if (stat(argv[c], &fileMetadata) < 0) {  // get info
                            ERR_MSG("ERROR (getting info about the file)");
                            exit(2);
                        }
                        if (S_ISDIR(fileMetadata.st_mode)) {
                            printf("SUCCESS\n");
                            listDir(argv[c]);
                        } else {
                            printf("invalid directory path");
                            exit(3);
                        }
                    }
                }
            }
        }
        //PARSE!!!!
        if(strcmp(argv[1], "parse") == 0){
            strcpy(path, argv[2]);
            sscanf(path,"path=%s", argv[2]);
            if (stat(argv[2], &fileMetadata) < 0) {  // get info
                ERR_MSG("ERROR (getting info about the file)");
                exit(2);
            }
            if (S_ISREG(fileMetadata.st_mode)) {
                if(parse_err(argv[2]) == 0){
                    printf("ERROR\n");
                    parse(argv[2]);
                }
                else{
                printf("SUCCESS\n");
                parse(argv[2]);
                }
            } else {
                printf("invalid directory path");
                exit(3);
            }
        }

        //EXTRACT
        if(strcmp(argv[1], "extract") == 0){
            strcpy(path, argv[2]);
            sscanf(path,"path=%s", argv[2]);
            if (stat(argv[2], &fileMetadata) < 0) {  // get info
                ERR_MSG("ERROR (getting info about the file)");
                exit(2);
            }
            strcpy(fpath, argv[3]);
            sscanf(fpath, "section=%s", argv[3]);
            value = atoi(argv[3]);
            strcpy(ffpath, argv[4]);
            sscanf(ffpath, "line=%s", argv[4]);
            value2 = atoi(argv[4]);
            if (S_ISREG(fileMetadata.st_mode)) {
                if(parse_err(argv[2]) == 0){
                    printf("ERROR\n");
                    printf("invalid file");
                }
                else{
                    extract(argv[2], value, value2);
                }
          }
        }

        //findall
        if(strcmp(argv[1], "findall") == 0){
            strcpy(path, argv[2]);
            sscanf(path,"path=%s", argv[2]);
            if (stat(argv[2], &fileMetadata) < 0) {  // get info
                ERR_MSG("ERROR (getting info about the file)");
                exit(2);
                }
            if (S_ISDIR(fileMetadata.st_mode)) {
                printf("SUCCESS\n");
                findall(argv[2]);
            } else {
                printf("invalid directory path");
                exit(3);
            }
        }
    }
    return 0;
}
