
#include "sp2_com.h"


int str_found(const char *filename, const char *search_str) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return ERR_NO_SUCH_RESOURCE;
	
    char line[1024];
	size_t read;
    while((read=fread(line, 1, sizeof(line) - 1, file )) > 0){
		if(ferror(file)) return ERR_NO_SUCH_RESOURCE;
        if (strstr(line, search_str) != NULL){
            fclose(file);
            return 1;
        }
        else{
			fseek(file, -(read < sizeof(line) ? read - 1 : sizeof(line)-1), SEEK_CUR);
		}
    }

    fclose(file);
    return 0;
}

int main(int argc, char** argv){
	if(argc != 3) return ERR_OUT_OF_RANGE;
	
	char* path = argv[2];
	char* str = argv[1];
		
	int err = str_found(path, str);
	
	if(err < 0) return err;
	if(err == 1) printf("Found - %s\n", path);
	return 0;
}

