#include <stdio.h>
#include <mysql.h>

int main(int argc, char *argv[])
{
	MYSQL conn;
	int res;
	mysql_init(&conn);
	if(mysql_real_connect(&conn, "localhost", "root",
				"12345678", "sensordb", 0, NULL, 0)) {
		printf("connect success!\n");
		
		mysql_close(&conn);
	}
	return 0;
}

