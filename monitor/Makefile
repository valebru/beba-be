all:	top-sqlite3 top-mysql

top-sqlite3:
	g++ -ggdb -std=c++11  main_sqlite.cpp -lpcap -lcurses -l sqlite3 -o sip_topSQLITE;
	g++ -ggdb -std=c++11 sqlite_showing.cpp -lcurses -l sqlite3 -o sqlite_show;
	g++ -ggdb -std=c++11 sqlite_test.cpp -l sqlite3 -o sqlite_test;

top-mysql:
	g++ -ggdb -std=c++11 -I/usr/include/mysql -DBIG_JOINS=1  -fno-strict-aliasing    -g -DNDEBUG  main_mysql.cpp -o sip_topMYSQL -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl -lpcap -lcurses
	g++ -ggdb -std=c++11 -I/usr/include/mysql -DBIG_JOINS=1  -fno-strict-aliasing    -g -DNDEBUG mysql_showing.cpp -lcurses -o mysql_show -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl
	g++ -ggdb -std=c++11 -I/usr/include/mysql -DBIG_JOINS=1  -fno-strict-aliasing    -g -DNDEBUG mysql_test.cpp -lcurses -o mysql_test -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl
