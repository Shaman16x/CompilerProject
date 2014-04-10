#
# Modified by Ming Zhou
#

all: main

# Added for Assignment 6 START
main: main.o semant.o types.o table.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o tree.o temp.o frame.o translate.o printtree.o canon.o assem.o codegen.o flowgraph.o graph.o
	cc -o tiger -g main.o semant.o types.o table.o symbol.o absyn.o y.tab.o lex.yy.o errormsg.o util.o tree.o temp.o frame.o translate.o printtree.o canon.o assem.o codegen.o flowgraph.o graph.o

main.o: main.c
	cc -g -c main.c

codegen.o: mipscodegen.c codegen.h
	cc -g -o codegen.o -c mipscodegen.c

canon.o: canon.c canon.h
	cc -g -c canon.c

assem.o: assem.c assem.h
	cc -g -c assem.c

flowgraph.o: flowgraph.c flowgraph.h
	cc -g -c flowgraph.c

graph.o: graph.c graph.h
	cc -g -c graph.c
# Added for Assignment 6 END

# Added for Assignment 5 START
translate_test: translate.o
	cc -o translate_test -g translate.o frame.o util.o temp.o table.o symbol.o tree.o

frame_test: frame.o
	cc -o frame_test -g frame.o util.o temp.o table.o symbol.o

translate.o: translate.c translate.h frame.h translate.h mipsframe.h tree.h
	cc -g -c translate.c

frame.o: mipsframe.c mipsframe.h frame.h
	cc -o frame.o -g -c mipsframe.c

printtree.o: printtree.c printtree.h
	cc -g -c printtree.c

tree.o: tree.c tree.h
	cc -g -c tree.c

temp.o: temp.c temp.h
	cc -g -c temp.c
# Added for Assignment 5 END

table.o: table.c
	cc -g -c table.c

y.tab.o: y.tab.c
	cc -g -c y.tab.c

y.tab.c: tiger.grm
	yacc -dv tiger.grm

y.tab.h: y.tab.c
	echo "y.tab.h was created at the same time as y.tab.c"

errormsg.o: errormsg.c errormsg.h util.h
	cc -g -c errormsg.c

lex.yy.o: lex.yy.c y.tab.h errormsg.h util.h
	cc -g -c lex.yy.c

absyn.o: absyn.c absyn.h
	cc -g -c absyn.c

types.o: types.c types.h
	cc -g -c types.c

semant.o: semant.c semant.h
	cc -g -c semant.c
#lex.yy.c: tiger.lex
#	lex tiger.lex

util.o: util.c util.h
	cc -g -c util.c

clean: 
	rm -f tiger parsetest parsetest_display a.out *.o y.tab.c y.tab.h y.tab.o
