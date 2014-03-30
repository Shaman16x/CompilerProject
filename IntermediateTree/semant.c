#include "semant.h"

// implementation based off of suggestions in the book


// ****************************************************
E_enventry E_VarEntry (Tr_access access, Ty_ty ty)
{
   E_enventry e = checked_malloc(sizeof(*e));
    e->kind = E_varEntry;
    e->u.var.ty = ty;
    e->u.var.access = access;
    return e;
}
E_enventry E_FunEntry (Tr_level level, Temp_label label, Ty_tyList formals, Ty_ty result)
{
    E_enventry e = checked_malloc(sizeof(*e));
    e->kind = E_funEntry;
    e->u.fun.level = level;
    e->u.fun.label = label;
    e->u.fun.result = result;
    e->u.fun.formals = formals;
    return e;
}

S_table E_base_tenv(void)
{
    S_table r = S_empty();
    
    S_enter(r, S_Symbol("int"), Ty_Int());
    S_enter(r, S_Symbol("string"), Ty_String());
    
    return r;
}

S_table E_base_venv(void)
{
    S_table r = S_empty();
    
    S_enter(r, S_Symbol("print"), E_FunEntry(NULL, NULL, Ty_TyList(Ty_String(), NULL), Ty_Void()));
    S_enter(r, S_Symbol("getchar"), E_FunEntry(NULL, NULL, NULL, Ty_String()));
    S_enter(r, S_Symbol("ord"), E_FunEntry(NULL, NULL, Ty_TyList(Ty_String(), NULL), Ty_Int()));
    S_enter(r, S_Symbol("chr"), E_FunEntry(NULL, NULL, Ty_TyList(Ty_Int(), NULL), Ty_String()));
    
    return r;
}

// ****************************************************

int compareTypes(Ty_ty a, Ty_ty b)
{
    Ty_ty x = actual_ty(a), y = actual_ty(b);
    
    if(x->kind == Ty_record) {
        if(y->kind == Ty_record){
            return x == y;
        }
        else
            return 0;
    }
    else 
        return x == y;
}

expty expTy(Tr_exp exp, Ty_ty ty){
    expty e; e.exp=exp; e.ty=ty; return e;
}

// finds the base type of Name types
Ty_ty actual_ty(Ty_ty t)
{
    Ty_ty temp = t;
    
    while(temp != NULL && temp->kind == Ty_name)
    {
        //temp = S_look(
        Ty_print(temp);
        printf("\n");
        temp = temp->u.name.ty;
    }
    
    return temp;
}


// finds the base type of Name types
Ty_ty actual_ty_v2(S_table tenv, Ty_ty t)
{
    Ty_ty temp = t;
    
    printf("actual_ty\n");
    while(temp != NULL && temp->kind == Ty_name)
    {
        Ty_print(temp);
        printf("\n");
        if(temp->u.name.ty == NULL){
            printf("this is null");
            temp = S_look(tenv, temp->u.name.sym);
        }
        else
            temp = temp->u.name.ty;
    }
    
    return temp;
}
//********************************************************************
// Trans Var
//********************************************************************

