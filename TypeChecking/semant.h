#include <stdio.h>
#include <stdlib.h>
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

Ty_tyList makeFormalTyList(S_table tenv, A_fieldList params);    // makes a list of type based on params

// env.h
// *******************************************************
// Eviroment stuff that we don't have for some reason
typedef struct E_enventry_ *E_enventry;

struct E_enventry_ {enum {E_varEntry, E_funEntry} kind;
                    union {struct {Ty_ty ty;} var;
                            struct {Ty_tyList formals; Ty_ty result;} fun;
                        } u;
                };
                
E_enventry E_VarEntry (Ty_ty ty);
E_enventry E_FunEntry (Ty_tyList formals, Ty_ty result);

S_table E_base_tenv(void);      // creates a type table with basic types
S_table E_base_venv(void);

// *******************************************************

void SEM_transProg(A_exp exp);

