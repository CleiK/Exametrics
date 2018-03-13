#include "ccExaWorker.h" 


ExaWorker::ExaWorker()
{

}

void ExaWorker::doWork(QStringList arguments, ExaLog* logger)
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


	emit resultReady("Done");
}
