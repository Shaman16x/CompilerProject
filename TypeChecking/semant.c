#include "semant.h"

// implementation based off of suggestions in the book

expty expTy(Tr_exp exp, Ty_ty ty){
    expty e; e.exp=exp; e.ty=ty; return e;
}


void SEM_transProg(A_exp exp)  {
    expty tree;
    
    tree = expTy(NULL, Ty_Nil());
    printf("I did it?\n");
    
}


