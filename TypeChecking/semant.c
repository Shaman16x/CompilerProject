#include "semant.h"

// implementation based off of suggestions in the book

// ****************************************************
E_enventry E_VarEntry (Ty_ty ty)
{
    E_enventry e;
    e->kind = E_varEntry;
    e->u.var.ty = ty;
    return e;
}
E_enventry E_FunEntry (Ty_tyList formals, Ty_ty result)
{
    E_enventry e;
    e->kind = E_funEntry;
    e->u.fun.result = result;
    e->u.fun.formals = formals;
    return e;
}
// ****************************************************

expty expTy(Tr_exp exp, Ty_ty ty){
    expty e; e.exp=exp; e.ty=ty; return e;
}

expty   transVar(S_table venv, S_table tenv, A_var v)
{
    switch(v->kind){
        case A_simpleVar:{
            E_enventry x = S_look(venv, v->u.simple);
            if (x && x->kind == E_varEntry)
                return expTy(NULL, x->u.var.ty);    // TODO: get the "actual type" as opposed to a Name type
            else {
                EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
                return expTy(NULL, Ty_Int());   // default type
            }
        }
        
        // handle record accesses
        case A_fieldVar: {
            expty rec = transVar(venv, tenv, v->u.field.var);
            Ty_fieldList f;
            
            // check that this var is a record
            if (rec.ty->kind == Ty_record) {
                for (f = rec.ty->u.record; f; f=f->tail){  // TODO: better accessors?
                    if(f->head->name == v->u.field.sym) {     // check for matching symbol
                        return expTy(NULL, f->head->ty);    // TODO: get true type?
                    }
                }
                // could not find the record
                EM_error(v->pos, "Field %s was not in record", S_name(v->u.field.sym));
                return expTy(NULL, Ty_Int());   // default type
            }
            else {
                EM_error(v->pos, "Not a record");
                return expTy(NULL, Ty_Int());   // default type
            }
        }
        
        // handle array accesses
        case A_subscriptVar: {
            expty arr = transVar(venv, tenv, v->u.subscript.var);
            expty e =   transExp(venv, tenv, v->u.subscript.exp);
            
            // check that this var is an array
            if (arr.ty->kind == Ty_array) {
                if(e.ty->kind == Ty_int){
                    return expTy(NULL, arr.ty->u.array);    // return the array's type
                }
                else {
                    EM_error(v->u.subscript.exp->pos, "Integer required");
                    return expTy(NULL, Ty_Int());   // default type
                }
            }
            else {
                EM_error(v->pos, "Not an array");
                return expTy(NULL, Ty_Int());   // default type
            }
        }
        
        default:
            printf("What IS this?...variable\n");
    }
    
    return expTy(NULL, Ty_Void());
}

