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

static int nextAccessIndex(F_accessList l){
    F_accessList temp;
    int i = -F_wordSize;
    for(temp = l; temp; temp=temp->tail)
        i-=F_wordSize;
    return i;
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
    int offset = -(F_wordSize); // assumes base frame is reserved?
    
    f->name = name;
    
    // create accesses for all formals
    for(l = formals; l; l=l->tail){
        f->formals = F_AccessList(InFrame(offset), f->formals);
        offset -=F_wordSize;
    }
    
    return f;
}

Temp_label F_name(F_frame frame) {return frame->name;}
F_accessList F_formals(F_frame frame) {return frame->formals;}


F_access F_allocLocal(F_frame frame, bool escape){
    F_access a;
    if(frame && escape){
        a = InFrame(nextAccessIndex(frame->formals));
        frame->formals = F_AccessList(a, frame->formals);
        return a;  // TODO: verify
    }
    else
        return InFrame(0);  // only doing inreg allocs this should not hit
}

Temp_temp F_FP(void){
    return Temp_newtemp();  // TODO: set the appropriate frame pointer
}

T_exp F_Exp(F_access acc, T_exp framePtr){
    return T_Mem(T_Binop(T_plus, framePtr, T_Const(acc->u.offset)));   // assuming only frame values
}


F_frag F_StringFrag(Temp_label label, string str){
	
	F_frag s = checked_malloc(sizeof(*s));
	s->u.stringg.label = label;
	s->u.stringg.str = str;	
	return s;
}

F_frag F_ProcFrag(T_stm body, F_frame frame){
	F_frag s = checked_malloc(sizeof(*s));
	s->u.proc.body = body;
	s->u.proc.frame = frame;
	return s;
}

F_fragList F_FragList(F_frag head, F_fragList tail){
/*	F_fragList fl = checked_malloc(sizeof(*fl));
	fl->head = head;
	fl->tail = tail;
	return fl;*/
}


