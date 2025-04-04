
#ifndef SP_2_INCL
#define SP_2_INCL

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#define ERR_NO_SUCH_FLAG -256
#define ERR_OUT_OF_RANGE -255
#define ERR_OUT_OF_MEMORY -254
#define ERR_NO_SUCH_RESOURCE -253
#define SP_TOO_BIG_NUM -252
#define SP_WRITE_ERR -251
#define SP_WRONG_MASK -250
#define SP_COPYN_EXE "%s/copy_file.elf"
#define SP_FINDER_EXE "%s/finder.elf"
#define SP_TXT_ERR "An error occurred: #%d\n"

#endif