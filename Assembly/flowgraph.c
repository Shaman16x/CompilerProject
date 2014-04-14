/* 
 * TODO for AS6
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "table.h"
#include "symbol.h"
#include "absyn.h"
#include "temp.h"
#include "tree.h"
#include "frame.h"
#include "assem.h"
#include "codegen.h"
#include "graph.h"


/*
    i added this and it *looks* right, and
    it *seems* right logically, but the compiler
    is complaining about there being no kind or u
    in something not a structure or union, when 
    clearly it is...




Temp_tempList FG_def(G_node n){
	AS_instr *instr = G_nodeInfo(n);
	if(instr != NULL){
		switch (instr->kind){
			case I_OPER:{
				return instr->u.OPER.dst;
				break;
			}
			case I_MOVE:{
				return instr->u.MOVE.dst;
				break;
			}
			assert(0); // only 2 kinds have destination regs
		}
	}
	
}


Temp_tempList FG_use(G_node n){
	AS_instr *instr = G_nodeInfo(n);
	if(instr != NULL){
		switch (instr->kind){
			case I_OPER:{
				return instr->u.OPER.src;
				break;
			}
			case I_MOVE:{
				return instr->u.MOVE.src;
				break;
			}
			assert(0); // only 2 kinds have source regs
		}
	}
}


bool FG_isMove(G_node n){
	AS_instr *instr = G_nodeInfo(n);
	if(instr != NULL){
		return (instr->kind == I_MOVE);
	}
}


G_graph FG_AssemFlowGraph(AS_instrList il){
		// create a new graph from a list of instructions
	G_graph g = G_Graph();
	AS_instrList li = il;
	AS_instr instr;
		
	// create a node for each instruction
	for( li; li != NULL; li = li->tail){
		G_node n = G_Node(g, li);
	}
	
	//for each node in the graph (nodelist) check for jumps
	//otherwise an instr falls through to the next to create edges
	G_nodeList list = G_nodes(g);
	for (list; list != NULL, list->tail->head != NULL; list = list->tail){
		
		G_addEdge(list->head, list->tail->head);
		
	}
	
	
		
	return g;
}

}


void FG_Showinfo(FILE *out, AS_instr instr, Temp_map map){
	
}
*/
