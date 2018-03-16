all:test
test:mysql_test.cpp
	g++ mysql_test.cpp `mysql_config --cflags --libs` -o test

clean:
	rm -f test
