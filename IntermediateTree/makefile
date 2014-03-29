all: parsetest parsetest_display
parsetest_display: semant.o types.o table.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o parsetest_display.o temp.o tree.o mipsframe.o printtree.o translate.o
	cc -o parsetest_display -g semant.o types.o table.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o parsetest_display.o temp.o tree.o mipsframe.o printtree.o translate.o

parsetest: parsetest.o semant.o types.o table.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o temp.o tree.o mipsframe.o printtree.o translate.o
	cc -o parsetest -g parsetest.o semant.o types.o table.o symbol.o absyn.o y.tab.o lex.yy.o errormsg.o util.o  temp.o tree.o mipsframe.o printtree.o translate.o

parsetest.o: parsetest.c errormsg.h util.h absyn.h
	cc -g -c parsetest.c

parsetest_display.o: parsetest.c errormsg.h util.h absyn.h
	cc -DDISPLAY -g -c parsetest.c -o parsetest_display.o

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

# PA5 related stuffs
translate.o: translate.c translate.h
	gcc -g -c translate.c

mipsframe.o: mipsframe.c mipsframe.h
	cc -g -c mipsframe.c

printtree.o: printtree.c printtree.h
	cc -g -c printtree.c

util.o: util.c util.h
	cc -g -c util.c

clean: 
	rm -f parsetest parsetest_display a.out *.o y.tab.c y.tab.h y.tab.o
