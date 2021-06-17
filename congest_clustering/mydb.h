#ifndef __DEF_DB__HPP
#define __DEF_DB__HPP

#include <memory>
#include <thread>
#include <string>
#include <chrono>
#include "tqueue.h"
#include "mysql/mysql.h"
struct logEplsTimeout{
	logEplsTimeout(const std::string &msg)
		:m_msg(msg)
	{
		 m_start = std::chrono::steady_clock::now();
	}
	~logEplsTimeout();
	std::string m_msg;
	std::chrono::steady_clock::time_point m_start;
};
struct myDb{
    static myDb * inst();
    bool start(const std::string &key,const std::string &,const std::string &,const std::string &,const std::string &);
    bool tryPut(const std::string &sql);
    bool m_bFlag=false;
private:
    myDb();
    ~myDb();
    myDb(const myDb &rhs)=delete;
    myDb &operator=(const myDb&rhs)=delete;
    void run();
	void close();
    bool connect();
    std::unique_ptr<std::thread> m_pThread;
    BoundedQueue<std::string> m_queue;
	MYSQL *m_pMysql;
	std::string m_host;
	std::string m_port;
	std::string m_user;
	std::string m_password;
	std::string m_database;
};
#endif
