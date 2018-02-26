all: send recv

send:broadcast_send.c
	gcc -o $@ $^
recv:broadcast_recv.c
	gcc -o $@ $^

.PHONY:clean
clean:
	rm -rf send recv
