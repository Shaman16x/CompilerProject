#include "translate.h"

/* translate source code */

typedef struct patchList_ *patchList;
struct patchList_ {Temp_label *head; patchList tail;};
static patchList PatchList(Temp_label *head, patchList tail)
{
    patchList p = malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

void doPatch(patchList tList, Temp_label label){
    for(; tList; tList=tList->tail){
            * (tList->head) = label;
        }
}

patchList joinPatch(patchList first, patchList second) {
    if(!first) return second;
    for(; first->tail; first=first->tail)   /* go to the end of list */
        first->tail = second;
    return first;
}

struct Cx {patchList trues; patchList falses; T_stm stm;};

struct Tr_exp_
    {enum {Tr_ex, Tr_nx, Tr_cx} kind;
        union {T_exp ex; T_stm nx; struct Cx cx;} u;
    };
    
// used to create ex, nx, and cx
static Tr_exp Tr_Ex(T_exp ex)
{
    Tr_exp r = malloc(sizeof(*r));
    r->kind = Tr_ex;
    r->u.ex = ex;
    return r;
}
static Tr_exp Tr_Nx(T_stm nx)
{
    Tr_exp r = malloc(sizeof(*r));
    r->kind = Tr_nx;
    r->u.nx = nx;
    return r;
}

static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm)
{
    Tr_exp r = malloc(sizeof(*r));
    r->kind = Tr_cx;
    r->u.cx.trues = trues;
    r->u.cx.falses = falses;
    r->u.cx.stm = stm;
    return r;
}

static T_exp unEx(Tr_exp e){
    switch(e->kind){
    case Tr_ex:
        return e->u.ex;
    case Tr_cx:{
        Temp_temp r = Temp_newtemp();
        Temp_label t = Temp_newlabel(), f = Temp_newlabel();
        doPatch(e->u.cx.trues, t);
        doPatch(e->u.cx.falses, f);
        
        return T_Eseq(T_Move(T_Temp(r), T_Const(1)),
                T_Eseq(e->u.cx.stm,
                 T_Eseq(T_Label(f),
                  T_Eseq(T_Move(T_Temp(r), T_Const(0)),
                   T_Eseq(T_Label(t),
                    T_Temp(r))))));
    }
    case Tr_nx:
        return T_Eseq(e->u.nx, T_Const(0));
    }
    assert(0);  /* should not get here */
}

static T_stm unNx(Tr_exp e){        // TODO: this
    switch(e->kind){
    case Tr_ex:
        return T_Exp(e->u.ex);
    case Tr_cx:{
        return e->u.cx.stm;// TODO: write the appropriate conversion
    }
    case Tr_nx:
        return e->u.nx;
    }
}


static struct Cx unCx(Tr_exp e){    // TODO: this
    switch(e->kind){
    case Tr_ex:{
        struct Cx c;
        c.stm = T_Cjump(T_ne, e->u.ex, T_Const(0), NULL, NULL); // e!=0 tests for nonzeros
        c.trues = PatchList(&c.stm->u.CJUMP.true, NULL);
        c.falses = PatchList(&c.stm->u.CJUMP.false, NULL);

        return c; 
    }
    case Tr_cx:
        return e->u.cx;
    case Tr_nx:
        break;
    }
}    

struct Tr_access_ {Tr_level level; F_access access;};

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail)
{
    Tr_accessList t = malloc(sizeof(*t));
    
    t->head = head;
    t->tail = tail;
    
    return t;
}

Tr_level Tr_outermost(void)
{
    Tr_level l = malloc(sizeof(*l));
    
    l->parent = NULL;
    l->name = NULL;
    l->frame = NULL;
    
    return l;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals)
{
    Tr_level l = malloc(sizeof(*l));
    l->parent = parent;
    l->name = name;
    l->frame = F_newFrame(l->name, formals);
    return l;
}

Tr_access Tr_allocLocal(Tr_level level, bool escape)
{
    Tr_access a = malloc(sizeof(*a));
    a->level = level;
    a->access = F_allocLocal(level->frame, escape);
    return a;
}

void printStm(T_stm stm)
{
    FILE *f = fopen("debug.txt", "a");
    T_stmList l = T_StmList(stm, NULL);
    printStmList(f, l);
    fclose(f);
}

// translate functions
Tr_exp Tr_int(int i)
{
    return Tr_Ex(T_Const(i));
}

Tr_exp Tr_simpleVar(Tr_access acc, Tr_level level)
{
    Tr_exp ret = NULL;  // TODO: turn this into a thing,pg 160
    ret = Tr_Ex(F_Exp(acc->access, T_Temp(F_FP())));
    // printStm(T_Exp(unEx(ret)));
    return ret;
}

