#compile the tracereader.c file

compile:
	gcc -c tracereader.c -o tracereader.o
	gcc -c trace_tree.c -o trace_tree.o
	gcc -c log.c -o log.o
	gcc -c page_table.c -o page_table.o
	gcc trace_tree.o tracereader.o log.o page_table.o -o trace_tree 

run:
	./trace_tree trace.tr 4 8 8

rc:
	make clean
	make compile
	make run
	make clean

clean:
	rm -f trace_tree 
	rm -f *.o

