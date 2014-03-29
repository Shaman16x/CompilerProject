#include <stdio.h>
#include <stdlib.h>
#include "translate.h"
#include "errormsg.h"


struct expty {Tr_exp exp; Ty_ty ty;};
typedef struct expty expty;
expty expTy(Tr_exp exp, Ty_ty ty);

expty   transVar(S_table venv, S_table tenv, A_var v, Tr_level level);
expty   transExp(S_table venv, S_table tenv, A_exp a, Tr_level level);
void    transDec(S_table venv, S_table tenv, A_dec d, Tr_level level);
Ty_ty   transTy (              S_table tenv, A_ty a);

Ty_tyList makeFormalTyList(S_table tenv, A_fieldList params);    // makes a list of type based on params
int compareTypes(Ty_ty a, Ty_ty b);
Ty_ty actual_ty(Ty_ty t);

// env.h
// *******************************************************
// Eviroment stuff that we don't have for some reason
typedef struct E_enventry_ *E_enventry;

struct E_enventry_ {enum {E_varEntry, E_funEntry} kind;
                    union {struct {Tr_access access; Ty_ty ty;} var;
                            struct {Tr_level level; Temp_label label;
                                    Ty_tyList formals; Ty_ty result;} fun;
                        } u;
                };
                
E_enventry E_VarEntry (Tr_access access, Ty_ty ty);
E_enventry E_FunEntry (Tr_level level, Temp_label label, Ty_tyList formals, Ty_ty result);

S_table E_base_tenv(void);      // creates a type table with basic types
S_table E_base_venv(void);

// *******************************************************

void SEM_transProg(A_exp exp);