Tr_exp Tr_oper(A_oper oper, Tr_exp left, Tr_exp right)
{
    Tr_exp ret = NULL;
    T_stm op;
    switch (oper){
        case A_plusOp:
            ret = Tr_Ex(T_Binop(T_plus, unEx(left), unEx(right)));
            break;
        case A_minusOp:
            ret = Tr_Ex(T_Binop(T_minus, unEx(left), unEx(right)));
            break;
        case A_timesOp:
            ret = Tr_Ex(T_Binop(T_mul, unEx(left), unEx(right)));
            break;
        case A_divideOp:
            ret = Tr_Ex(T_Binop(T_div, unEx(left), unEx(right)));
            break;
        case A_eqOp:{
            op = T_Cjump(T_eq, unEx(left), unEx(right), NULL, NULL);
            patchList trues = PatchList(&op->u.CJUMP.true, NULL);
            patchList falses = PatchList(&op->u.CJUMP.false, NULL);
            ret = Tr_Cx(trues, falses, op);
            break;
        }
        case A_neqOp:{
            op = T_Cjump(T_ne, unEx(left), unEx(right), NULL, NULL);
            patchList trues = PatchList(&op->u.CJUMP.true, NULL);
            patchList falses = PatchList(&op->u.CJUMP.false, NULL);
            ret = Tr_Cx(trues, falses, op);
            break;
        }
        case A_ltOp:{
            op = T_Cjump(T_lt, unEx(left), unEx(right), NULL, NULL);
            patchList trues = PatchList(&op->u.CJUMP.true, NULL);
            patchList falses = PatchList(&op->u.CJUMP.false, NULL);
            ret = Tr_Cx(trues, falses, op);
            break;
        }
        case A_leOp:{
            op = T_Cjump(T_le, unEx(left), unEx(right), NULL, NULL);
            patchList trues = PatchList(&op->u.CJUMP.true, NULL);
            patchList falses = PatchList(&op->u.CJUMP.false, NULL);
            ret = Tr_Cx(trues, falses, op);
            break;
        }
        case A_gtOp:{
            op = T_Cjump(T_gt, unEx(left), unEx(right), NULL, NULL);
            patchList trues = PatchList(&op->u.CJUMP.true, NULL);
            patchList falses = PatchList(&op->u.CJUMP.false, NULL);
            ret = Tr_Cx(trues, falses, op);
            break;
        }
        case A_geOp:{
            op = T_Cjump(T_ge, unEx(left), unEx(right), NULL, NULL);
            patchList trues = PatchList(&op->u.CJUMP.true, NULL);
            patchList falses = PatchList(&op->u.CJUMP.false, NULL);
            ret = Tr_Cx(trues, falses, op);
            break;
        }
        default:
            return NULL;
    }
    
    //printStm(T_Exp(unEx(ret)));
    
    return ret;
}

Tr_exp Tr_assign(Tr_exp var, Tr_exp exp)
{
    T_stm ret;
    
    ret = T_Move(T_Mem(unEx(var)), unEx(exp)); 
    //printStm(ret);
    return Tr_Nx(ret);
}

Tr_exp Tr_declist(Tr_exp head, Tr_exp tail){
    T_stm ret;
    ret = T_Seq(unNx(head), unNx(tail)); 
    //printStm(ret);
    return Tr_Nx(ret);
}

Tr_exp Tr_nop(void){
    return Tr_Ex(T_Const(0));
}

Tr_exp Tr_let(Tr_exp decs, Tr_exp body){
    T_stm ret;
    ret = T_Seq(unNx(decs), unNx(body));
    //printStm(ret);
    return Tr_Nx(ret);
}

Tr_exp Tr_if(Tr_exp test, Tr_exp then, Tr_exp elsee){
    T_stm ret = NULL;
    Temp_label tr = Temp_newlabel(), fl = Temp_newlabel(), end = Temp_newlabel();
    struct Cx c = unCx(test);
    
    doPatch(c.trues, tr);   // patch the test
    doPatch(c.falses, fl);
    
    if(elsee == NULL){  // only a then statment
        ret = T_Seq(c.stm,
                T_Seq(T_Label(tr),
                  T_Seq(T_Exp(unEx(then)),
                    T_Label(fl))));
        // printStm(ret);
        return Tr_Nx(ret);
    }
    else {  // both then and else, then statement jumps to end
        ret = T_Seq(c.stm,
                T_Seq(T_Label(tr),
                  T_Seq(T_Exp(unEx(then)),
                    T_Seq(T_Jump(T_Name(end), Temp_LabelList(end, NULL)),
                      T_Seq(T_Label(fl),
                        T_Seq(T_Exp(unEx(elsee)), T_Label(end)))))));
        //printStm(ret);  // DEBUG
        return Tr_Nx(ret);
    }
}

//TODO: this, also break handling 169
Tr_exp Tr_while(Tr_exp test, Tr_exp body, Temp_label done){
    Temp_label head = Temp_newlabel(), loop = Temp_newlabel();
    T_stm stm;
    struct Cx c = unCx(test);
    doPatch(c.trues, loop);
    doPatch(c.falses, done);
    stm = T_Seq(T_Label(head),
           T_Seq(c.stm,
             T_Seq(T_Label(loop),
               T_Seq(T_Exp(unEx(body)),
                 T_Seq(T_Jump(T_Name(head), Temp_LabelList(head, NULL)),
                   T_Label(done))))));
    //printStm(stm);
    return Tr_Nx(stm);
}


Tr_exp Tr_for(void);


Tr_exp Tr_break(Temp_label done){
    //printStm(T_Jump(T_Name(done), Temp_LabelList(done, NULL))); 
    return Tr_Nx(T_Jump(T_Name(done), Temp_LabelList(done, NULL)));
}

