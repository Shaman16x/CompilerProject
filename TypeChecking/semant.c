#include "semant.h"

// implementation based off of suggestions in the book

expty expTy(Tr_exp exp, Ty_ty ty){
    expty e; e.exp=exp; e.ty=ty; return e;
}

expty   transVar(S_table venv, S_table tenv, A_var v)
{
    return expTy(NULL, Ty_Void());
}

// translates expressions
expty   transExp(S_table venv, S_table tenv, A_exp a) {
    switch (a->kind) {
        case A_varExp:
            //return transVar(venv, tenv, a->var);
        case A_nilExp:
            // not sure about this one
            return expTy(NULL, Ty_Nil());
            break;
        case A_intExp:
            return expTy(NULL, Ty_Int());
            break;
        case A_stringExp:
            return expTy(NULL, Ty_String());
            break;
        case A_callExp:
            // function
        case A_opExp: {
            A_oper oper = a->u.op.oper;
            expty left = transExp(venv, tenv, a->u.op.left);
            expty right = transExp(venv, tenv, a->u.op.right);
            switch (oper){
                case A_plusOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(NULL, Ty_Int());
                case A_minusOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(NULL, Ty_Int());
                case A_timesOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(NULL, Ty_Int());
                case A_divideOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(NULL, Ty_Int());
                case A_eqOp:
                    // this is special, can be used on records and arrays
                    return expTy(NULL, Ty_Int());
                case A_neqOp:
                    // this is special, can be used on records and arrays
                    return expTy(NULL, Ty_Int());
                case A_ltOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(NULL, Ty_Int());
                case A_leOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(NULL, Ty_Int());
                case A_gtOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(NULL, Ty_Int());
                case A_geOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(NULL, Ty_Int());
                default:
                    printf("this is an unhandled operation!\n");
                
            }
            // requires a lot of work
            }
        case A_recordExp:
        case A_seqExp:
        case A_assignExp:
        case A_ifExp:
        case A_whileExp:
        case A_forExp:
        case A_breakExp:
        case A_letExp:
        case A_arrayExp:
        default:
            printf("what IS this?\n");
    }
}

void    transDec(S_table venv, S_table tenv, A_dec d)
{
}

Ty_ty   transTy (              S_table tenv, A_ty a)
{
    
}

void SEM_transProg(A_exp exp)  {
    expty tree;
    
    transExp(S_empty(), S_empty(), exp);
    printf("I did it?\n");
    
}


