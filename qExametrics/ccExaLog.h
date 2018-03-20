#ifndef CC_EXA_LOG_HEADER
#define CC_EXA_LOG_HEADER

// Qt
#include <QString>

// CloudCompare plugin
#include "../ccStdPluginInterface.h"

/* This class is used to easily send messages to CloudCompare's log console.
   All messages will have "[ccExametrics] " as prefix. */
class ExaLog
{
public:
	explicit ExaLog(ccMainAppInterface* app);

	/* Write the string s to the log console as information */
	void logInfo(QString s);
	/* Write the string s to the log console as warning */
	void logWarn(QString s);
	/* Write the string s to the log console as error */
	void logError(QString s);

protected:
	ccMainAppInterface* m_app = nullptr;

};

#endif 
