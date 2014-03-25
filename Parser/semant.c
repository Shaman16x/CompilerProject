#include "semant.h"
#include "util.h"
#include "errormsg.h"
#include "env.h"
#include "types.h"
#include <assert.h>
#include <stdlib.h>

void SEM_transProg(A_exp exp)  {
}

static struct expty transExp(S_table venv, S_table tenv, A_exp a);
static struct expty transVar(S_table venv, S_table tenv, A_var v);
static Tr_exp transDec(S_table venv, S_table tenv, A_dec d);
static Ty_ty transTy(S_table tenv, A_ty t);

static struct expty transExp(S_table venv, S_table tenv, A_exp a)
{
	switch (a->kind) {
		
		case A_callExp:
		{
			A_expList args = NULL;
			Ty_tyList formals;
			E_enventry x = S_look(venv, a->u.call.func);
			Tr_exp translation = Tr_noExp();
			Tr_expList argList = Tr_ExpList();
			if (x && x->kind == E_funEntry) {
				// check type of formals
				formals = x->u.fun.formals;
				for (args = a->u.call.args; args && formals;
						args = args->tail, formals = formals->tail) {
					struct expty arg = transExp(level, venv, tenv, breakk, args->head);
					if (!is_equal_ty(arg.ty, formals->head))
						EM_error(args->head->pos, "incorrect type %s; expected %s",
							Ty_ToString(arg.ty), Ty_ToString(formals->head));
					Tr_ExpList_append(argList, arg.exp);
				}
				/* Check we have the same number of arguments and formals */
				if (args == NULL && formals != NULL)
					EM_error(a->pos, "not enough arguments");
				else if (args != NULL && formals == NULL)
					EM_error(a->pos, "too many arguments");
				translation = Tr_callExp(level, x->u.fun.level, x->u.fun.label, argList);
				return expTy(translation, actual_ty(x->u.fun.result));
			} else {
				EM_error(a->pos, "undefined function %s", S_name(a->u.call.func));
				return expTy(translation, Ty_Int());
			}
		}
		case A_opExp:{
			A_oper oper = a->u.op.oper;
			struct expty left = transExp(level, venv, tenv, breakk, a->u.op.left);
			struct expty right = transExp(level, venv, tenv, breakk, a->u.op.right);
			
			switch (oper) {
				case A_plusOp:
				case A_minusOp:
				case A_timesOp:
				case A_divideOp:
					if (left.ty->kind != Ty_int)
						EM_error(a->u.op.left->pos, "integer required");
					if (right.ty->kind != Ty_int)
						EM_error(a->u.op.right->pos, "integer required");
					return expTy(NULL, Ty_Int());
					
				case A_eqOp:
				case A_neqOp:
				
					if(left.ty->kind == right.ty->kind){
						switch(left.ty->kind){
							case Ty_int:
								return expTy(NULL, Ty_Int());
							case Ty_string:
								return expTy(NULL, Ty_String());
							case Ty_array:
								return expTy(NULL, Ty_Array());
							case Ty_record:
								return expTy(NULL, Ty_Record());
							assert(0);
						}
					}
					else{
						EM_error(a->u.op.right->pos, "comparison of unequal types");
					}
					
					
				
				case A_ltOp:
				case A_leOp:
				case A_gtOp:
				case A_geOp:{
					if (right.ty->kind == left.ty->kind) {
						switch(left.ty->kind) {
							case Ty_int:
								return expTy(NULL, Ty_Int());
							case Ty_string:
								return expTy(NULL, Ty_String());
							assert(0);
						}
					}
					else{
						EM_error(a->u.op.right->pos, "comparison of unequal types");
					}
				}
			}
			assert(0);
			
		}
		
		
	}
	assert(0);
}
