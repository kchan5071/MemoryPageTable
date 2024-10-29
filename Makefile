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
	gcc -c TLB_table.c -o TLB_table.o
	gcc -c recently_accessed_pages.c -o recently_accessed_pages.o

# main file
	gcc -c main.c -o main.o

# link output files to pagingwithatc executable
	gcc main.o tracereader.o log.o page_table.o page_table_params.o TLB_table.o recently_accessed_pages.o -o pagingwithatc -lm

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
	make clean
	make pagingwithatc
	# ./pagingwithatc trace_files/trace.tr 20
	# ./pagingwithatc trace_files/trace.tr 7 3 5 -n 1000
	./pagingwithatc trace_files/trace.tr 7 7 7 -c 4
	./pagingwithatc trace_files/trace.tr 2 1 10 -n 9999 -c 7
	# ./pagingwithatc trace_files/trace.tr -o bitmasks 3 7 11
	# ./pagingwithatc trace_files/trace.tr -o offset 4 4 12
	# ./pagingwithatc trace_files/trace.tr -o vpn2pfn 2 8 12
	# ./pagingwithatc trace_files/trace.tr -o va2pa 2 10 2
	# ./pagingwithatc trace_files/trace.tr -o va2pa_atc_ptwalk 20 2 -c 10
	# ./pagingwithatc trace_files/trace.tr 12 0 8	
	# ./pagingwithatc trace_files/trace.tr 20 8 1
	# ./pagingwithatc trace_files/trace.tr 8 8 8 -c -1
	# ./pagingwithatc trace_files/trace.tr 8 8 8 -n -1
	# ./pagingwithatc trace_files/trace.tr 8 8 8 -c 20
	# ./pagingwithatc trace_files/trace.tr 8 6 6 4
	# ./pagingwithatc trace_files/trace.tr 8 8 4
	# ./pagingwithatc -o bitmasks trace_files/trace.tr 4 8 8
	# ./pagingwithatc -n 30 -o offset trace_files/trace.tr 8 8 4
	# ./pagingwithatc -n 20 -o vpn2pfn trace_files/trace.tr 6 4 8
	# ./pagingwithatc -n 20 -o va2pa trace_files/trace.tr 20
	# ./pagingwithatc -n 20 -c 5 -o va2pa_atc_ptwalk trace_files/trace.tr 4 8 6
	# ./pagingwithatc -n 8000 -o summary trace_files/trace.tr 24
	# ./pagingwithatc trace_files/trace.tr 8 8 8
	make clean


compile_run:
	make clean
	make pagingwithatc
	make run
	make clean

clean:
	rm -f pagingwithatc
	rm -f *.o