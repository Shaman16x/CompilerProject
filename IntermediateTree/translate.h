#include <stdio.h>
#include <stdlib.h>
#include "frame.h"
#include "errormsg.h"
#include "types.h"
#include "absyn.h"

/* translate header */

typedef struct Tr_access_ *Tr_access;
typedef int patchList;
typedef struct Tr_level_ *Tr_level;       // TODO: find the true values of these
struct Tr_level_ {
    Tr_level parent;
    Temp_label name;
    F_frame frame;
};

typedef struct Tr_accessList_ *Tr_accessList;
struct Tr_accessList_ {Tr_access head; Tr_accessList tail; };

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail);

Tr_level Tr_outermost(void);
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals);

Tr_accessList Tr_formals(Tr_level level);
Tr_access Tr_allocLocal(Tr_level level, bool escape);

typedef struct Tr_exp_ * Tr_exp;


static Tr_exp Tr_Ex(T_exp ex);
static Tr_exp Tr_Nx(T_stm nx);
static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm);
