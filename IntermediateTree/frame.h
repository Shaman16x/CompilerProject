#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"

typedef struct F_frame_ *F_frame;
typedef struct F_access_ *F_access;

typedef struct F_accessList_ *F_accessList;

/*
struct F_accessList_ {
	F_access head;
	F_accessList tail;
};
*/
F_frame F_newFrame(Temp_label name, U_boolList formals);
Temp_label F_name(F_frame frame);
F_accessList F_formals(F_frame frame);
F_access F_allocLocal(F_frame frame, bool escape);

