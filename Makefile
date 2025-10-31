stego = $(patsubst %.c, %.o, $(wildcard *.c))
stegno.out : $(stego)
	gcc -o $@ $^
clean : 
	rm *.out *.o
