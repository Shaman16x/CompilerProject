#include "translate.h"

/* translate source code */

struct Cx {patchList trues; patchList falses; T_stm stm;};

struct Tr_exp_
    {enum {Tr_ex, Tr_nx, Tr_cx} kind;
        union {T_exp ex; T_stm nx; struct Cx cx;} u;
    };
    
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




