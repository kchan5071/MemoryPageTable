#Kai Chan 
#827673009

pagetrace:
# create output files
# lib contains code not written by me
	gcc -c tracereader.c -o tracereader.o
	gcc -c log.c -o log.o
# page_table contains the specific page_table implementation
	gcc -c page_table.c -o page_table.o
	gcc -c page_table_params.c -o page_table_params.o

# main file
	gcc -c main.c -o main.o

# link output files to pagetrace executable
	gcc main.o tracereader.o log.o page_table.o page_table_params.o -o pagetrace

run:
	make pagetrace
	./pagetrace -o TEST -n 500 -c 20 trace_files/trace.tr "7 15" 
	make clean


compile_run:
	make clean
	make pagetrace
	make run
	make clean

clean:
	rm -f pagetrace 
	rm -f *.o