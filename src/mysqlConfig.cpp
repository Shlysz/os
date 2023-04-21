
#include "mysqlConfig.h"

//实现mysqlConfig类
mysqlConfig::mysqlConfig()
{
    //初始化数据库
    if(!initDB("10.128.220.155","root","123456","os_disk"))
    {
        cout<<"数据库初始化失败"<<endl;
    }  
}

mysqlConfig::~mysqlConfig()
{
    mysql_close(conn);
}

bool mysqlConfig::initDB(string host,string user,string pwd,string db_name)
{
    //初始化数据库
    conn=mysql_init(NULL);
    if(!conn)
    {
        cout<<"数据库初始化失败"<<endl;
        return false;
    }
    //连接数据库
    conn=mysql_real_connect(conn,host.c_str(),user.c_str(),pwd.c_str(),db_name.c_str(),0,NULL,0);
    if(conn)
    {
        return true;
    }
    else
    {
        return false;
    }
}

 vector<vector<string>> mysqlConfig::execSelectSQL(string sql)
{
    //执行sql select语句

    int t=mysql_query(conn,sql.c_str());
    if(!t)
    {
        return getRes();
    }
    else
    {
        cout<<"未取到数据"<<endl;
        return vector<vector<string>>();
    }
}

vector<vector<string>> mysqlConfig::getRes()
{
    //获取结果集
    MYSQL_RES *res=mysql_store_result(conn);
    //获取结果集中的列数
    int num_fields=mysql_num_fields(res);
    //获取结果集中的行数
    int num_rows=mysql_num_rows(res);
    //获取结果集中的所有字段名
    MYSQL_FIELD *fields=mysql_fetch_fields(res);
    //获取结果集中的所有数据
    vector<vector<string>> resVec;
    for(int i=0;i<num_rows;i++)
    {
        MYSQL_ROW row=mysql_fetch_row(res);
        vector<string> rowVec;
        for(int j=0;j<num_fields;j++)
        {
            if(row[j]!=NULL)
            rowVec.push_back(row[j]);
            else rowVec.push_back("");
        }
        resVec.push_back(rowVec);
    }
    return resVec;
}

bool mysqlConfig::execInsertSQL(string sql)
{
    //执行sql insert语句
    int t=mysql_query(conn,sql.c_str());
    if(!t)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool mysqlConfig::execUpdateSQL(string sql)
{
    //执行sql update语句
    int t=mysql_query(conn,sql.c_str());
    if(!t)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool mysqlConfig::execDeleteSQL(string sql)
{
    //执行sql delete语句
    int t=mysql_query(conn,sql.c_str());
    if(!t)
    {
        return true;
    }
    else
    {
        return false;
    }
}