expty   transVar(S_table venv, S_table tenv, A_var v, Tr_level level)
{
    printf("transVar\n"); // DEBUG
    switch(v->kind){
        case A_simpleVar:{
            E_enventry x = S_look(venv, v->u.simple);
            if (x && x->kind == E_varEntry){
                return expTy(Tr_simpleVar(x->u.var.access, level), actual_ty(x->u.var.ty));    // TODO: get the "actual type" as opposed to a Name type
            }
            else {
                EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
                return expTy(NULL, Ty_Int());   // default type
            }
        }
        
        // handle record accesses
        case A_fieldVar: {
            expty rec = transVar(venv, tenv, v->u.field.var, level);
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
            expty arr = transVar(venv, tenv, v->u.subscript.var, level);
            expty e =   transExp(venv, tenv, v->u.subscript.exp, level);
            
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

//********************************************************************
// Trans Exp
//********************************************************************

expty   transExp(S_table venv, S_table tenv, A_exp a, Tr_level level) {
    printf("transExp: %d\n", a->kind); // DEBUG
    switch (a->kind) {
        case A_varExp:
            // translate variable
            return transVar(venv, tenv, a->u.var, level);
        case A_nilExp:
            // not sure about this one
            return expTy(Tr_int(0), Ty_Nil());  // 0 is nil?
            break;
        case A_intExp:
            return expTy(Tr_int(a->u.intt), Ty_Int());
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
                    arg = transExp(venv, tenv, arg_list->head, level);
                    if (actual_ty(arg.ty) != actual_ty(formals->head)) {
                        EM_error(arg_list->head->pos, "mismatch of types");
                    }
                }
                // when the for loop exits, we're at the end of at least 
                // one of the lists: args or formals. if one is not null,
                // then there is an incorrect number of arguments
                if((arg_list == NULL && formals != NULL) || 
                    (formals == NULL && arg_list != NULL)){
                    EM_error(a->pos, "incorrect number of arguments");
                }
                return expTy(NULL, e->u.fun.result);
                // check the return type
            }
            else {
                EM_error(a->pos, "Function is undefined");
                return expTy(NULL, Ty_Void());
            }
        break;
        } 
            break;
        case A_opExp: {
            A_oper oper = a->u.op.oper;
            expty left = transExp(venv, tenv, a->u.op.left, level);
            expty right = transExp(venv, tenv, a->u.op.right, level);
            switch (oper){
                case A_plusOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(Tr_oper(oper, left.exp, right.exp), Ty_Int());
                case A_minusOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(Tr_oper(oper, left.exp, right.exp), Ty_Int());
                case A_timesOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(Tr_oper(oper, left.exp, right.exp), Ty_Int());
                case A_divideOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(Tr_oper(oper, left.exp, right.exp), Ty_Int());
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
                    case Ty_nil:
                        if(right.ty->kind != Ty_record && right.ty->kind != Ty_nil)
                            EM_error(a->u.op.right->pos, "record required");
                        break;
                    default:
                        EM_error(a->u.op.left->pos, "Not a comparable type");
                    }
                    return expTy(Tr_oper(oper, left.exp, right.exp), Ty_Int());
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
                    case Ty_nil:
                        if(right.ty->kind != Ty_record && right.ty->kind != Ty_nil)
                            EM_error(a->u.op.right->pos, "record required");
                        break;
                    default:
                        EM_error(a->u.op.left->pos, "Not a comparable type");
                    }
                    return expTy(Tr_oper(oper, left.exp, right.exp), Ty_Int());
                case A_ltOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(Tr_oper(oper, left.exp, right.exp), Ty_Int());
                case A_leOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(Tr_oper(oper, left.exp, right.exp), Ty_Int());
                case A_gtOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(Tr_oper(oper, left.exp, right.exp), Ty_Int());
                case A_geOp:
                    if (left.ty->kind != Ty_int)
                        EM_error(a->u.op.left->pos, "Integer required");
                    if (right.ty->kind != Ty_int)
                        EM_error(a->u.op.right->pos, "Integer required");
                    return expTy(Tr_oper(oper, left.exp, right.exp), Ty_Int());
                default:
                    printf("this is an unhandled operation!\n");
                    break;
            }
            // requires a lot of work
        }
        case A_recordExp: {
            expty exp;  // TODO: type checking
            A_efieldList ef;
            Ty_fieldList f = NULL, g = NULL;
            Ty_ty recType = S_look(tenv,  a->u.record.typ);
            /*
            // convert efields into Ty_Fields
            for(ef=a->u.record.fields; ef; ef=ef->tail){
                exp = transExp(venv, tenv, ef->head->exp);
                f = Ty_FieldList(Ty_Field(ef->head->name, exp.ty), f);
            }
            for(f; f; f=f->tail)    // put list in order
                g = Ty_FieldList(f->head, g);

            recType = Ty_Record(f);
            */
            if(recType == NULL){
                EM_error(a->pos, "Record Type is unkown");
                return expTy(NULL, Ty_Void());  // placeholder
            }

            return expTy(NULL, S_look(tenv,  a->u.record.typ));
        }
            
        case A_seqExp: {
            A_expList e;
            expty exp;
            
            exp = expTy(NULL, Ty_Void());           // initialized to value of an empty sequence
            for(e = a->u.seq; e; e=e->tail)
                exp = transExp(venv, tenv, e->head, level);

            return exp;
        }
        case A_assignExp: {
            expty var = transVar(venv, tenv, a->u.assign.var, level);
            expty exp = transExp(venv, tenv, a->u.assign.exp, level);
            
            if (var.ty->kind == Ty_record && exp.ty->kind == Ty_nil){ // record was assignment nil case
            }
            else if (var.ty->kind != exp.ty->kind)
                EM_error(a->u.assign.exp->pos, "Mismatch of type in assignment");
                
            return expTy(Tr_assign(var.exp, exp.exp), Ty_Void());
        }
        case A_ifExp:{
            expty test = transExp(venv, tenv, a->u.iff.test, level);
            expty then = transExp(venv, tenv, a->u.iff.then, level);
            expty elsee;
            
            printf("Doing the if statement\n"); // DEBUG
            
            // check E for type int
            if(test.ty->kind != Ty_int)
                EM_error(a->u.iff.test->pos, "Test for if statement must evaluate to integer");
                
            // check that else and then match types
            if(a->u.iff.elsee != NULL) {
                elsee = transExp(venv, tenv, a->u.iff.elsee, level);
                if(then.ty->kind != elsee.ty->kind)
                    if(then.ty->kind != Ty_record && elsee.ty != Ty_Nil())
                        EM_error(a->u.iff.elsee->pos, "Else type must match Then type");
                return expTy(Tr_if(test.exp, then.exp, elsee.exp), then.ty);    // return then's type
            }
            else if (then.ty != Ty_Void()){
                EM_error(a->u.iff.then->pos, "Then statement must not return a value");
            }
            return expTy(Tr_if(test.exp, then.exp, NULL), Ty_Void());
        }
        
        case A_whileExp: {
            expty test = transExp(venv, tenv, a->u.whilee.test, level);
            expty body = transExp(venv, tenv, a->u.whilee.body, level);
            
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
            expty lo = transExp(venv, tenv, a->u.forr.lo, level);
            expty hi = transExp(venv, tenv, a->u.forr.hi, level);
            expty body;
            
            if(lo.ty->kind != Ty_int)
                EM_error(a->u.forr.lo->pos, "Low end of range must be an integer");
            if(hi.ty->kind != Ty_int)
                EM_error(a->u.forr.hi->pos, "High end of range must be an integer");
            // new scope for for loop
            S_beginScope(venv);
            S_beginScope(tenv);
            
            // assign new symbol
            // TODO: check that this is right, make sure this position is OK
            transDec(venv, tenv, A_VarDec(a->pos, a->u.forr.var, NULL, a->u.forr.lo), level);
            
            body = transExp(venv, tenv, a->u.forr.body, level);
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
            
        case A_letExp: {    // copied from the book's implementation
            A_decList d;
            expty exp;
            
            S_beginScope(venv);
            S_beginScope(tenv);
            for(d = a->u.let.decs; d; d=d->tail)
                transDec(venv, tenv, d->head, level);
            exp = transExp(venv, tenv, a->u.let.body, level);
            S_endScope(tenv);
            S_endScope(venv);
            return exp;
        }
        
        case A_arrayExp:{
            
            Ty_ty t = S_look(tenv, a->u.array.typ);
            expty size = transExp(venv, tenv, a->u.array.size, level);
            expty init = transExp(venv, tenv, a->u.array.init, level);

            if (!t) {
                EM_error(a->pos, "Array Type is undeclared");
                return expTy(NULL, Ty_Array(Ty_Int())); // placeholder for nonexistent types
            }
            else if (t->kind != Ty_array)
                EM_error(a->pos, "Type is not an array");
            else if (t->u.array != init.ty)
                EM_error(a->u.array.size->pos, "mismatch of array type and init type"); // TODO: fix these
            else if (size.ty->kind != Ty_int)
                EM_error(a->u.array.size->pos, "size must be an integer");
                
            return expTy(NULL, t);
            }
        default:
            //printf("what IS this?\n");      // seems that there are hidden expessions?
            return expTy(NULL, Ty_Void());
    }
}

