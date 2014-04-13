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
T_exp e1, e2;

Temp_tempList L(Temp_temp h, Temp_tempList t) {
	return Temp_TempList(h,t);
}

static AS_instrList instrList = NULL, last = NULL;

static void emit(AS_instr instr)
{
    printf("%s", instr->u.OPER.assem); // DEBUG
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

static Temp_tempList munchArgs(int index, T_expList args){
    Temp_tempList retList;
    if(args == NULL) return NULL;
    
    retList = Temp_TempList(munchExp(args->head), munchArgs(index+1, args->tail));
    return retList;
}

static Temp_temp munchExp(T_exp e){
	switch(e->kind){
        /*
		case MEM(BINOP(PLUS, e1, CONST(i))): {
			Temp_temp r = Temp_newtemp();
			emit(AS_Oper("LOAD ~d0 <- M[~s0+" + i + "]\n",
					L(r, NULL), L(munchExp(e1), NULL), NULL));
			return r;
		}

		case MEM(BINOP(PLUS, CONST(i), e1)): {
			Temp_temp r = Temp_newtemp();
			emit(AS_Oper("LOAD ~d0 <- M[~s0+" + i + "]\n",
					L(r, NULL), L(munchExp(e1), NULL), NULL));
			return r;
		}
        */
        case T_MEM:{
            T_exp mem = e->u.MEM;
            printf("MEM EXP\n"); // DEBUG
            if(mem->kind == T_BINOP){
                string temp = malloc(100);
                Temp_temp r = Temp_newtemp();
                if(mem->u.BINOP.left->kind == T_CONST){
                    string temp = malloc(100);
                    sprintf(temp, "lw ~d0 %d(~s0)\n", mem->u.BINOP.left->u.CONST);
                    emit(AS_Oper(temp, L(r, NULL), L(munchExp(mem->u.BINOP.right), NULL), NULL));
                    return r;
                }
                else if(mem->u.BINOP.right->kind == T_CONST){
                    string temp = malloc(100);
                    sprintf(temp, "lw ~d0 %d(~s0)\n", mem->u.BINOP.right->u.CONST);
                    emit(AS_Oper(temp, L(r, NULL), L(munchExp(mem->u.BINOP.left), NULL), NULL));
                    return r;
                }
            }
            if(mem->kind == T_CONST){ // add some maximal checks
                /* MEM(CONST(i) */
                Temp_temp r = Temp_newtemp();
                string temp = malloc(100);
                sprintf(temp, "lw ~d0 <- M[r0+%d]\n", mem->u.CONST); // TODO: determine MIPS conversion
                emit(AS_Oper(temp, L(r, NULL), NULL, NULL));
                return r;
            }
            else {
                /* MEM(e1) */
                Temp_temp r = Temp_newtemp();
                emit(AS_Oper("lw ~d0, ~s0\n",
                                L(r, NULL), L(munchExp(mem), NULL), NULL));
                return r;
            }
        }
        case T_BINOP:{  // TODO create binary short cuts for const args
            Temp_temp r = Temp_newtemp();
            T_exp e1 = e->u.BINOP.left, e2 = e->u.BINOP.right; 
            printf("BINOP EXP\n"); // DEBUG
            
            switch(e->u.BINOP.op){
                case T_plus:{
                    if(e1->kind == T_CONST){
                        string temp = malloc(100);
                        sprintf(temp, "addi ~d0,~s0, %d\n", e1->u.CONST);
                        emit(AS_Oper(temp, L(r, NULL), L(munchExp(e2), NULL), NULL));
                        return r;
                    }
                    else if (e2->kind == T_CONST){
                        string temp = malloc(100);
                        sprintf(temp, "addi ~d0, ~s0, %d\n", e2->u.CONST);
                        emit(AS_Oper(temp, L(r, NULL), L(munchExp(e1), NULL), NULL));
                        return r;
                    }
                    else{
                        emit(AS_Oper("add ~d0, ~s0, ~s1\n",
                            L(r, NULL), L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
                        return r;
                    }
                }
                case T_minus:{
                    emit(AS_Oper("sub ~d0, ~s0, ~s1\n",
                            L(r, NULL), L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
                    return r;
                }
                case T_mul:{
                    emit(AS_Oper("mul ~d0, ~s0, ~s1\n",
                            L(r, NULL), L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
                    return r;
                }
                case T_div:{
                    emit(AS_Oper("div ~d0, ~s0, ~s1\n",
                            L(r, NULL), L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
                    return r;
                }
                case T_and:{
                    if(e1->kind == T_CONST){
                        string temp = malloc(100);
                        sprintf(temp, "andi ~d0, ~s0, %d\n", e1->u.CONST);
                        emit(AS_Oper(temp, L(r, NULL), L(munchExp(e2), NULL), NULL));
                        return r;
                    }
                    else if (e2->kind == T_CONST){
                        string temp = malloc(100);
                        sprintf(temp, "andi ~d0, ~s0, %d\n", e2->u.CONST);
                        emit(AS_Oper(temp, L(r, NULL), L(munchExp(e1), NULL), NULL));
                        return r;
                    }
                    else {
                        emit(AS_Oper("and ~d0, ~s0, ~s1\n",
                                L(r, NULL), L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
                        return r;
                    }
                }
                case T_or:{
                    if(e1->kind == T_CONST){
                        string temp = malloc(100);
                        sprintf(temp, "ori ~d0, ~s0, %d\n", e1->u.CONST);
                        emit(AS_Oper(temp, L(r, NULL), L(munchExp(e2), NULL), NULL));
                        return r;
                    }
                    else if (e2->kind == T_CONST){
                        string temp = malloc(100);
                        sprintf(temp, "ori ~d0, ~s0, %d\n", e2->u.CONST);
                        emit(AS_Oper(temp, L(r, NULL), L(munchExp(e1), NULL), NULL));
                        return r;
                    }
                    else {
                        emit(AS_Oper("or ~d0, ~s0, ~s1\n",
                                L(r, NULL), L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
                        return r;
                    }
                }
                case T_lshift:{
                    emit(AS_Oper("sll ~d0, ~s0, ~s1\n",
                            L(r, NULL), L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
                    return r;
                }
                case T_rshift:{
                    emit(AS_Oper("srl ~d0, ~s0, ~s1\n",
                            L(r, NULL), L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
                    return r;
                }
                case T_arshift:{
                    emit(AS_Oper("sra ~d0, ~s0, ~s1\n",
                            L(r, NULL), L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
                    return r;
                }
                case T_xor:{
                    if(e1->kind == T_CONST){
                        string temp = malloc(100);
                        sprintf(temp, "xori ~d0, ~s0, %d\n", e1->u.CONST);
                        emit(AS_Oper(temp, L(r, NULL), L(munchExp(e2), NULL), NULL));
                        return r;
                    }
                    else if (e2->kind == T_CONST){
                        string temp = malloc(100);
                        sprintf(temp, "xori ~d0, ~s0, %d\n", e2->u.CONST);
                        emit(AS_Oper(temp, L(r, NULL), L(munchExp(e1), NULL), NULL));
                        return r;
                    }
                    else{
                        emit(AS_Oper("xor ~d0, ~s0, ~s1\n",
                                L(r, NULL), L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
                        return r;
                    }
                }
                default:
                    assert(0);
            }
        }
        case T_TEMP:{
            printf("TEMP EXP\n"); // DEBUG
            return e->u.TEMP;
        }
        case T_ESEQ:{
            Temp_temp r = Temp_newtemp();
            printf("ESEQ EXP\n"); // DEBUG
            munchStm(e->u.ESEQ.stm);
            r = munchExp(e->u.ESEQ.exp);
            return r;
        }
        case T_NAME:{
            Temp_temp r = Temp_newtemp();
            printf("NAME EXP\n"); // DEBUG
            //emit(AS_Label("label", e->u.NAME));    // TODO: determine if this is correct handling
            return r;
        }
        case T_CONST:{
            Temp_temp r = Temp_newtemp();
            printf("CONST EXP\n"); // DEBUG
            string temp = malloc(100);
            sprintf(temp, "li ~d0, %d\n", e->u.CONST);
            emit(AS_Oper(temp, L(r, NULL), NULL, NULL));
            return r;
        }
        case T_CALL:{
            /* CALL(e, args) */
            Temp_temp r = munchExp(e->u.CALL.fun);
            Temp_tempList l = munchArgs(0, e->u.CALL.args);
            printf("CALL EXP\n"); // DEBUG
            r = munchExp(e->u.CALL.fun);
            emit(AS_Oper("jal ~s0\n", NULL, L(r, l), NULL));  //TODO calldefs, pg 212
            return r; //TODO correct return?
        }
        default:
            printf("ERROR\n");
	}
    assert(0);
}

static void munchStm(T_stm s) {
	switch (s->kind){
		case T_MOVE:{
			T_exp dst = s->u.MOVE.dst, src = s->u.MOVE.src;
            printf("MOVE STMT\n"); // DEBUG
			if(dst->kind == T_MEM){
				if(dst->u.MEM->kind == T_BINOP
				&& dst->u.MEM->u.BINOP.op == T_plus
				&& dst->u.MEM->u.BINOP.right->kind == T_CONST){
					T_exp e1 = dst->u.MEM->u.BINOP.left, e2 = src;
                    string temp = malloc(100);
                    sprintf(temp, "sw ~s1, %d(~s0)\n", e2->u.CONST);
					emit(AS_Oper(temp, NULL, L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
				}
				else if (dst->u.MEM->kind == T_BINOP
				      && dst->u.MEM->u.BINOP.op == T_plus
				      && dst->u.MEM->u.BINOP.left->kind  == T_CONST) {
                        string temp = malloc(100);
                        sprintf(temp, "sw ~s1, %d(~s0)\n", e1->u.CONST);
				      	emit(AS_Oper(temp, NULL, L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
				      }
				else if (src->kind == T_MEM){
					T_exp e1 = dst->u.MEM,	e2 = src->u.MEM;
					string temp = malloc(100);
                        sprintf(temp, "move ~s0, ~s1\n");
					emit(AS_Oper(temp, NULL, L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
				}
				else{
					T_exp e1 = dst->u.MEM, e2 = src;
					string temp = malloc(100);
					sprintf(temp, "sw ~s0, ~s1\n");
					emit(AS_Oper(temp, NULL, L(munchExp(e1), L(munchExp(e2), NULL)), NULL));
				}
            }
			else if(dst->kind == T_TEMP){
				T_exp e2 = src;
				string temp = malloc(100);
				sprintf(temp, "move ~d0, ~s0\n");
                printf("%d\n", e2->kind);
                printf("HERE\n"); //DEBUG
				emit(AS_Oper(temp, L(e2->u.TEMP,NULL), L(munchExp(e2), NULL), NULL));
                //printf("THERE\n");
			}
			else assert(0);
            printf("X MOVE STMT\n"); //DEBUG
			break;
		}
		case T_LABEL:{
			Temp_label lab = s->u.LABEL;
            printf("LABEL STMT\n"); // DEBUG
			string temp = malloc(100);
            sprintf(temp, "%s:\n", Temp_labelstring(lab));
			emit(AS_Label(temp, lab));
			break;
		}
		case T_SEQ:{
			T_stm left = s->u.SEQ.left, right = s->u.SEQ.right;
            printf("SEQ STMT\n"); // DEBUG
			munchStm(left);
			munchStm(right);
			break;
		}
		case T_JUMP:{
			Temp_temp r = Temp_newtemp();
			T_exp e = s->u.JUMP.exp;
			Temp_labelList jumps = s->u.JUMP.jumps;
			string temp = malloc(100);
			sprintf(temp, "jal ~j0\n");
			emit(AS_Oper(temp, L(r, NULL), L(r, NULL), AS_Targets(jumps)));
            printf("JUMP STMT\n"); // DEBUG
			break;
		}
		case T_CJUMP:{
            printf("CJUMP STMT\n"); // DEBUG
            Temp_temp r = Temp_newtemp();
			T_exp left = s->u.CJUMP.left;
			T_exp right = s->u.CJUMP.right;
			string temp = malloc(100);
			
			switch(s->u.CJUMP.op){
				case T_eq:
					sprintf(temp, "beq ~s0, ~s1, label\n");
					break;
				case T_ne:
					sprintf(temp, "bne ~s0, ~s1, label\n");
					break;
				case T_lt:
					sprintf(temp, "blt ~s0, ~s1, label\n");
					break;
				case T_le:
					sprintf(temp, "ble ~s0, ~s1, label\n");
					break;
				case T_gt:
					sprintf(temp, "bgt ~s0, ~s1, label\n");
					break;
				case T_ge:
					sprintf(temp, "bge ~s0, ~s1, label\n");
					break;
				case T_ult:
					sprintf(temp, "blt ~s0, ~s1, label\n");
					break;
				case T_ule:
					sprintf(temp, "ble ~s0, ~s1, label\n");
					break;
				case T_ugt:
					sprintf(temp, "bgt ~s0, ~s1, label\n");
					break;
				case T_uge:
					sprintf(temp, "bge ~s0, ~s1, label\n");
					break;
			}
			emit(AS_Oper(temp, NULL, L(munchExp(left), L(munchExp(right), NULL)), NULL));
			
			break;	
		}
	
		case T_EXP:{
			munchExp(s->u.EXP);
            printf("EXP STMT\n"); // DEBUG
			break;
		}
	}		
}
