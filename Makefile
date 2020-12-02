compiler:
	gcc compiler.c scanner.c parser.c codegen.c vm.c -I headers -o compile

scanner:
	gcc scanner.c lexeme.c -I headers -o scanner.out