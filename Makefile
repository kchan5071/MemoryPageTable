pagetrace:
# create output files
# lib contains code not written by me
	gcc -c lib/tracereader.c -o tracereader.o
	gcc -c lib/log.c -o log.o
# page_table contains the specific page_table implementation
	gcc -c page_table/page_table.c -o page_table.o
	gcc -c page_table/page_table_params.c -o page_table_params.o

# main file
	gcc -c main.c -o main.o

# link output files
	gcc main.o tracereader.o log.o page_table.o page_table_params.o -o pagetrace

run:
	./pagetrace trace_files/trace.tr 4 8 8

compile_run:
	make clean
	make pagetrace
	make run
	make clean

clean:
	rm -f pagetrace 
	rm -f *.o