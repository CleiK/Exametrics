#ifndef CC_EXA_LOG_HEADER
#define CC_EXA_LOG_HEADER


#include <QString>
#include "../ccStdPluginInterface.h"


class ExaLog
{
public:
	explicit ExaLog(ccMainAppInterface* app);

	void logInfo(QString s);
	void logWarn(QString s);
	void logError(QString s);

protected:
	ccMainAppInterface* m_app = nullptr;

};

#endif 
