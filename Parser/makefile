all: parsetest parsetest_display
parsetest_display: table.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o parsetest_display.o
	cc -o parsetest_display -g table.o symbol.o absyn.o y.tab.o lex.yy.o errormsg.o util.o parsetest_display.o

parsetest: table.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o parsetest.o
	cc -o parsetest -g table.o symbol.o absyn.o y.tab.o lex.yy.o errormsg.o util.o parsetest.o

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

#lex.yy.c: tiger.lex
#	lex tiger.lex

util.o: util.c util.h
	cc -g -c util.c

clean: 
	rm -f parsetest   parsetest_display a.out *.o y.tab.c y.tab.h y.tab.o
