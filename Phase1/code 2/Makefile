build:
	gcc process_generator.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc scheduler.c -o scheduler.out
	gcc process.c -o process.out
	gcc test_generator.c -o test_generator.out

clean:
	rm -f *.out  processes.txt

all: clean build


test_sjf:
	./process_generator.out processes.txt -sch 0

test_hpf:
	./process_generator.out processes.txt -sch 1

test_rr:
	./process_generator.out processes.txt -sch 2 -q 5
	# Kill the clock process after testing

test_mlfq:
	./process_generator.out processes.txt -sch 3 -qu 5 