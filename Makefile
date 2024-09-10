#Kai Chan 
#827673009

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

# link output files to pagetrace executable
	gcc main.o tracereader.o log.o page_table.o page_table_params.o -o pagetrace

run-complete:
	./pagetrace trace_files/trace.tr "7 15" > results/page_trage_7_15.txt
	./pagetrace trace_files/trace.tr "2 3 4 5 8" > results/page_trage_2_3_4_5_8.txt
	./pagetrace trace_files/trace.tr "6 2 4 2 2" > results/page_trage_6_2_4_2_2.txt
	./pagetrace trace_files/trace.tr "3 5 7" > results/page_trage_3_5_7.txt
	./pagetrace trace_files/trace.tr "2 2 2 2 2 2 2 2 2 2 2" > results/page_trage_2_2_2_2_2_2_2_2_2_2_2.txt
	./pagetrace trace_files/trace.tr "1" > results/page_trage_1.txt
	./pagetrace trace_files/trace.tr "20" > results/page_trage_20.txt
	./pagetrace trace_files/trace.tr "12 12" > results/page_trage_12_12.txt
	./pagetrace trace_files/trace.tr "4 4 4 4" > results/page_trage_4_4_4_4.txt
	./pagetrace trace_files/trace.tr "8 8 8" > results/page_trage_8_8_8.txt

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