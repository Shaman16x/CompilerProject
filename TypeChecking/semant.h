#include <stdio.h>
#include "util.h"
#include "symbol.h" 
#include "errormsg.h"
#include "types.h"
#include "absyn.h"

typedef void *Tr_exp;
struct expty {Tr_exp exp; Ty_ty ty;};
typedef struct expty expty;
expty expTy(Tr_exp exp, Ty_ty ty);

expty   transVar(S_table venv, S_table tenv, A_var v);
expty   transExp(S_table venv, S_table tenv, A_exp a);
void    transDec(S_table venv, S_table tenv, A_dec d);
Ty_ty   transTy (              S_table tenv, A_ty a);


void SEM_transProg(A_exp exp);



