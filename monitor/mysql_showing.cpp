#include <mysql.h>
#include <functional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <string.h>
#include <curses.h>
#include <pcap.h>
#include <term.h>
#include <stdio.h>



void ClearScreen()
{
	//std::system("clear");

	if (!cur_term)
	{
		int result;
		setupterm( NULL, STDOUT_FILENO, &result );
		if (result <= 0) return;
	}
	putp( tigetstr( "clear" ) );

}

void finish_with_error(MYSQL *con)
{
	fprintf(stderr, "%s\n", mysql_error(con));
	mysql_close(con);
	exit(1);        
}


main() {
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	char *server = "localhost";
	char *user = "root";
	char *password = "37"; /* set me first */
	char *database = "mysql";

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, server,
				user, password, database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}

	while(true){
		ClearScreen();

		if (mysql_query(conn, "SELECT * FROM LAST;")) 
		{
			finish_with_error(conn);
		}

		res = mysql_store_result(conn);

		if (res == NULL) 
		{
			finish_with_error(conn);
		}

		int num_fields = mysql_num_fields(res);

		row;

		while ((row = mysql_fetch_row(res))) 
		{ 
			for(int i = 0; i < num_fields; i++) 
			{ 
				printf("%s ||", row[i] ? row[i] : "NULL"); 
			} 
			printf("\n"); 
		}

		/* close connection */
		mysql_free_result(res);
		sleep(2);
	}
	mysql_close(conn);
}