// translates expressions
expty   transExp(S_table venv, S_table tenv, A_exp a) {
    switch (a->kind) {
        case A_varExp:
            // translate variable
            return transVar(venv, tenv, a->u.var);
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
           {
           /* i'm not sure this is right. logically, it seems to be correct
           	but i can't get it to fail a test. 
           */
	    expty arg;
            // the list of args
            A_expList arg_list = NULL;
            // the list of types of the args
            Ty_tyList formals;
            E_enventry e = S_look(venv, a->u.call.func);
            if (e && e->kind == E_funEntry){
		formals = e->u.fun.formals;
		// check the types of the formal args
		for (arg_list = a->u.call.args; arg_list && formals; 
					arg_list = arg_list->tail, formals = formals->tail){
			arg = transExp(venv, tenv, arg_list->head);
			if (arg.ty != formals->head)
				EM_error(a->u.op.left->pos, "mismatch of types");
		}
		// when the for loop exits, we're at the end of at least 
		// one of the lists: args or formals. if one is not null,
		// then there is an incorrect number of arguments
		if((arg_list == NULL && formals != NULL) || 
			(formals == NULL && arg_list != NULL)){
			EM_error(a->u.op.left->pos, "incorrect number of arguments");
		}
		return expTy(NULL, Ty_Int());
		// check the return type
	}
            break;
        } 
            break;
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
                    switch (left.ty->kind){
			case Ty_int:
				if(right.ty->kind != Ty_int)
					EM_error(a->u.op.right->pos, "Integer required");
				break;
			case Ty_string:
				if(right.ty->kind != Ty_string)
					EM_error(a->u.op.right->pos, "string required");
				break;
			case Ty_array:
				if(right.ty->kind != Ty_array)
					EM_error(a->u.op.right->pos, "array required");
				break;
			case Ty_record:
				if(right.ty->kind != Ty_record || right.ty->kind != Ty_nil)
					EM_error(a->u.op.right->pos, "record required");
				break;
					}
                    return expTy(NULL, Ty_Int());
                case A_neqOp:
                    switch (left.ty->kind){
			case Ty_int:
				if(right.ty->kind != Ty_int)
					EM_error(a->u.op.right->pos, "Integer required");
				break;
			case Ty_string:
				if(right.ty->kind != Ty_string)
					EM_error(a->u.op.right->pos, "string required");
				break;
			case Ty_array:
				if(right.ty->kind != Ty_array)
					EM_error(a->u.op.right->pos, "array required");
				 break;
			case Ty_record:
				if(right.ty->kind != Ty_record || right.ty->kind != Ty_nil)
					EM_error(a->u.op.right->pos, "record required");
				break;
					}
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
            // TODO: this
            break;
            
        case A_seqExp: {
            A_expList e;
            expty exp;
            
            for(e = a->u.seq; e->tail; e=e->tail)
                transExp(venv, tenv, e->head);
            exp = transExp(venv, tenv, e->head);     // type of sequence is that of the last expression
            return exp;
        }
        case A_assignExp: {
            expty var = transVar(venv, tenv, a->u.assign.var);
            expty exp = transExp(venv, tenv, a->u.assign.exp);
            
            if (var.ty->kind != exp.ty->kind)
                EM_error(a->u.assign.exp->pos, "Mismatch of type in assignment");
                
            return expTy(NULL, Ty_Void());
        }
        case A_ifExp:{
            expty test = transExp(venv, tenv, a->u.iff.test);
            expty then = transExp(venv, tenv, a->u.iff.then);
            expty elsee;
            
            // check E for type int
            if(test.ty->kind != Ty_int)
                EM_error(a->u.iff.test->pos, "Test for if statement must evaluate to integer");
                
            // check that else and then match types
            if(a->u.iff.elsee->kind != Ty_nil) {
                elsee = transExp(venv, tenv, a->u.iff.elsee);
                if(then.ty->kind != elsee.ty->kind)
                    EM_error(a->u.iff.elsee->pos, "Else type must match Then type");
                
                return then;    // return then's type
            }
            return expTy(NULL, Ty_Void());
        }
        
        case A_whileExp: {
            expty test = transExp(venv, tenv, a->u.whilee.test);
            expty body = transExp(venv, tenv, a->u.whilee.body);
            
            // type check test for int
            if(test.ty->kind != Ty_int)
                EM_error(a->u.whilee.test->pos, "Test for while statement must evaluate to integer");
                
            // type check body for void
            if(body.ty->kind != Ty_void)
                EM_error(a->u.whilee.test->pos, "Body of while loop must produce no value");
            return expTy(NULL, Ty_Void());
        }
        
        case A_forExp: {
            // eval lo and hi
            expty lo = transExp(venv, tenv, a->u.forr.lo);
            expty hi = transExp(venv, tenv, a->u.forr.hi);
            expty body;
            
            if(lo.ty->kind != Ty_int)
                EM_error(a->u.forr.lo->pos, "Integer Required");
            if(hi.ty->kind != Ty_int)
                EM_error(a->u.forr.hi->pos, "Integer Required");
            // new scope for for loop
            S_beginScope(venv);
            S_beginScope(tenv);
            
            // assign new symbol
            // TODO: check that this is right, make sure this position is OK
            transDec(venv, tenv, A_VarDec(a->pos, a->u.forr.var, NULL, a->u.forr.lo));
            
            body = transExp(venv, tenv, a->u.forr.body);
            if(body.ty->kind != Ty_void)
                EM_error(a->u.forr.body->pos, "body must produce no value");
            
            S_endScope(tenv);
            S_endScope(venv);
            // exit for loop scope
            return expTy(NULL, Ty_Void());
        }
        case A_breakExp:
            // TODO: what do we do with breaks?
            return expTy(NULL, Ty_Void());
            break;
            
        case A_letExp: {
            A_decList d;
            expty exp;
            
            S_beginScope(venv);
            S_beginScope(tenv);
            for(d = a->u.let.decs; d; d=d->tail)
                transDec(venv, tenv, d->head);
            exp = transExp(venv, tenv, a->u.let.body);     // type of sequence is that of the last expression
            S_endScope(tenv);
            S_endScope(venv);
            return exp;
        }
        
        case A_arrayExp:
            break;
        default:
            printf("what IS this?\n");
    }
}

void    transDec(S_table venv, S_table tenv, A_dec d)
{
    switch(d->kind) {
        case A_varDec: {
            expty e = transExp(venv, tenv, d->u.var.init);
            S_enter(venv, d->u.var.var, E_VarEntry(e.ty));
        }
        case A_typeDec:
        case A_functionDec:
            break;
        default:
            printf("Thats not a correct kind of declaration");
    }
}

Ty_ty   transTy (              S_table tenv, A_ty a)
{
    return Ty_Nil();
}

void SEM_transProg(A_exp exp)
{
    expty tree;
    
    transExp(S_empty(), S_empty(), exp);
    printf("I did it?\n");
}


