#include<mysql/mysql.h>
#include<iostream>
#include<vector>
using namespace std;

class mysqlConfig
{
public:
    mysqlConfig();
    ~mysqlConfig();
    bool initDB(string host,string user,string pwd,string db_name);
    vector<vector<string>> execSelectSQL(string sql);
    vector<vector<string>> getRes();
    bool execInsertSQL(string sql);
    bool execUpdateSQL(string sql);
    bool execDeleteSQL(string sql);
private:
    MYSQL *conn;
    
};