
#include "main_fun.h"

int main(int argc, char** argv){
	struct FlagHandle handles[] = {
		(struct FlagHandle){.flag="xorN", .handle=handleXorN},
		(struct FlagHandle){.flag="mask", .handle=handleMask},
		(struct FlagHandle){.flag="copyN", .handle=handleCopyN},
		(struct FlagHandle){.flag="find", .handle=handleFind},
		(struct FlagHandle){.flag='\0', .handle=NULL}
	};
	
	int err = processHandles(handles, argc, argc - 1, argv);
	if(err == ERR_NO_SUCH_FLAG && argc > 2) err = processHandles(handles, argc, argc - 2, argv);
	
	if(err) printf(SP_TXT_ERR, err);
	return err;
}