//********************************************************************
// Trans Dec
//********************************************************************

void    transDec(S_table venv, S_table tenv, A_dec d, Tr_level level)
{
    printf("transDec\n"); // DEBUG
    switch(d->kind) {
        case A_varDec: {    // TODO: check type and nil assignments
            expty e = transExp(venv, tenv, d->u.var.init, level);
            Ty_ty type;
            // check that type and initial value match
            if(d->u.var.typ != NULL){
                type = S_look(tenv, d->u.var.typ);      // look for type's existence
                if (type == NULL) {
                    EM_error(d->pos, "Variable Type is undeclared, defaulting to int");
                    type = Ty_Int();
                }
                else if (type->kind == Ty_array) {  // handle array types
                    // TODO: proper array type checking
                    if(actual_ty(type) != actual_ty(e.ty))
                        EM_error(d->u.var.init->pos, "Mismatch of array types");
                }
                else if (type->kind == Ty_record) {
                    Ty_fieldList f, g;
                    // TODO: something something type checking
                    if(e.ty->kind == Ty_nil){}
                    else if (actual_ty(type) != actual_ty(e.ty)) {
                        EM_error(d->u.var.init->pos, "Mismatch of Record types");
                    }
                    else if (e.ty->kind == Ty_record){  // check that the initialization is correct
                        printf("check the record\n");
                        for(f = type->u.record, g = e.ty->u.record; f && g; f=f->tail, g=g->tail){
                        }
                    }
                    else
                        EM_error(d->u.var.init->pos, "initial value is not a record or nil");
                }
                else if(type != e.ty)       // basic type check
                    EM_error(d->u.var.init->pos, "value does not match the variable's type");
                S_enter(venv, d->u.var.var, E_VarEntry(Tr_allocLocal(level, TRUE), type));
            }
            else
                if(e.ty->kind == Ty_nil){
                    EM_error(d->u.var.init->pos, "Cannot assign a non-record the value of nil");
                }
                S_enter(venv, d->u.var.var, E_VarEntry(Tr_allocLocal(level, TRUE),e.ty)); 
            break;
        }
            
        case A_typeDec: {       // TODO: need to "generalize" these, see book
            A_nametyList l;
            Ty_ty check;
            Ty_fieldList rec;
            Ty_field f;
            
            // declare the types as names first
            for(l = d->u.type; l; l=l->tail){
                S_enter(tenv, l->head->name, Ty_Name(l->head->name, NULL));   // for recursive definitions
            }
            
            // define types
            for(l = d->u.type; l; l=l->tail){
                check = S_look(tenv, l->head->name);
                if(actual_ty(check) != NULL){
                    EM_error(d->pos, "Redefinition of type %s in the same sequence", S_name(l->head->name));
                }
                
                S_enter(tenv, l->head->name, transTy(tenv, l->head->ty));

            }
            
            // check for cyclical definitions and fix record decs
            for(l = d->u.type; l; l=l->tail){
                check = S_look(tenv, l->head->name);   // for recursive definitions
                
                if(check->kind == Ty_record){
                    printf("Fixing record\n");  // DEBUG
                    rec = check->u.record;
                    for(rec; rec; rec=rec->tail){
                        if(rec->head->ty->kind == Ty_name){ // clear up name definitions
                            rec->head->ty->u.name.ty = S_look(tenv, rec->head->ty->u.name.sym);
                            if(actual_ty(rec->head->ty->u.name.ty) == NULL) {
                                EM_error(d->pos, "Type definition for %s is cyclical", S_name(l->head->name));
                            }
                        }
                    }
                }
                else if (actual_ty(S_look(tenv, l->head->name)) == NULL){
                    EM_error(d->pos, "Type definition for %s is cyclical", S_name(l->head->name));
                }
            }
            break;
        }
        case A_functionDec: {// TODO: type checking?
            A_fundecList f, g;
            A_fundec check;
            Ty_ty resultTy;
            expty body;
            U_boolList boolList = NULL;
            Temp_label label;
            printf("funcDec\n");
            // f->result == NULL mean no return value
            
            // added pointers to all functions
            for(f = d->u.function; f; f=f->tail){
                if(f->head->result==NULL)
                    resultTy = Ty_Void();
                else
                    resultTy = S_look(tenv, f->head->result);
                
                Ty_tyList formalTys = makeFormalTyList(tenv, f->head->params);
                Ty_tyList temp;
                // create escaping characters list for new level
                // always TRUE
                for(temp = formalTys; temp; temp=temp->tail)
                    boolList = U_BoolList(TRUE, boolList);
                label = Temp_newlabel();
                S_enter(venv, f->head->name, E_FunEntry(Tr_newLevel(level, label, boolList), label, formalTys, resultTy));
            }
            
            // check for function redefinitions
            for(f = d->u.function; f; f=f->tail){
                check = S_look(venv, f->head->name);
                for(g=f->tail; g; g=g->tail)
                    if(check == S_look(venv, g->head->name))
                        EM_error(d->pos, "Redefinition of function %s in same sequence", S_name(f->head->name));    
            }
            
            // translate function bodies
            for(f = d->u.function; f; f=f->tail){
                if(f->head->result==NULL)
                    resultTy = Ty_Void();
                else
                    resultTy = S_look(tenv, f->head->result);

                Ty_tyList formalTys = makeFormalTyList(tenv, f->head->params);

                S_beginScope(venv);
                {A_fieldList l; Ty_tyList t;
                    for(l=f->head->params, t=formalTys; l; l=l->tail, t=t->tail)
                        S_enter(venv, l->head->name, E_VarEntry(Tr_allocLocal(level, TRUE),t->head));
                }
                body = transExp(venv, tenv, f->head->body, level);
                if(actual_ty(body.ty) != actual_ty(resultTy))
                    EM_error(d->pos, "Function return type does not match the body's");
                S_endScope(venv);
            }
            break;
        }

        default:
            printf("Thats not a correct kind of declaration");
    }
}

