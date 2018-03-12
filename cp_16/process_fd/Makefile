all:sockets_fd 3process_fd
sockets_fd:socketpair_fd.cpp
	g++ -o $@ $^

3process_fd:3process_fd.cpp 3process_fun.cpp
	g++ -o $@ $^

clean:
	rm -f sockets_fd newfile 3process_fd
