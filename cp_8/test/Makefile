all:server client01

server: server.cpp
	g++ -o $@ $^

client01: client01.cpp
	g++ -o $@ $^

.PHONY:clean
clean:
	rm -rf server client01
