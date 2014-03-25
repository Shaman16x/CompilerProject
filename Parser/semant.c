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
		
		case A_letExp:
		{
			struct expty exp;
			A_decList d;
			Tr_expList list = Tr_ExpList();
			S_beginScope(venv);
			S_beginScope(tenv);
			for (d = a->u.let.decs; d; d = d->tail)
				transDec(venv, tenv, d->head));
			exp = transExp(venv, tenv, a->u.let.body);
			Tr_ExpList_prepend(list, expr.exp); 
			S_endScope(venv);
			S_endScope(tenv);
			return exp;
		}
		
	}
	assert(0);
}
