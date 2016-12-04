CC=mpicc

assignment3:
	$(CC) parallel_floyd_s1.c -o parallel_floyd_s1
	$(CC) parallel_floyd_s2.c -o parallel_floyd_s2
	gcc serial_floyd.c -o serial_floyd -lm
	gcc graph_generator.c -o graph_generator
clean:
	rm parallel_floyd_s1
	rm parallel_floyd_s2
	rm serial_floyd
	rm graph_generator