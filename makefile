assembler: assembler.o macroProcedure.o firstTransition.o secondTransition.o createOutputFiles.o transitionsFunctions.o
	gcc -g -ansi -Wall -pedantic assembler.o -o assembler

assembler.o: assembler.c 
	gcc -c -g -ansi -Wall -pedantic assembler.c -o assembler.o 
macroProcedure.o: macroProcedure.c header.h 
	gcc -c -g -ansi -Wall -pedantic macroProcedure.c -o macroProcedure.o
firstTransition.o: firstTransition.c header.h
	gcc -c -g -ansi -Wall -pedantic firstTransition.c -o firstTransition.o
secondTransition.o: secondTransition.c header.h
	gcc -c -g -ansi -Wall -pedantic secondTransition.c -o secondTransition.o
createOutputFiles.o: createOutputFiles.c header.h
	gcc -c -g -ansi -Wall -pedantic createOutputFiles.c -o createOutputFiles.o
transitionsFunctions.o: transitionsFunctions.c header.h
	gcc -c -g -ansi -Wall -pedantic transitionsFunctions.c -o transitionsFunctions.o
