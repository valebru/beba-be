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
#include <mysql.h>

int win_time=1;

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


void sort_umap(CounterMap& v, double interval, MYSQL *conn, int timestamp, char* sqlL, char* sqlH, char* buff) {

	saveMap b;
	int blength=0;

	for (CounterMap::iterator it = v.begin(); it != v.end(); ++it) {
		b.insert(make_pair(it->second, it->first));
		blength++;
	}

	/* Create SQL statement */
	sprintf(sqlL,"DELETE from LAST;");

	/* send SQL query */
	if (mysql_query(conn, sqlL)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
	}
	/* Create SQL statement */
	sprintf(sqlL,"ALTER TABLE LAST AUTO_INCREMENT = 1;");

	/* send SQL query */
	if (mysql_query(conn, sqlL)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
	}


	sprintf(sqlL,"INSERT INTO LAST (FLOW,PPS,TIMESTAMP) VALUES ");
	sprintf(sqlH,"INSERT INTO HISTORY (FLOW,PPS,TIMESTAMP) VALUES ");
	int i=0;
	for (saveMap::reverse_iterator it = b.rbegin(); it != b.rend(); ++it) {
		cout << inet_ntoa({it->second}) << "\t" << it->first/interval << " pkt/sec " << endl;

		/* Create SQL statement */
		sprintf(buff, "('%s', %f, %d ) " \
				,inet_ntoa({it->second}), it->first/interval, timestamp); 

		strcat(sqlL,buff);
		strcat(sqlH,buff);
		if (++i == blength || i>10) break;
		strcat(sqlL,",");
		strcat(sqlH,",");
	}
	
	strcat(sqlL,";");
	strcat(sqlH,";");
//	fprintf(stdout, "===Records: %s\n", sqlL);
//	fprintf(stdout, "===Records: %s\n", sqlH);

	/* send SQL query */
	if (mysql_query(conn, sqlL)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
	}

	/* send SQL query */
	if (mysql_query(conn, sqlH)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
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

	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	char *server = "localhost";
	char *user = "root";
	char *password = "37"; /* set me first */
	char *database = "mysql";
	char *sql1;
	char *sql2;
	char *buff;
	int timestamp = 0;
	sql1= (char*) malloc(1024);
	sql2= (char*) malloc(1024);
	buff= (char*) malloc(1024);

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, server,
				user, password, database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}




	/* Create SQL statement */
	sprintf(sql1,"CREATE TABLE LAST("  \
			"ID INTEGER PRIMARY KEY AUTO_INCREMENT," \
			"FLOW 		TEXT 	   NOT NULL," \
			"PPS           INT    NOT NULL," \
			"TIMESTAMP            INT     NOT NULL);");


	/* send SQL query */
	if (mysql_query(conn, sql1)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
//		exit(1);
	}

	/* Create SQL statement */
	sprintf(sql1,"CREATE TABLE HISTORY("  \
			"ID INTEGER PRIMARY KEY AUTO_INCREMENT," \
			"FLOW 		TEXT 	   NOT NULL," \
			"PPS           INT    NOT NULL," \
			"TIMESTAMP            INT     NOT NULL);");

	/* send SQL query */
	if (mysql_query(conn, sql1)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
//		exit(1);
	}


	/* Create SQL statement */
	sprintf(sql1,"DELETE FROM LAST;");

	/* send SQL query */
	if (mysql_query(conn, sql1)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
//		exit(1);
	}


	/* Create SQL statement */
	sprintf(sql1,"DELETE FROM HISTORY;");

	/* send SQL query */
	if (mysql_query(conn, sql1)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
//		exit(1);
	}

	/* Create SQL statement */
	sprintf(sql1,"ALTER TABLE LAST AUTO_INCREMENT = 1;");

	/* send SQL query */
	if (mysql_query(conn, sql1)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
//		exit(1);
	}

	/* Create SQL statement */
	sprintf(sql1,"ALTER TABLE HISTORY AUTO_INCREMENT = 1;");

	/* send SQL query */
	if (mysql_query(conn, sql1)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
//		exit(1);
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
	    sort_umap(sip_map, win_time, conn, timestamp, sql1, sql2, buff);
	    sip_map.clear();
	    //sleep(1);
    }

    free(sql1);
    free(sql2);
    free(buff);
    mysql_close(conn);
    /* And close the session */
    pcap_close(handle);
    return(0);

}
