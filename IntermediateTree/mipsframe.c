#include "frame.h"
// #include "mipsframe.h"

/* mipsframe source code */

const int F_wordSize = 4;   // four bytes per word

struct F_access_
{
    enum {inFrame, inReg} kind;
    union {
        int offset;     /* InFrame */
        Temp_temp reg;  /* InReg */
    } u;
};

F_accessList F_AccessList(F_access head, F_accessList tail)
{
    F_accessList l = malloc(sizeof(*l));
    
    l->head = head;
    l->tail = tail;
    
    return l;
}

// mips frame allocation
static F_access InFrame(int offset)
{
    F_access a = malloc(sizeof(*a));
    
    a->kind = inFrame;
    a->u.offset = offset;
    
    return a;
}

// mips reg allocation
static F_access InReg(Temp_temp reg);

F_frame F_newFrame(Temp_label name, U_boolList formals){
    F_frame f = malloc(sizeof(*f));
    U_boolList l;
    bool b;
    int offset = 0;
    
    f->name = name;
    
    for(l = formals; l; l=l->tail){
        f->formals = F_AccessList(InFrame(offset), f->formals);
        offset += 4;
    }
    
    return f;
}

Temp_label F_name(F_frame frame) {return frame->name;}
F_accessList F_formals(F_frame frame) {return frame->formals;}

F_access F_allocLocal(F_frame frame, bool escape){
    if(escape)
        return InFrame(0);  // fix this
    else 
        return InFrame(0);  // only doing inreg allocs
}

Temp_temp F_FP(void){
    return Temp_newtemp();  // set the appropriate frame pointer
}

T_exp F_Exp(F_access acc, T_exp framePtr){
    return T_Mem(T_Binop(T_plus, framePtr, T_Const(acc->u.offset)));   // assuming only frame values
}



