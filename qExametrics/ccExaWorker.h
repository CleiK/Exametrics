#ifndef CC_EXA_WORKER_HEADER
#define CC_EXA_WORKER_HEADER

#include <QString>
#include <QStringList>
#include <QThread>
#include <QProcess>
#include "ccExaLog.h"

class ExaWorker : public QObject
{
    Q_OBJECT
    
public:
	explicit ExaWorker();

protected slots:
	void doWork(QStringList arguments, ExaLog* logger);


protected:

	QThread workerThread;

signals:
    void resultReady(const QString &result);

};

#endif
 
