#ifndef __LOG_H__
#define __LOG_H__

#include <stdarg.h>

class Log
{

public:
	static void info(const char *message, ...); // log an info message
	static void warning(const char *message, ...); // log a warning message
	static void error(const char *message, ...); // log an error message
	static void log(const char *type, const char *message, va_list args); // log a message with given type

};

#endif
