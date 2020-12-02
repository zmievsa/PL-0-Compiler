compiler:
	gcc -static -g compiler.c scanner.c parser.c symbol.c lexeme.c codegen.c instruction.c vm.c -I headers -o compile.out

scanner:
	gcc scanner.c lexeme.c -I headers -o scanner.out