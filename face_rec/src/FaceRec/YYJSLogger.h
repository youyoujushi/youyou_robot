#pragma once

enum LOGGER_LEVEL
{
	LOG_TRACE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL
};

class CYYJSLogger
{
public:
	CYYJSLogger(void);
	~CYYJSLogger(void);

	/**
	*	д��־���� 
	*	@param	msg		��־����
	*	@param	level	��־����
	*/
	void log(char* msg, LOGGER_LEVEL level);
};

