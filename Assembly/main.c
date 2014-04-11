/*
 * main.c
 *
 * Modified by Ming Zhou
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "temp.h" /* needed by translate.h */
#include "tree.h" /* needed by frame.h */
#include "assem.h"
#include "frame.h" /* needed by translate.h and printfrags prototype */
#include "types.h"
#include "translate.h"
#include "semant.h" /* function prototype for transProg */
#include "canon.h"
#include "prabsyn.h"
#include "printtree.h"
//#include "escape.h" //NOT AVAILABLE
//#include "parse.h" //NOT AVAILABLE; Intead copy "A_exp parse(string fname)" from parsetest.c
#include "codegen.h"
#include "graph.h"
#include "flowgraph.h"

extern bool anyErrors;

extern int yyparse(void);
extern A_exp absyn_root;

/* parse source file fname; 
   return abstract syntax data structure */
A_exp parse(string fname)
{EM_reset(fname);
 if (yyparse() == 0) /* parsing worked */
   return absyn_root;
 else return NULL;
}

/* print the assembly language instructions to filename.s */
static void doProc(FILE *out, F_frame frame, T_stm body)
{
	static seq = 0;

	AS_proc proc;
//	struct RA_result allocation;
	T_stmList stmList;
	AS_instrList iList;

	fprintf(out, "\nProcedure %d (%s):\n", ++seq, Temp_labelstring(F_name(frame)));

	//printStmList(out, T_StmList(body, NULL));

	stmList = C_linearize(body);
	struct C_block block = C_basicBlocks(stmList);
	stmList = C_traceSchedule(block);

	printStmList(out, stmList);

/* Assignment 6 */
	iList = F_codegen(frame, stmList);
	//Temp_map map = NULL;//TODO Create a temp map here
	//fprintf(out, "BEGIN %s\n", Temp_labelstring(F_name(frame)));
	//AS_printInstrList (out, iList, map);
	//fprintf(out, "END %s\n", Temp_labelstring(F_name(frame)));

}

int main(int argc, string *argv)
{
	A_exp absyn_root = NULL;
//	S_table base_env, base_tenv;
	F_fragList frags = NULL;
    AS_instrList insts = NULL;
	char outfile[100];	
	FILE *out = stdout;

	if (argc==2) {
		absyn_root = parse(argv[1]);
		if (!absyn_root) {
			return 1;
		}

#if 0
   pr_exp(out, absyn_root, 0); /* print absyn data structure */
   fprintf(out, "\n");
#endif

//   Esc_findEscape(absyn_root); /* set varDec's escape field */	//NOT AVAILABLE

	frags = SEM_transProg(absyn_root);
    
	if (anyErrors) {
		// don't continue
		return 1; 
	}

   sprintf(outfile, "%s.s", argv[1]);
    out = fopen(outfile, "w");
    for (;frags;frags=frags->tail) {
        if (frags->head != NULL) {
            if (frags->head->kind == F_procFrag) {
                doProc(out, frags->head->u.proc.frame, frags->head->u.proc.body);
            } 
            else if (frags->head->kind == F_stringFrag) {
                fprintf(out, "%s\n", frags->head->u.stringg.str);
            }
        }
    }

    fclose(out);
    return 0;
	}

	EM_error(0, "usage: tiger file.tig");
	return 1;
}
