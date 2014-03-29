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
    for(;tList; tList=tList->tail)
        *(tList->head) = label;
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
    // TODO: this
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

static T_stm unNx(Tr_exp e);
static struct Cx unCx(Tr_exp e);
    
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



