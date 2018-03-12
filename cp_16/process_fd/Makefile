all:sockets_fd
sockets_fd:socketpair_fd.cpp
	g++ -o $@ $^
clean:
	rm -f sockets_fd newfile