Ty_tyList makeFormalTyList(S_table tenv, A_fieldList params){
    A_fieldList f;
    Ty_tyList t = NULL;
    Ty_tyList s = NULL;
    
    // get all the types, produces reversed list
    for(f=params; f; f=f->tail){
        t = Ty_TyList(S_look(tenv, f->head->typ), t);
    }
    
    // put the list in the proper order
    for(t; t; t=t->tail){
        s = Ty_TyList(t->head, s);
    }
    
    return s;
}

Ty_ty   transTy (              S_table tenv, A_ty a)
{
    Ty_ty t = NULL;
    printf("transTy\n"); // DEBUG
    switch(a->kind){
        case A_nameTy:
            t = S_look(tenv, a->u.name);
            break;
        case A_recordTy: {
            Ty_fieldList f = NULL;
            Ty_fieldList g = NULL;
            A_fieldList l;
            
            for(l = a->u.record; l; l=l->tail){  // create the field list
                if(S_look(tenv, l->head->typ) == NULL) {
                    EM_error(l->head->pos, "Undeclared Type, defaulting to Int");
                    f= Ty_FieldList(Ty_Field(l->head->name, Ty_Int()), f);
                }
                else {
                    f= Ty_FieldList(Ty_Field(l->head->name, S_look(tenv, l->head->typ)), f);
                }
            }
            for(f; f; f=f->tail)            // reorder list
                g = Ty_FieldList(f->head, g);
            t = Ty_Record(g); // TODO: is this the correct usage?
            break;
        }
        case A_arrayTy:
            printf("translating array\n");
            t = Ty_Array(S_look(tenv, a->u.array));

            break;
        default:
            printf("no kind?\n");
            break;
    }
    
    if (t == NULL){
        EM_error(a->pos, "Unknown type, defaulting to int");
        t = Ty_Int();
    }
    printf("Type is ");
    Ty_print(t);
    printf("\n");
    
    return t;
}

void SEM_transProg(A_exp exp)
{
    expty tree;
    
    transExp(E_base_venv(), E_base_tenv(), exp, Tr_outermost());
    
    printf("Finished Type Checking\n");
}


