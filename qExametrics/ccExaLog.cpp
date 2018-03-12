#include "ccExaLog.h"

ExaLog::ExaLog(ccMainAppInterface* app)
{
	this->m_app = app;
}

void ExaLog::logInfo(QString s)
{
    this->m_app->dispToConsole("[ccExametrics] " + s, ccMainAppInterface::STD_CONSOLE_MESSAGE);
}

void ExaLog::logWarn(QString s)
{
    this->m_app->dispToConsole("[ccExametrics] " + s, ccMainAppInterface::WRN_CONSOLE_MESSAGE);
}

void ExaLog::logError(QString s)
{
    this->m_app->dispToConsole("[ccExametrics] " + s, ccMainAppInterface::ERR_CONSOLE_MESSAGE);
} 
