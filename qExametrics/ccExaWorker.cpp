#include "ccExaWorker.h"


ExaWorker::ExaWorker()
{

}

void ExaWorker::doPythonWork(QStringList arguments, ExaLog* logger)
{
    // Executing python intersection script
    QProcess intersectionProcess;

    //logger->logInfo("python " + arguments.join(" "));
    intersectionProcess.start("python", arguments);

    logger->logInfo("Creating intersection...");
    intersectionProcess.waitForFinished();

    int exitCode = intersectionProcess.exitCode();
    if(exitCode != 0)
    {
        logger->logWarn("An error occured while creating an intersection output file.");
        logger->logWarn(intersectionProcess.readAllStandardError());
    }
    else
    {
        logger->logInfo(intersectionProcess.readAllStandardOutput());
    }


	emit pythonResultReady("Python");
}

void ExaWorker::doOctreeWork(ccOctree::Shared octree, double tolerance, ExaLog* logger)
{
	// find octree level

	float maxCellLength = K_CELL_TOLERANCE * tolerance;

    int level = 1;
    float cellLength = octree->getCellSize(level);

    bool maxSubLevel = false;
    while( (cellLength > maxCellLength) && (!maxSubLevel) )
    {
        level++;
        cellLength = octree->getCellSize(level);

        if(level == 10)
        {
            maxSubLevel = true;
        }
    }

    emit octreeLevelReady(level);

    // search intersection

	emit octreeResultReady("Octree");

}
