pagetrace:
# create output files
# lib contains code not written by me
	gcc-14 -c lib/tracereader.c -o tracereader.o
	gcc-14 -c lib/log.c -o log.o
# page_table contains the specific page_table implementation
	gcc-14 -c page_table/page_table.c -o page_table.o
	gcc-14 -c page_table/page_table_params.c -o page_table_params.o

# main file
	gcc-14 -c main.c -o main.o

# link output files
	gcc-14 main.o tracereader.o log.o page_table.o page_table_params.o -o pagetrace

run:
	./pagetrace trace_files/trace.tr "7 15"

compile_run:
	make clean
	make pagetrace
	make run
	make clean

clean:
	rm -f pagetrace 
	rm -f *.o