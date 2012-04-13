#include "Log.h"

#include <stdio.h>

void Log::info(const char *message, ...)
{
	va_list args;
	va_start(args, message);
	Log::log("INFO", message, args);
}

void Log::warning(const char *message, ...)
{
	va_list args;
	va_start(args, message);
	Log::log("WARNING", message, args);
}

void Log::error(const char *message, ...)
{
	va_list args;
	va_start(args, message);
	Log::log("ERROR", message, args);
}

void Log::log(const char *type, const char *message, va_list args)
{
	printf("[%s] ", type);
	vprintf(message, args);
	printf("\n");
	va_end(args);
}
