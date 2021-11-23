matrix: mm.c 
	gcc -o matrix.exe -O3 mm.c 

blocked: mm_blocked.c 
	gcc -o matrix.exe -O3 mm_blocked.c 

run: matrix.exe
	./matrix.exe

.PHONY: matrix

clean: matrix.exe
	rm matrix.exe