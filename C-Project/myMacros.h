#pragma once
#include <stdio.h>

#define CHECK_RETURN_0(ptr){\
	 if (ptr == NULL)\
		return 0;\
}

#define CHECK_RETURN_NULL(ptr){\
	if (ptr == NULL)\
		return NULL;\
}

#define CHECK_MSG_RETURN_0(ptr, str){\
	if (ptr == NULL){\
		printf(#str "\n");\
		return 0;\
	}\
}

#define CHECK_0_MSG_CLOSE_FILE(val, fp, str){\
	if (val == 0){\
		printf(#str "\n");\
		fclose(fp);\
		return 0;\
	}\
} 

#define CHECK_NULL_MSG_CLOSE_FILE(val, fp, str){\
	if(val == NULL){\
		printf(#str "\n");\
		fclose(fp);\
		return 0;\
	}\
}

#define MSG_CLOSE_RETURN_0(fp, str){\
	printf(#str "\n");\
	fclose(fp);\
	return 0;\
}
