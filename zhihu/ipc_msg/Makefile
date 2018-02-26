all:client server

client: common.c client.c
	gcc -o $@ $^
server: common.c server.c
	gcc -o $@ $^

.PHONY:clean
clean:
	rm -rf client server
