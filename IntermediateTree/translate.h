#include <stdio.h>
#include <stdlib.h>
#include "frame.h"
#include "errormsg.h"
#include "types.h"
#include "absyn.h"
#include "printtree.h"

/* translate header */

typedef struct Tr_access_ *Tr_access;
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

// all transalation functions
Tr_exp Tr_int(int i);
Tr_exp Tr_oper(A_oper oper, Tr_exp left, Tr_exp right);
Tr_exp Tr_simpleVar(Tr_access acc, Tr_level level);
Tr_exp Tr_assign(Tr_exp var, Tr_exp exp);
Tr_exp Tr_let(void);
Tr_exp Tr_if(Tr_exp test, Tr_exp then, Tr_exp elsee);
Tr_exp Tr_while(Tr_exp test, Tr_exp body, Temp_label done);
Tr_exp Tr_for(void);
Tr_exp Tr_break(Temp_label done);
