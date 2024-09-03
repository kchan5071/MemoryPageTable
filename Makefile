#compile the tracereader.c file

compile:
	gcc -c tracereader.c -o tracereader.o
	gcc -c trace_tree.c -o trace_tree.o
	gcc -c log.c -o log.o
	gcc -c page_table.c -o page_table.o
	gcc trace_tree.o tracereader.o log.o page_table.o -o pagetrace

run:
	./pagetrace trace.tr 4 8 8

compile_run:
	make clean
	make compile
	make run
	make clean

clean:
	rm -f pagetrace 
	rm -f *.o

