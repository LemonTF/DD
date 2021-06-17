
#include "LogUtils.h"
#include <memory>


Logger LogUtils::_logger = log4cplus::Logger::getInstance("main_log");

LogUtils::LogUtils(){
}

LogUtils::~LogUtils(){
}

LogUtils& LogUtils::instance()
{
    static LogUtils log;
    return log;
}

void LogUtils::setLogName(const std::string &s){
	m_logName=s;
}
bool LogUtils::open_log(){
    //SharedAppenderPtr _append(new FileAppender(_log_name));
    SharedAppenderPtr _append(new DailyRollingFileAppender(m_logName,DAILY,true,5));
    _append->setName(LOG4CPLUS_TEXT("file"));

	//std::string pattern = "[%p][%d{%m/%d/%y %H:%M:%S}][%l] [%t] - %m %n";
	std::string pattern = "[%-5p][%D{%Y-%m-%d %H:%M:%S.%Q %Z}][%l][%t] %m%n";
    std::auto_ptr<Layout> _layout(new PatternLayout(pattern));

    //_append->setLayout(std::auto_ptr<log4cplus::Layout>(new PatternLayout(pattern)));


//    std::auto_ptr<Layout> pTTCLayout(new TTCCLayout());
    _append->setLayout(_layout);

    LogUtils::_logger.addAppender(_append);

    LogUtils::_logger.setLogLevel(ALL_LOG_LEVEL);
    return true;
}
#ifdef DEBUG_LOG
int main(){
	LogUtils::instance().setLogName("./log/app.log");
	LogUtils::instance().open_log();
	NOTICE("hello world");
	return 0;
}
#endif
