all: sendmsg mycat openfile

sendmsg:sendmsg.c
	gcc -o $@ $^

mycat: mycat.c myopen.c read_fd.c
	gcc -o $@ $^ -lunp

openfile:openfile.c
	gcc -o $@ $^ -lunp

clean:
	rm -f mycat sendmsg
