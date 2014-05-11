CC= gcc
obj=errno.o main.o mystring.o global.o passwd.o create.o file.o insert.o select.o
exe=main
head=errno.h mystring.h global.h passwd.h create.h file.h select.h
main:   $(head) $(obj)
	$(CC) -o main $(obj) 
errno.o: errno.c
	$(CC) -c -o errno.o errno.c
main.o: main.c
	$(CC) -c -o main.o  main.c
mysring.o: mystring.c
	$(CC) -c -o mystring.o mystring.c
global.o: global.c
	$(CC) -c -o global.o global.c
passwd.o: passwd.c
	$(CC) -c -o passwd.o passwd.c
create.o:create.c
	$(CC) -c -o create.o create.c
file.o: file.c
	$(CC) -c -o file.o file.c
insert.o:insert.c
	$(CC) -c -o insert.o insert.c
select.o:select.c
	$(CC) -c -o select.o select.c
run:
	./main
clean:
	rm $(obj) $(exe)



