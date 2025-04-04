
#include "sp2_com.h"


int copy_file(char* source, char* destination) {
	FILE* src;
	FILE* dst;
	
    if((src = fopen(source, "rb")) == NULL) return ERR_NO_SUCH_RESOURCE;
	
	if((dst=fopen( destination, "wb" )) == NULL){
		fclose(src);
		return ERR_NO_SUCH_RESOURCE;
	}

    char buffer[1024];
    size_t bytesRead;
	
    while((bytesRead = fread( buffer, 1, sizeof(buffer), src )) > 0)  {
		if(ferror(src)) return ERR_NO_SUCH_RESOURCE;
		fwrite(buffer, 1, bytesRead, dst); 
		if(ferror(dst)) return ERR_NO_SUCH_RESOURCE;
	}

    fclose(src);
    fclose(dst);
	
	return 0;
}

int main(int argc, char** argv){
	if(argc != 3) return ERR_OUT_OF_RANGE;
	
	int num = atoi(argv[1]);
	char* path = calloc(strlen(argv[2]), sizeof(char));
	strcpy(path, argv[2]);
	char* extension = NULL;
	
	for(ssize_t i = strlen(path)-1; i > -1; --i){
		if(path[i] == '.'){
			path[i] = '\0';
			extension = &path[i+1];
			break;
		}
	}
	
	size_t err = 0;
	size_t strsize = strlen(argv[2]) + 33;
	char* dest = (char*)calloc(strsize, sizeof(char));
	if (dest == NULL){
		return ERR_OUT_OF_MEMORY;
	}
	
	for(size_t i = 0; i < num; ++i){
		snprintf(dest, strsize, "%s%ld.%s", path, i, extension);
		err = copy_file(argv[2], dest);
		if(err){
			free(dest);
			return err;
		}
	}
	
	free(dest);
	return err;
}

