/* 
 * TODO for AS6
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "temp.h"
#include "tree.h"
#include "assem.h"
#include "frame.h"
#include "mipsframe.h"
#include "errormsg.h"
#include "codegen.h"

static void emit(AS_instr instr);
static Temp_temp munchExp(T_exp expr);
static void munchStm(T_stm stm);

Temp_tempList L(Temp_temp h, Temp_tempList t) {
	return Temp_tempList(h,t);
}

static AS_instrList instrList = NULL, last = NULL;

static void emit(AS_instr instr)
{
	if (!instrList) instrList = last = AS_InstrList(instr, NULL);
	else last = last->tail = AS_InstrList(instr, NULL);
}

AS_instrList F_codegen(F_frame frame, T_stmList stmList)
{
	AS_instrList asList = NULL;
	T_stmList sList = stmList;
	for (; sList; sList = sList->tail)
		munchStm(sList->head);
	asList = instrList;
	instrList = last = NULL;
	return asList;
}

static Temp_temp munchExp(T_exp e){
	switch(e){
		case MEM(BINOP(PLUS, e1, CONST(i))): {
			Temp_temp r = Temp_newtemp();
			emit(AS_Oper("LOAD 'd0 <- M['s0+" + i + "]\n",
					L(r, NULL), L(munchExp(e1), NULL), NULL));
			return r;
		}

		case MEM(BINOP(PLUS, CONST(i), e1)): {
			Temp_temp r = Temp_newtemp();
			emit(AS_Oper("LOAD 'd0 <- M['s0+" + i + "]\n",
					L(r, NULL), L(munchExp(e1), NULL), NULL));
			return r;
		}

		case MEM(CONST(i)): {
			Temp_temp r = Temp_newtemp();
			emit(AS_Oper("LOAD 'd0 <- M[r0+" + i + "]\n",
					L(r, NULL), NULL), NULL));
			return r;
		}

		case MEM(e1): {
			Temp_temp r = Temp_newtemp();
			emit(AS_Oper("LOAD 'd0 <- M['s0+0]\n",
					L(r, NULL), L(munchExp(e1), NULL), NULL));
			return r;
		}

		case BINOP(PLUS, e1, CONST(i)): {
			Temp_temp r = Temp_newtemp();
			emit(AS_Oper("ADDI 'd0 <- 's0+" + i + "\n",
					L(r, NULL), L(munchExp(e1), NULL), NULL));
			return r;
		}

		case BINOP(PLUS, CONST(i), e1): {
			Temp_temp r = Temp_newtemp();
			emit(AS_Oper("ADDI 'd0 <- 's0+" + i + "\n",
					L(r, NULL), L(munchExp(e1), NULL), NULL));
			return r;
		}

		case CONST(i): {
			Temp_temp r = Temp_newtemp();
			emit(AS_Oper("ADDI 'd0 <- r0+" + i + "\n",
					NULL, L(munchExp(e1), NULL), NULL));
			return r;
		}

		case BINOP(PLUS, e1, e2): {
			Temp_temp r = Temp_newtemp();
			emit(AS_Oper("ADD 'd0 <- 's0+'s1\n",
					L(r, NULL), L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
			return r;
		}

		case TEMP(t):
			return t;

	}
}

static void munchStm(T_stm s) {
	switch (s->kind)
		case T_MOVE:{
			T_exp dst = s->u.MOVE.dst, src = s->u.MOVE.src;
			if(dst->kind == T_MEM)
				if(dst->u.MEM->kind == T_BINOP
				&& dst->u.MEM->u.BINOP.op == T_plus
				&& dst->u.MEM->u.BINOP.right->kind == T_CONST){
					T_exp e1 = dst->u.MEM->u.BINOP.left, e2 = src;
					munchExp(e1); munchExp(e2); emit("STORE");
				}
				else if (dst->u.MEM->kind == T_BINOP
				      && dst->u.MEM->u.BINOP.op == T_plus
				      && dst->u.MEM->u.BINOP.left->kind  == T_CONST) {
				      	munchExp(e1); munchExp(e2); emit("STORE");
				      }
				else if (src->kind == T_MEM){
					T_exp e1 = dst->u.MEM,	e2 = src->u.MEM;
					munchExp(e1); munchExp(e2); emit("MOVEM");
				}
				else{
					T_exp e1 = dst->u.MEM, e2 = src;
					munchExp(e1); munchExp(e2); emit("STORE");
				}
			else if(dst->kind == T_TEMP){
				T_exp e2 = src;
				munchExp(e2); emit("ADD");
			}
			else assert(0);
		}
		case T_JUMP:{
			
		}
		case T_CJUMP:{
			
		}
		case T_NAME:{
			
		}
	
		
}
