
exe:libperf_example.c
	gcc -I/usr/local/include/perf -I/usr/local/lib/perf/include -L/usr/local/lib -lperf libperf_example.c
	gcc -o test test.c
