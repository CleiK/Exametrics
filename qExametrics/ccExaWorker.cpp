#include "ccExaWorker.h"


ExaWorker::ExaWorker()
{
    //this->sharedMemory = QSharedMemory("QSharedMemoryTest");
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
    /*this->sharedMemory.attach();
    QBuffer buffer;
    QString msg;
    QDataStream in(&buffer);
    sharedMemory.lock();
    buffer.setData((char*)sharedMemory.constData(), sharedMemory.size());
    in >> msg;
    sharedMemory.unlock();

    sharedMemory.detach();

    std::cout <<  msg.toStdString() << "\n";


    std::cout << "1\n";
    wi->octree = wi->cloud->getOctree();
    std::cout << "2\n";
    if(!wi->octree)
    {
        std::cout << "3\n";
       wi->octree = wi->cloud->computeOctree();
    }*/

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

    //std::cout << "lala " << level << "\n";

    emit octreeLevelReady(level);

    //CCVector3 center;
    //const CCVector3* myPoint = octree->associatedCloud()->getPoint(1000);
    //Tuple3i cellPos;
    //octree->getTheCellPosWhichIncludesThePoint(myPoint, cellPos, level);

    //octree->computeCellCenter(cellPos, level, center);

    //td::cout << center.x << " " << center.y << " " << center.z << "\n";

   /* std::cout << "4\n";

    DistanceComputationTools::Cloud2MeshDistanceComputationParams params;
    params.octreeLevel = level;

    //GenericProgressCallback* progressCb = 0;

    std::cout << "4.1\n";

    int csize = wi->cloud->size();
    std::cout << "4.2\n";

    int resizeErr = sf->resize(csize);

    std::cout << "4.3 " << csize << "\n";
    if (!resizeErr)
    {
        std::cout << "4.4\n";
        logger->logError("Not enough memory");
        sf->release();
        return;
    }

    std::cout << "5\n";

    int sfIdx = cloud->addScalarField(sf);
    std::cout << "6\n";
    //make this SF 'active'
    cloud->setCurrentScalarField(sfIdx);

    std::cout << "7\n";
    int err = DistanceComputationTools::computeCloud2MeshDistance(cloud, box, params, 0, 0);

    logger->logInfo("ExaWorker err: " + QString::number(err));

    if(err == 0)
    {
        sf->computeMinAndMax();
    }
    else
    {
        //if an error occurred, this SF is useless
        cloud->deleteScalarField(sfIdx);
    }






    // search intersection*/



	emit octreeResultReady("Octree");
}
