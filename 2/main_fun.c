
#include "main_fun.h"


// 0< - ERR; 0 == match; 0> match + arg 
int strcmpno(char* A, char* B){
	int no = 0;
	for(; *A; ++A){
		if( *A == 'N' ){
			if( *B < '0' || *B > '9' ) return ERR_NO_SUCH_FLAG;
			
			for(; *B >= '0' && *B <= '9'; ++B){
				if( INT_MAX / 10 < no ) return SP_TOO_BIG_NUM;
				if(no * 10 > INT_MAX - (*B - '0')) return SP_TOO_BIG_NUM;
				no = no * 10 + (*B - '0');
			}
		}
		else if(*A != *B) return ERR_NO_SUCH_FLAG;
		else ++B;
	}
	if(*A != *B) return ERR_NO_SUCH_FLAG;
	return no;
}


int processHandles(struct FlagHandle* handles, int argc, int argn, char** argv){
	while(handles->handle != NULL){
		if(strcmpno(handles->flag, argv[argn]) >= 0) return handles->handle(argc, argv);
		++handles;
	}
	return ERR_NO_SUCH_FLAG;
}


int printMemReg(void *ptr, size_t size, size_t bits){
    unsigned char *byte_ptr = (unsigned char *)ptr;
    for(size_t i = 0; i < size; i++){
        unsigned char byte = byte_ptr[i];
    
        for(int j = ((7 < bits) ? 7 : bits); j >= 0; j--){
            putchar((byte & (1 << j)) ? '1' : '0');
            if (ferror(stdout)){
                return SP_WRITE_ERR;
            }
        }
    }
	putchar('\n');
    if (ferror(stdout)){
        return SP_WRITE_ERR;
    }
	return 0;
}

int handleXorN(int argc, char** argv) {
	int value = strcmpno("xorN", argv[argc-1]);
	if(value < 2 || value > 8) return ERR_OUT_OF_RANGE;
	
	size_t bytes = ceil(pow(2, value) / 8);
	char* storage = (char*)calloc(bytes, 1);
	if(storage == NULL) return ERR_OUT_OF_MEMORY;
	
	for(size_t i = 1; i < argc - 1; i++){
		FILE* handle = fopen(argv[i], "r");
		if(handle == NULL){
            free(storage);
            return ERR_NO_SUCH_RESOURCE;
        }
		if(ferror(handle)){
            free(storage);
            return ERR_NO_SUCH_RESOURCE;
        }
		int c = 0;
		while(!feof(handle)){
            c = fgetc(handle);
			if(ferror(handle)) {
                fclose(handle);
                free(storage);
                return ERR_NO_SUCH_RESOURCE;
            }
			storage[i % bytes] ^= (char)(c);
			
			if(value == 2) storage[i % bytes] ^= storage[i % bytes] >> 4;
		}
		if (printMemReg(storage, bytes, pow(2, value) - 1) < 0){
            free(storage);
            fclose(handle);
            return SP_WRITE_ERR;
        }
		fclose(handle);
	}

	free(storage);
	return 0;
}


char* toHex(size_t num, char* trg, size_t len){
    char digits[] = "0123456789ABCDEF";
    int index = 0;
	
	trg[--len] = '\0';

    while(num > 0){
        trg[--len] = digits[num % 16];
        num /= 16;
    }
	
	return &trg[len];
}

int isHexTemplate(char* template, char* hex){
	do {
		switch(*template){
			case '?':
				break;
			case '*':
				while( *(hex+1) != *(template+1) && *(hex+1) != '\0' )
					++hex;
				break;
			default:
				if(*hex != *template) return 0;
				break;
		}
		
		++hex;
		++template;
	} while((*template != '\0') && (*hex != '\0'));
	
	if(*template != *hex) return 0; 
	
	return 1;
}


int correct_hex(const char* ptr){
    // char corr_sym[17] = "0123456789ABCDEF";
    while (*ptr != '\0') {
        if ((!(((*ptr - '0' >= 0) && (*ptr - '0') <= 9)
        || (toupper(*ptr) - 'A' >= 0 && toupper(*ptr) - 'A' <= 'F' - 'A')))
        && (*ptr != '*' && *ptr != '?')){
            return 0;
        }
        ++ptr;
    }
    return 1;
}

int handleMask(int argc, char** argv) {
	char data[10] = {0};
	size_t no = 0;

    if (!correct_hex(argv[argc - 1])) return SP_WRONG_MASK;
    

	for(size_t i = 1; i < argc - 2; i++){
		FILE* handle = fopen(argv[i], "r");
		if(ferror(handle)) return ERR_NO_SUCH_RESOURCE;
		if(handle == NULL) return ERR_NO_SUCH_RESOURCE;
		
		int read = 0;
		uint32_t c = 0;
		while((read=fread(&c, 4, 1, handle)) != 0) {
			if(ferror(handle)) return ERR_NO_SUCH_RESOURCE;
			char* hex = toHex(c, data, 10);
			
			if(isHexTemplate(argv[argc - 1], hex)) {
				no++;
			}
            else{
				fseek(handle, -(read-1 < 3 ? read - 1 : 3), SEEK_CUR);
			}
		}
		
		printf("%ld\n", no);
		fclose(handle);
	}
	
	return 0;
}


int handleCopyN(int argc, char** argv){
	int value = strcmpno("copyN", argv[argc-1]);
	if (value <= 0) return value;
	int count = 0;
	int err = 0;
	int buff_err = 0;
	for(size_t argn = 1; argn < argc - 1; ++argn){
		pid_t pid = fork();
		switch(pid){
			case -1:
				for (;count > 0; --count){
					wait(&buff_err);
					// err |= buff_err;
				}
				return ERR_NO_SUCH_RESOURCE;
			case 0: {
				char cwd[1024] = {0};
				char out[1536] = {0};
				char txt[32] = {0};
				
				getcwd(cwd, sizeof(cwd));
				sprintf(out, SP_COPYN_EXE, cwd);
				sprintf(txt, "%d", value);
				char* args[] = {out, txt, argv[argn], NULL};
				if(execv(out, args) == -1) return ERR_NO_SUCH_RESOURCE;
				break;
            }
			default:
				++count;
				break;
		}
	}
	for (;count > 0; --count){
		wait(&buff_err);
		err |= buff_err;
	}
	return err;
}


int handleFind(int argc, char** argv){
	int count = 0;
	int err = 0;
	int buff_err = 0;
	for(size_t argn = 1; argn < argc - 2; ++argn){
		pid_t pid = fork();
		switch(pid){
			case -1:
				for (;count > 0; --count){
					wait(&buff_err);
				//	err |= buff_err;
				}
				return ERR_NO_SUCH_RESOURCE;
			case 0:{ 
				char cwd[1024];
				char out[1536];
				char txt[32];
				
				getcwd(cwd, sizeof(cwd));
				sprintf(out, SP_FINDER_EXE, cwd);
				char* args[] = {out, argv[argc-1], argv[argn], NULL};
				
				if(execv(out, args) == -1) return ERR_NO_SUCH_RESOURCE;
				break;
            }
			default:
				++count;
				break;
		}
	}
	for (;count > 0; --count){
		wait(&buff_err);
		err |= buff_err;
	}
	return err;
}


