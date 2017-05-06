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
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}


main ( int argc, char **argv ) {

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql;
	sql = (char*) malloc(1024);

	/* Open Database */
	rc = sqlite3_open("flow_monitoring.db", &db);

	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(0);
	}else{
		fprintf(stderr, "Opened database successfully\n");
	}

	/* Create SQL statement */
	sprintf(sql,"CREATE TABLE LAST("  \
	       "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
	       "FLOW 		TEXT 	   NOT NULL," \
	       "PPS           INT    NOT NULL," \
	       "TIMESTAMP            INT     NOT NULL);");

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}else{
		fprintf(stdout, "Table created successfully\n");
	}


	/* Create SQL statement */
	sprintf(sql,"DELETE from LAST; ");

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}else{
		fprintf(stdout, "Records deleted successfully\n");
	}


	/* Create SQL statement */
	sprintf(sql,"INSERT INTO LAST (FLOW,PPS,TIMESTAMP) "  \
	       "VALUES ('%s', %f, %d ); ","13.0.0.1",10, 10); 
	
	fprintf(stdout, "Records: %s \n", sql);
	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}else{
		fprintf(stdout, "Records created successfully\n");
	}
	/* Create SQL statement 
	sql = "INSERT INTO LAST (FLOW,PPS,TIMESTAMP) "  \
	       "VALUES ('10.0.0.1', 10, 100 ); " \
	       "INSERT INTO LAST (FLOW,PPS,TIMESTAMP) "  \
	       "VALUES ('10.0.0.5', 12, 100); " \
	       "INSERT INTO LAST (FLOW,PPS,TIMESTAMP) "  \
	       "VALUES ('10.0.0.10', 15, 100 ); " ;

	 Execute SQL statement 
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}else{
		fprintf(stdout, "Records created successfully\n");
	}

*/
	free(sql);
	/* Close Database */
	sqlite3_close(db);
	return 0;





}
