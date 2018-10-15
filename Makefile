all: calculator

calculator.tab.c calculator.tab.h: calculator.y
		bison -d calculator.y

calculator.lex.c:	 calculator.l calculator.tab.h
		flex -o calculator.lex.c calculator.l

calculator: calculator.lex.c calculator.tab.c calculator.tab.h
		gcc -o calculator -g -lm -ldl -O0 ast_defs.c calculator.lex.c calculator.tab.c -lfl

clean:
	rm calculator calculator.tab.c calculator.lex.c calculator.tab.h
