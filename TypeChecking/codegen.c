#include "codegen.h"
#include "temp.h"
#include "util.h"

static void emit(AS_instr instr);
static Temp_temp munchExp(T_exp expr);
static void munchStm(T_stm stm);

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

static void Temp_temp munchStm(T_stm s) {
	switch (s)
		case MOVE(MEM(BINOP(PLUS, e1, CONST(i))), e2):
			emit(AS_Oper("STORE M['s0+" + i + "] <- 's1\n",
					NULL, L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
	
		case MOVE(MEM(BINOP(PLUS, CONST(i), e1,)), e2):
			emit(AS_Oper("STORE M['s0+" + i + "] <- 's1\n",
					NULL, L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
	
		case MOVE(MEM(e1), MEM(e2)):
			emit(AS_Oper("MOVE M['s0] <- M['s1]\n",
					NULL, L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
	
		case MOVE(MEM(CONST(i)), e2):
			emit(AS_Oper("STORE M[r0+" + i + "] <- 's0\n",
					NULL, L(munchExp(e2), NULL), NULL));
	
		case MOVE(MEM(e1), e2):
			emit(AS_Oper("STORE M['s0] <- 's1\n",
					NULL, L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
					
		case MOVE(TEMP(i), e2):
			emit(AS_Oper("ADD 'd0 <- 's0 + r0\n",
					L(i,NULL), L(munchExp(e2), NULL), NULL));			
		
		case LABEL(lab):
			emit(AS_Label(Temp_labelstring(lab) + ":\n", lab));
}



