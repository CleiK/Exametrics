#ifndef CC_EXA_WORKER_HEADER
#define CC_EXA_WORKER_HEADER

#include <ccOctree.h>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QProcess>
#include "ccExaLog.h"

// Octree proportionnal to plan tolerance
#define K_CELL_TOLERANCE 0.05 // maxCellLength = 5%(tolerance)

class ExaWorker : public QObject
{
    Q_OBJECT
    
public:
	explicit ExaWorker();

protected slots:
	void doPythonWork(QStringList arguments, ExaLog* logger);
	void doOctreeWork(ccOctree::Shared octree, double tolerance, ExaLog* logger);


protected:

	QThread workerThread;

signals:
    void pythonResultReady(const QString &result);
    void octreeLevelReady(const unsigned int &level);
    void octreeResultReady(const QString &result);

};

#endif
 
