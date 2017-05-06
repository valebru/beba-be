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
#include <sqlite3.h>

int win_time=2;

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



using namespace std;
//typedef multimap<int, string> saveMap;
typedef multimap<int,unsigned int> saveMap;
typedef unordered_map<int, int> CounterMap;
//typedef unordered_map<string, int> CounterMap;

CounterMap sip_map;


//interval=1000000/std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count();

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}


void sort_umap( CounterMap& v, double interval, sqlite3* db, int timestamp, char* sql1, char* sql2, char* buff){

	saveMap b;
	char *zErrMsg = 0;
	int rc;

	for (CounterMap::iterator it = v.begin(); it != v.end(); ++it) {
		b.insert(make_pair(it->second, it->first));
	}

	/* Create SQL statement */
	sprintf(sql1,"DELETE from LAST;" \
			"DELETE from sqlite_sequence WHERE name='LAST';");

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql1, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}else{
		fprintf(stdout, "LAST table flushed successfully\n");
	}

	sprintf(sql1,"INSERT INTO LAST (FLOW,PPS,TIMESTAMP) \n");
	sprintf(sql2,"INSERT INTO HISTORY (FLOW,PPS,TIMESTAMP) \n");
	int i=9;
	for (saveMap::reverse_iterator it = b.rbegin(); it != b.rend(); ++it) {
		cout << inet_ntoa({it->second}) << "\t" << it->first/interval << " pkt/sec " << endl;
		//insert select sql!!

		/* Create SQL statement */
		sprintf(buff,	"VALUES ('%s', %f, %d ) \n ", 
				inet_ntoa({it->second}), it->first/interval, timestamp); 


		strcat(sql1,buff);
		strcat(sql2,buff);
		if (0==i--) break;
		strcat(sql1,",");
		strcat(sql2,",");
	}
	
	strcat(sql1,";");
	strcat(sql1,sql2);
	fprintf(stdout, "Records: %s\n",sql1);
	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql1, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}else{
		fprintf(stdout, "Records created successfully\n");
	}

}


main ( int argc, char **argv ) {

	int sip=0;
	int num_pkt=0;
	pcap_t *handle;			/* Session handle */
	char dev[128];			/* The device to sniff on */
	char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */
	struct bpf_program fp;		/* The compiled filter */
	char filter_exp[] = "port 23";	/* The filter expression */
	bpf_u_int32 mask;		/* Our netmask */
	bpf_u_int32 net;		/* Our IP */
	struct pcap_pkthdr header;	/* The header that pcap gives us */
	const u_char *packet;		/* The actual packet */


	if (argc==2) strcpy(dev,argv[1]);
std:cout << dev << std::endl;

    /* Define the device */
    if (dev == NULL) {
	    fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
	    return(2);
    }
    /* Find the properties for the device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
	    fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
	    net = 0;
	    mask = 0;
    }
    /* Open the session in promiscuous mode */
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
	    fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
	    return(2);
    }
    /* Compile and apply the filter */
    /*if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
      fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
      return(2);
      }
      if (pcap_setfilter(handle, &fp) == -1) {
      fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
      return(2);
      }*/


    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    char *sql2;
    char *buff;
    int timestamp = 0;
    sql= (char*) malloc(1024);
    sql2= (char*) malloc(1024);
    buff= (char*) malloc(1024);

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
	    fprintf(stdout, "Operation done successfully\n\n");
    }
    /* Create SQL statement */
    sprintf(sql,"CREATE TABLE HISTORY("  \
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
	    fprintf(stdout, "Operation done successfully\n\n");
    }
    /* Create SQL statement */
    sprintf(sql,"DELETE FROM LAST;" \
		    "DELETE from sqlite_sequence WHERE name='LAST';"
		    "DELETE FROM HISTORY;" \
		    "DELETE from sqlite_sequence WHERE name='HISTORY';");

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
	    fprintf(stderr, "SQL error: %s\n", zErrMsg);
	    sqlite3_free(zErrMsg);
    }else{
	    fprintf(stdout, "Operation done successfully\n\n");
    }

    bool first=true;
    timeval start;
    for(;;) {
	    ClearScreen();
	    for (;;) {
		    /* Grab a packet */
		    packet = pcap_next(handle, &header);
		    if(packet == NULL) continue;
		    if (first) {
			    start = header.ts;
			    first=false;
		    }
		    if  ((header.len>14) && (0x0008 == *reinterpret_cast<const uint16_t *>(packet + 12))) {
			    const iphdr *ipv4 = reinterpret_cast<const iphdr *> (packet + 14);
			    if (!ipv4) continue;
			    if (ipv4->protocol == IPPROTO_TCP || ipv4->protocol == IPPROTO_UDP){
				    sip_map[ipv4->saddr]++;
				    if (sip_map[ipv4->saddr] == 1)
					    sip++;
			    }
		    }
		    num_pkt++;
		    timeval current = header.ts;
		    timestamp = static_cast<int> (current.tv_sec);
		    timeval diff;
		    timersub(&current, &start, &diff);
		    if (diff.tv_sec > (win_time-1)) break;
		    //if (diff.tv_usec > 100000) break;
		    //if (num_pkt > 10000) break;
	    }
	    std::cout << "num_pkt: " << num_pkt << std::endl;
	    std::cout << "sip: " << sip << std::endl;
	    sip = 0;
	    num_pkt=0;
	    first=true;
	    //sort_umap(sip_map, win_time);
	    sort_umap(sip_map, win_time, db, timestamp, sql, sql2, buff);
	    sip_map.clear();
	    //sleep(1);
    }

    free(sql);
    free(sql2);
    free(buff);
    /* Close Database */
    sqlite3_close(db);
    /* And close the session */
    pcap_close(handle);
    return(0);

}
