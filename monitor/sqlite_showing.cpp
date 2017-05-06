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
      printf("%s = %s || ", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}


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

main ( int argc, char **argv ) {

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql;
	const char* data = "Callback function called";

	/* Open Database */
	rc = sqlite3_open("flow_monitoring.db", &db);

	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(0);
	}else{
		fprintf(stderr, "Opened database successfully\n");
	}

	while(true){
		ClearScreen();
		printf("========================================================================================\n");
		/* Create SQL statement */
		sql = "SELECT * from LAST";

		/* Execute SQL statement */
		rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
		if( rc != SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		//	fprintf(stdout, "Operation done successfully\n");
		}
		printf("_______________________________________________________________________________________\n");
		sleep(4);
	}



	/* Close Database */
	sqlite3_close(db);
	return 0;





}
