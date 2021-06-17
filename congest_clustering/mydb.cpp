#include "mydb.h"
#include "LogUtils.h"
#include <chrono>

logEplsTimeout::~logEplsTimeout(){
	auto end=std::chrono::steady_clock::now();
	NOTICE(m_msg<<":"<<std::chrono::duration_cast<std::chrono::milliseconds>(end-m_start).count()<<"ms");
}
myDb::myDb()
    :m_queue(8192)
{
}
bool myDb::connect(){
    bool bFlag=true;
	m_pMysql=mysql_init(NULL);
	char v=1;
	mysql_options(m_pMysql,MYSQL_OPT_RECONNECT,&v);
	if(!mysql_real_connect(m_pMysql,m_host.c_str(),m_user.c_str(),m_password.c_str(),m_database.c_str(),std::stoi(m_port),NULL,0)){
		bFlag=false;
	}else{
		mysql_query(m_pMysql,"set names utf8");
	}
    return bFlag;
}
bool myDb::start(const std::string &host,const std::string &port,const std::string &user,const std::string &password,const std::string &db){
	bool flag=true;
	m_host=host;m_port=port;m_user=user;m_password=password;m_database=db;
	flag = connect();
	if(flag){
		m_pThread.reset(new std::thread(&myDb::run,this));
	}
	return flag;
}

void myDb::run(){
	bool change_flag=false;
    while(!m_bFlag) {
		if (m_queue.size() != 0)
			NOTICE("current_pool_size:"<<m_queue.size());
        std::string sql;
        if(m_queue.tryGet(sql)) {
			if(change_flag){
				change_flag=false;
				mysql_query(m_pMysql,"set names utf8;");
			}
			logEplsTimeout let("Exec_sql:"+sql);
			int code =mysql_query(m_pMysql,sql.c_str());
			if(code!=0){
				mysql_query(m_pMysql,"set names utf8");
				NOTICE("Exec_sql_error:"<<code<<":"+sql);
			}
			if(code==2006){
				if(mysql_ping(m_pMysql))
					std::this_thread::sleep_for(std::chrono::seconds(10));
				mysql_query(m_pMysql,"set names utf8");
			}
        }else{
			change_flag=true;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}
bool myDb::tryPut(const std::string& sql){
    m_queue.put(sql);
	return true;
}
void myDb::close(){
	if(m_pMysql)
		mysql_close(m_pMysql);
}
myDb::~myDb(){
    m_bFlag=true;
    m_pThread->join();
	close();
}
myDb * myDb::inst(){
    static myDb db;
    return &db;
}
