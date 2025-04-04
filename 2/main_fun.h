
#ifndef SP_2_P
#define SP_2_P


#include "sp2_com.h"


struct FlagHandle {
	char flag[16];
	int (*handle)(int, char**);
};

int strcmpno(char* A, char* B);
int processHandles(struct FlagHandle* handles, int argc, int argn, char** argv);
int printMemReg(void *ptr, size_t size, size_t bits);
int handleXorN(int argc, char** argv);

int correct_hex(const char* ptr);
char* toHex(size_t num, char* trg, size_t len);
int isHexTemplate(char* template, char* hex);
int handleMask(int argc, char** argv);

int handleCopyN(int argc, char** argv);
int handleFind(int argc, char** argv);

#endif