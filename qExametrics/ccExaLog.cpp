#include "ccExaLog.h"

/* This class is used to easily send messages to CloudCompare's log console.
   All messages will have "[ccExametrics] " as prefix. */
ExaLog::ExaLog(ccMainAppInterface* app)
{
	this->m_app = app;
}

/* Write the string s to the log console as information */
void ExaLog::logInfo(QString s)
{
    this->m_app->dispToConsole("[ccExametrics] " + s, ccMainAppInterface::STD_CONSOLE_MESSAGE);
}

/* Write the string s to the log console as warning */
void ExaLog::logWarn(QString s)
{
    this->m_app->dispToConsole("[ccExametrics] " + s, ccMainAppInterface::WRN_CONSOLE_MESSAGE);
}

/* Write the string s to the log console as error */
void ExaLog::logError(QString s)
{
    this->m_app->dispToConsole("[ccExametrics] " + s, ccMainAppInterface::ERR_CONSOLE_MESSAGE);
} 
