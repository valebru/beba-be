#include <string>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <mysql.h>


main ( int argc, char **argv ) {

	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	char *server = "localhost";
	char *user = "root";
	char *password = "37"; /* set me first */
	char *database = "mysql";
	char *sql;
	int timestamp = 0;
	sql= (char*) malloc(1024);

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, server,
				user, password, database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}




	/* Create SQL statement */
	sprintf(sql,"CREATE TABLE LAST("  \
			"ID INTEGER PRIMARY KEY AUTO_INCREMENT," \
			"FLOW 		TEXT 	   NOT NULL," \
			"PPS           INT    NOT NULL," \
			"TIMESTAMP            INT     NOT NULL);");


	/* send SQL query */
	if (mysql_query(conn, sql)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
//		exit(1);
	}

	/* Create SQL statement */
	sprintf(sql,"CREATE TABLE HISTORY("  \
			"ID INTEGER PRIMARY KEY AUTO_INCREMENT," \
			"FLOW 		TEXT 	   NOT NULL," \
			"PPS           INT    NOT NULL," \
			"TIMESTAMP            INT     NOT NULL);");

	/* send SQL query */
	if (mysql_query(conn, sql)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
//		exit(1);
	}


	/* Create SQL statement */
	sprintf(sql,"DELETE FROM LAST;");

	/* send SQL query */
	if (mysql_query(conn, sql)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
//		exit(1);
	}


	/* Create SQL statement */
	sprintf(sql,"DELETE FROM HISTORY;");

	/* send SQL query */
	if (mysql_query(conn, sql)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
//		exit(1);
	}


	/* Create SQL statement */
	sprintf(sql,"INSERT INTO LAST (FLOW,PPS,TIMESTAMP) \n" \
			"VALUES ('10.0.0.1', 12, 11 ),  \n" \ 
			"('10.0.0.5', 12, 11 ),  \n" \ 
			"('10.0.0.115', 11, 11 ),  \n" \ 
			" ('10.0.0.5', 15, 11 );");

	/* send SQL query */
	if (mysql_query(conn, sql)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
//		exit(1);
	}


	/* close connection */
	mysql_close(conn);


	return 0;
}
