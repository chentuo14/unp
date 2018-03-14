all: server client myserver myclient

server:server.c
	g++ -o $@ $^
client:client.c
	g++ -o $@ $^
myclient:myclient.c
	g++ -o $@ $^

clean:
	rm -f server client myclient
