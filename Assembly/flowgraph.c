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



Temp_tempList FG_def(G_node n){
	AS_instr instr = G_nodeInfo(n);
	if(instr != NULL){
		switch ((instr)->kind){
			case I_OPER:{
				return (instr)->u.OPER.dst;
				break;
			}
			case I_MOVE:{
				return (instr)->u.MOVE.dst;
				break;
			}
			assert(0); // only 2 kinds have destination regs
		}
	}
	
}


Temp_tempList FG_use(G_node n){
	AS_instr instr = G_nodeInfo(n);
	if(instr != NULL){
		switch ((instr)->kind){
			case I_OPER:{
				return (instr)->u.OPER.src;
				break;
			}
			case I_MOVE:{
				return (instr)->u.MOVE.src;
				break;
			}
			assert(0); // only 2 kinds have source regs
		}
	}
}


bool FG_isMove(G_node n){
	AS_instr instr = G_nodeInfo(n);
	if(instr != NULL){
		return ((instr)->kind == I_MOVE);
	}
}

bool FG_isLabel(G_node n){
	AS_instr instr = G_nodeInfo(n);
	if(instr != NULL){
		return ((instr)->kind == I_LABEL);
	}
}

bool FG_isOper(G_node n){
	AS_instr instr = G_nodeInfo(n);
	if(instr != NULL){
		return ((instr)->kind == I_OPER);
	}
}


G_graph FG_AssemFlowGraph(AS_instrList il){
		// create a new graph from a list of instructions
	G_graph g = G_Graph();
	AS_instrList li = il;
	AS_instr instr1, instr2;
	
		
	// create a node for each instruction
	for( li; li != NULL; li = li->tail){
		G_node n = G_Node(g, li);
	}
	
	//for each node in the graph (nodelist) check for jumps
	//otherwise an instr falls through to the next to create edges
	G_nodeList list = G_nodes(g);
	G_nodeList curr = G_nodes(g);
	
	for (list; list != NULL, list->tail->head != NULL; list = list->tail){
		// adds an edge between consecutive nodes
		G_addEdge(list->head, list->tail->head);
		 
		if(FG_isOper(list->head)){
			instr1 = (AS_instr)G_nodeInfo(list->head);
            instr2 = (AS_instr)G_nodeInfo(curr->head);
			if((instr1)->u.OPER.jumps){
                while(instr2->u.LABEL.label != instr1->u.OPER.jumps){
					curr = curr->tail;
                    instr2 = (AS_instr)G_nodeInfo(curr->head);
				}
				G_addEdge(list->head, curr->head);
			}
		}
		curr = G_nodes(g);
	}
	
	
		
	return g;
}




void FG_Showinfo(FILE *out, AS_instr instr, Temp_map map){
       
	char r[200]; /* result */

/* For test
static int cnt=1;
printf("instr %d\n", cnt++);
  switch (instr->kind) {
  case I_OPER:
    fprintf(out, "OPER: %s", instr->u.OPER.assem);
    break;
  case I_LABEL:
    fprintf(out, "LABEL: %s", instr->u.LABEL.assem);
    break;
  case I_MOVE:
    fprintf(out, "MOVE: %s", instr->u.MOVE.assem);
    break;
  }
return;
*/

  switch (instr->kind) {
  case I_OPER:
    AS_format(r, instr->u.OPER.assem, instr->u.OPER.dst, instr->u.OPER.src, instr->u.OPER.jumps, map);
    fprintf(out, "%s", r);
    break;
  case I_LABEL:
    AS_format(r, instr->u.LABEL.assem, NULL, NULL, NULL, map); 
    fprintf(out, "%s", r); 
    /* instr->u.LABEL->label); */
    break;
  case I_MOVE:
    AS_format(r, instr->u.MOVE.assem, instr->u.MOVE.dst, instr->u.MOVE.src, NULL, map);
    fprintf(out, "%s", r);
    break;
  }
}

