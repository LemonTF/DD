#ifndef UTILS_LOGUTILS_H_
#define UTILS_LOGUTILS_H_

#include "log4cplus/loglevel.h"
#include "log4cplus/ndc.h"
#include "log4cplus/logger.h"
#include "log4cplus/configurator.h"
#include "iomanip"
#include "log4cplus/fileappender.h"
#include "log4cplus/layout.h"
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;
using namespace log4cplus::helpers;
//日志封装
#define TRACE(p) LOG4CPLUS_TRACE(LogUtils::_logger, p)
#define DEBUG(p) LOG4CPLUS_DEBUG(LogUtils::_logger, p)
#define NOTICE(p) LOG4CPLUS_INFO(LogUtils::_logger, p)
#define WARNING(p) LOG4CPLUS_WARN(LogUtils::_logger, p)
#define FATAL(p) LOG4CPLUS_ERROR(LogUtils::_logger, p)

// 日志控制类，全局共用一个日志
class LogUtils
{
public:
	bool open_log();
	static LogUtils& instance();
	static Logger _logger;
	void setLogName(const std::string &s);
private:
	LogUtils();
	LogUtils(const LogUtils&)=delete;
	LogUtils & operator=(const LogUtils&)=delete;
	~LogUtils();
	std::string m_logName;
};


#endif /* UTILS_LOGUTILS_H_ */
