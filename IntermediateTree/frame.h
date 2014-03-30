#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"

typedef struct F_frame_ *F_frame;
typedef struct F_access_ *F_access;
typedef struct F_accessList_ *F_accessList;

struct F_frame_ {Temp_label name; F_accessList formals;};
struct F_accessList_ {F_access head;F_accessList tail;};

F_frame F_newFrame(Temp_label name, U_boolList formals);
Temp_label F_name(F_frame frame);
F_accessList F_formals(F_frame frame);
F_access F_allocLocal(F_frame frame, bool escape);

Temp_temp F_FP(void);       // location of frame pointer
extern const int F_wordSize;    // size of registers
T_exp F_Exp(F_access acc, T_exp framePtr);


typedef struct F_frag_ * F_frag;
struct F_frag_ {enum {F_stringFrag, F_procFrag} kind;
				union {
					struct {Temp_label label;
							string str;} stringg;
					struct {T_stm body; F_frame frame;} proc;
				} u;
			};
F_frag F_StringFrag(Temp_label label, string str);
F_frag F_ProcFrag(T_stm body, F_frame frame);

typedef struct F_fraglist_ *F_fragList;
struct F_fragList_ {F_frag head; F_fragList tail;};
F_fragList F_FragList(F_frag head, F_fragList tail);
