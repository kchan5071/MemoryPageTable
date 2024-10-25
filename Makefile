#Kai Chan 
#827673009

pagingwithatc:
# create output files
# lib contains code not written by me
	gcc -c tracereader.c -o tracereader.o
	gcc -c log.c -o log.o
# page_table contains the specific page_table implementation
	gcc -c page_table.c -o page_table.o
	gcc -c page_table_params.c -o page_table_params.o

# main file
	gcc -c main.c -o main.o

# link output files to pagingwithatc executable
	gcc main.o tracereader.o log.o page_table.o page_table_params.o -o pagingwithatc

run-tests:
	make pagingwithatc
#default case
	./pagingwithatc trace_files/trace.tr 7 15 3
#check for args to work
	./pagingwithatc -o TEST -n 500 -c 20 trace_files/trace.tr 7 15
#check for missing file
	./pagingwithatc trace_files/missing_file.tr 7 15 3
	./pagingwithatc trace_files/trace 7 15 3

#check for bit validity
	./pagingwithatc trace_files/trace.tr 7 15 0
	./pagingwithatc trace_files/trace.tr 7 15 4 4 4 4

	make clean

run:
	make pagingwithatc
	./pagingwithatc -o TEST -n 500 -c 20 trace_files/trace.tr 7 15 3
	make clean


compile_run:
	make clean
	make pagingwithatc
	make run
	make clean

clean:
	rm -f pagingwithatc
	rm -f *.o