#ifndef CC_EXA_WORKER_HEADER
#define CC_EXA_WORKER_HEADER

// Qt
#include <QString>
#include <QStringList>
#include <QThread>
#include <QProcess>

// CloudCompare
#include <ccOctree.h>

// Exametrics
#include "ccExaLog.h"

// Octree proportionnal to plan tolerance
#define K_CELL_TOLERANCE 0.05 // maxCellLength = 5%(tolerance)

/* Working class that will be instancied as a Thread.
   Used to some work (intersection, octree travel) in parallel of the GUI */
class ExaWorker : public QObject
{
    Q_OBJECT
    
public:
	/* Default constructor */
	explicit ExaWorker();

protected slots:
	/* Execute python script */
	void doPythonWork(QStringList arguments, ExaLog* logger);
	/* Execute octree work */
	void doOctreeWork(ccOctree::Shared octree, double tolerance, ExaLog* logger);


protected:

	QThread workerThread;

signals:
	/* Emitted when python is done  */
    void pythonResultReady(const QString &result);
    /* Emitted when the octree level has been computed */
    void octreeLevelReady(const unsigned int &level);
    /* Emitted when octree work is done */
    void octreeResultReady(const QString &result);

};

#endif
 
