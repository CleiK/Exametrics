
//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qExametrics                 #
//#                                                                        #
//##########################################################################

#include "ccExametrics.h"
#include "ccExametricsDialog.h"

//Qt
#include <QtGui>

/* Default constructor */
ccExametrics::ccExametrics(QObject* parent/*=0*/)
    : QObject(parent)
    , m_action(0)
{
}

/* Deconstructor */
ccExametrics::~ccExametrics()
{
    if (m_dlg)
        delete m_dlg;

}


//This method should enable or disable each plugin action
//depending on the currently selected entities ('selectedEntities').
//For example: if none of the selected entities is a cloud, and your
//plugin deals only with clouds, call 'm_action->setEnabled(false)'
void ccExametrics::onNewSelection(const ccHObject::Container& selectedEntities)
{

    //m_action->setEnabled(m_app && m_app->dbRootObject() && m_app->dbRootObject()->getChildrenNumber() != 0);

    /*Enable EXAMETREICS Plugin only when you select a point cloud */
    m_action->setEnabled(selectedEntities.size()==1 && selectedEntities[0]->isA(CC_TYPES::POINT_CLOUD));

    if (!m_dlg)
    {
        return; //not initialized yet - ignore callback
    }
}

//This method returns all 'actions' of your plugin.
//It will be called only once, when plugin is loaded.
void ccExametrics::getActions(QActionGroup& group)
{
    //default action (if it has not been already created, it's the moment to do it)
    if (!m_action)
    {
        //here we use the default plugin name, description and icon,
        //but each action can have its own!
        m_action = new QAction(getName(),this);
        m_action->setToolTip(getDescription());
        m_action->setIcon(getIcon());
        //connect appropriate signal
        connect(m_action, SIGNAL(triggered()), this, SLOT(doAction()));
    }

    group.addAction(m_action);


    this->logger = new ExaLog(m_app);

    logger->logInfo("Plugin succesfully initialized!");
}

//This is an example of an action's slot called when the corresponding action
//is triggered (i.e. the corresponding icon or menu entry is clicked in CC's
//main interface). You can access to most of CC components (database,
//3D views, console, etc.) via the 'm_app' attribute (ccMainAppInterface
//object).
void ccExametrics::doAction()
{
    //m_app should have already been initialized by CC when plugin is loaded!
    //(--> pure internal check)
    assert(m_app);
    if (!m_app)
        return;

    /*** HERE STARTS THE ACTION ***/

    //check valid window
    if (!m_app->getActiveGLWindow())
    {
        logger->logError("Could not find valid 3D window.");
        return;
    }

    //bind GUI events
    m_dlg = new ccExametricsDialog((QWidget*)m_app->getMainWindow());

    // initialize parameters widgets with the object in db tree
    this->rootLasFile = m_app->dbRootObject()->getChild(0);
    // for random values
    srand (time(NULL));
    initializeParameterWidgets();

    //general
    ccExametricsDialog::connect(m_dlg->computeButton, SIGNAL(clicked()), this, SLOT(onCompute()));
    ccExametricsDialog::connect(m_dlg->closeButton, SIGNAL(clicked()), this, SLOT(onClose()));
    //parameter changed
    ccExametricsDialog::connect(m_dlg->spbXA, SIGNAL(valueChanged(double)), this, SLOT(onSpbXAChanged(double)));
    ccExametricsDialog::connect(m_dlg->spbYA, SIGNAL(valueChanged(double)), this, SLOT(onSpbYAChanged(double)));
    ccExametricsDialog::connect(m_dlg->spbZA, SIGNAL(valueChanged(double)), this, SLOT(onSpbZAChanged(double)));
    ccExametricsDialog::connect(m_dlg->spbXB, SIGNAL(valueChanged(double)), this, SLOT(onSpbXBChanged(double)));
    ccExametricsDialog::connect(m_dlg->spbYB, SIGNAL(valueChanged(double)), this, SLOT(onSpbYBChanged(double)));
    ccExametricsDialog::connect(m_dlg->spbZB, SIGNAL(valueChanged(double)), this, SLOT(onSpbZBChanged(double)));
    ccExametricsDialog::connect(m_dlg->coefSlider, SIGNAL(valueChanged(int)), this, SLOT(onCoefSliderChanged(int)));
    ccExametricsDialog::connect(m_dlg->spbCoef, SIGNAL(valueChanged(int)), this, SLOT(onCoefSpinBoxChanged(int)));
    ccExametricsDialog::connect(m_dlg->toleranceSpb, SIGNAL(valueChanged(double)), this, SLOT(onToleranceSpbChanged(double)));


    // initialize polylines etc
    initializeDrawSettings();


    m_dlg->linkWith(m_app->getActiveGLWindow());

    //start GUI
    m_app->registerOverlayDialog(m_dlg, Qt::TopRightCorner);
    m_dlg->start();
}

/* Called when the plugin is being stopped */
void ccExametrics::stop()
{
    //remove click listener
    /*if (m_app->getActiveGLWindow())
    {
    	m_app->getActiveGLWindow()->removeEventFilter(this);
    }*/

    //remove overlay GUI
    if (m_dlg)
    {
        m_dlg->stop(true);
        m_app->unregisterOverlayDialog(m_dlg);
    }

    //redraw
    if (m_app->getActiveGLWindow())
    {
        m_app->getActiveGLWindow()->redraw(true, false);
    }

    workerThread.quit();
    workerThread.wait();

    m_dlg = nullptr;


    ccStdPluginInterface::stop();
}

/* Return the plugin icon */
QIcon ccExametrics::getIcon() const
{
    return QIcon(":/CC/plugin/qExametrics/img/exametrics_icon.png");
}

void ccExametrics::setGifLoading(bool enabled)
{
    if(enabled)
    {
        if(!loadingGifMovie)
        {
            loadingGifMovie = new QMovie(":/CC/plugin/qExametrics/img/loading_2.gif");
            m_dlg->lblLoading->setMovie(loadingGifMovie);
        }

        loadingGifMovie->start();
        m_dlg->lblLoading->setVisible(true);
    }
    else
    {
        loadingGifMovie->stop();
        m_dlg->lblLoading->setVisible(false);
    }
}

/* Slot on Compute button click */
void ccExametrics::onCompute()
{
    setGifLoading(true);
    // All algorithms are executed in a separated Thread
    QString algo = m_dlg->cmbAlgo->currentText();
    if(algo == "Octree")
    {
        this->logger->logInfo("Generating octree.");
        ccGenericPointCloud* cloud = static_cast<ccGenericPointCloud*>(this->rootLasFile->getChild(0));

        octree = cloud->getOctree();
        if(!octree)
        {
            octree = cloud->computeOctree();

/* Get a cell center in the main cloud */
            CCVector3 center;
            Tuple3i pos;
            this->newCloud = new ccPointCloud();
            const CCVector3 *thePoint = cloud->getPoint(25000);
            this->octree->getTheCellPosWhichIncludesThePoint(thePoint,pos,6);
            this->octree->computeCellCenter(pos,6,center);
            this->newCloud->reserve(1);
            this->newCloud->addPoint(center);
            this->centrePoint2DLabel = new cc2DLabel("Cell center");
            this->centrePoint2DLabel->setVisible(true);
            this->centrePoint2DLabel->setDisplay(m_app->getActiveGLWindow());
            centrePoint2DLabel->addPoint(newCloud, newCloud->size() - 1);
/* end of getting cell center of the point number X */




/* crop and extract data*/
            ccPointCloud* cloudx = static_cast<ccPointCloud*>(this->rootLasFile->getChild(0));
            ccGenericPointCloud* boxCloud = this->box->getAssociatedCloud();
            ccBBox bbox = boxCloud->getBB_recursive();
            CCLib::ReferenceCloud* selection =cloudx->crop(bbox,true);
    if (!selection)
		{
			//process failed!
			ccLog::Warning(QString("[Crop] Failed to crop cloud '%1'!").arg(cloudx->getName()));
        }
		//crop
            ccPointCloud* croppedEnt = cloudx->partialClone(selection);
            delete selection;
            selection = 0;
            this->m_exametricsGroup->addChild(croppedEnt);
/* end of crop*/
        }




        // configure worker that will do the recursive intersection in a thread
        if(!this->exaWorker)
        {
            //std::cout << "new exaworker\n";
            this->exaWorker = new ExaWorker();
            this->exaWorker->moveToThread(&this->workerThread);
            connect(&workerThread, SIGNAL(finished()), this->exaWorker, SLOT(deleteLater()));
        }
        qRegisterMetaType<ccOctree::Shared>("Shared");
        connect(this, SIGNAL(operateOctreeWorker(ccOctree::Shared, double, ExaLog*)), this->exaWorker, SLOT(doOctreeWork(ccOctree::Shared, double, ExaLog*)));
        connect(this->exaWorker, SIGNAL(octreeLevelReady(const unsigned int)), this, SLOT(octreeLevelReady(const unsigned int)));
        connect(this->exaWorker, SIGNAL(octreeResultReady(QString)), this, SLOT(workerDone(QString)));
        // Start thread
        this->workerThread.start();
        // Emit signal to start the work
        emit operateOctreeWorker(octree, this->getTolerance(), this->logger);
    }

    else if(algo == "Linear")
    {
        this->logger->logInfo("Generating intersection with python linear algorithm.");
        /* Get parameters for python script */

        // File name
        QString tmpFileName = this->rootLasFile->getName();
        int parenthesis = tmpFileName.indexOf('(');
        QString path = tmpFileName.right(tmpFileName.length() - parenthesis).remove('(').remove(')');
        QString fileName  = tmpFileName.left(parenthesis - 1).prepend('/').prepend(path);

        // Current cloud
        ccGenericPointCloud* boxCloud = this->box->getAssociatedCloud();


        // Retrieve box points and remove duplicates to get 8 points instead of 24
        bool duplicate = false;
        QList<const CCVector3*>* points = new QList<const CCVector3*>();
        float e = 0.0001;
        for(int i = 0; i < 24; i++)
        {
            const CCVector3* p1 = boxCloud->getPoint(i);
            //logInfo(Utils::ccVector3ToString(p1));
            for(int j = 0; j < points->length(); j++)
            {
                const CCVector3* p2 = points->at(j);

                if(p1 != p2)
                {
                    //std::cout << "p1 != p2 ";
                    if(Utils::double_equals(p1->x, p2->x, e) && Utils::double_equals(p1->y, p2->y, e) && Utils::double_equals(p1->z, p2->z, e))
                    {
                        //std::cout << "duplicate \n";
                        duplicate = true;
                    }
                }

                if(duplicate)
                    break;
            }

            if(!duplicate)
            {
                //std::cout << "add point " << i << "\n";
                points->append(p1);
            }

            duplicate = false;
        }

        // Display points
        if(DEBUG_BOX_POINTS)
        {
            // affichage des points (8 normalement)
            for(int i = 0; i < points->length(); i++)
            {
                const CCVector3* p = points->at(i);

                this->tmpPointCloudList->at(i)->clear();
                this->tmpPointCloudList->at(i)->reserve(1);
                this->tmpPointCloudList->at(i)->addPoint(CCVector3(p->x, p->y, p->z));
            }
        }

        // https://math.stackexchange.com/questions/1472049/check-if-a-point-is-inside-a-rectangular-shaped-area-3d
        const CCVector3* P1 = points->at(5);
        const CCVector3* P2 = points->at(3);
        const CCVector3* P4 = points->at(1);
        const CCVector3* P5 = points->at(0);

        // Arguments for python intersection script
        QStringList arguments = QStringList() << "/usr/local/lib/cloudcompare/plugins/exa.py" << fileName
                                              << QString::number(P1->x) << QString::number(P1->y) << QString::number(P1->z)
                                              << QString::number(P2->x) << QString::number(P2->y) << QString::number(P2->z)
                                              << QString::number(P4->x) << QString::number(P4->y) << QString::number(P4->z)
                                              << QString::number(P5->x) << QString::number(P5->y) << QString::number(P5->z);

        // configure worker that will do the python intersection in a thread
        if(!this->exaWorker)
        {
            //std::cout << "new exaworker\n";
            this->exaWorker = new ExaWorker();
            this->exaWorker->moveToThread(&this->workerThread);
            connect(&workerThread, SIGNAL(finished()), this->exaWorker, SLOT(deleteLater()));
        }

        connect(this, SIGNAL(operatePythonWorker(QStringList, ExaLog*)), this->exaWorker, SLOT(doPythonWork(QStringList, ExaLog*)));
        connect(this->exaWorker, SIGNAL(pythonResultReady(QString)), this, SLOT(workerDone(QString)));

        // Start thread
        this->workerThread.start();
        // Emit signal to start the work
        emit operatePythonWorker(arguments, this->logger);
    }

    /*// Plan equation
    CCVector3 normalVector;
    PointCoordinateType d = 0;
    this->pPlane->getEquation(normalVector, d);*/


}

// Called when the worker is done
void ccExametrics::workerDone(const QString s)
{
    this->logger->logInfo(s + " done!");
    setGifLoading(false);

    this->m_exametricsGroup->addChild(this->centrePoint2DLabel);
}

// Called when the worker has computed the desired octree level
void ccExametrics::octreeLevelReady(const unsigned int level)
{
    this->logger->logInfo("Displaying Octree with level " + QString::number(level));
    octree->setDisplayedLevel(level);

    this->octreeProxy = new ccOctreeProxy(octree, "Cloud Octree");
    this->octreeProxy->setDisplay(m_app->getActiveGLWindow());
    this->octreeProxy->setVisible(true);

    this->m_exametricsGroup->addChild(octreeProxy);
}


/* Slot on dialog closed */
void ccExametrics::onClose()
{
    stop();
    // also removes childs of this->m_exametricsGroup
    m_app->removeFromDB(this->m_exametricsGroup, true);
}

/* Initialize plan parameters at random values with min and max limits */
void ccExametrics::initializeParameterWidgets()
{
    logger->logInfo("Initializing parameters widgets with informations from \"" + this->rootLasFile->getName() + "\"");

    ccBBox box = this->rootLasFile->getBB_recursive();
    CCVector3 minCorner = box.minCorner();
    CCVector3 maxCorner = box.maxCorner();

    float xBox = 0, yBox = 0, zBox = 0;
    xBox = maxCorner.x - minCorner.x;
    yBox = maxCorner.y - minCorner.y;
    zBox = maxCorner.z - minCorner.z;

    this->m_boxXWidth = xBox;
    this->m_boxYWidth = yBox;


    m_dlg->spbXA->setMinimum(minCorner.x);
    m_dlg->spbYA->setMinimum(minCorner.y);
    m_dlg->spbZA->setMinimum(minCorner.z);
    m_dlg->spbXA->setMaximum(maxCorner.x);
    m_dlg->spbYA->setMaximum(maxCorner.y);
    m_dlg->spbZA->setMaximum(maxCorner.z);

    m_dlg->spbXB->setMinimum(minCorner.x);
    m_dlg->spbYB->setMinimum(minCorner.y);
    m_dlg->spbZB->setMinimum(minCorner.z);
    m_dlg->spbXB->setMaximum(maxCorner.x);
    m_dlg->spbYB->setMaximum(maxCorner.y);
    m_dlg->spbZB->setMaximum(maxCorner.z);

    m_dlg->spbXA_2->setMinimum(minCorner.x);
    m_dlg->spbYA_2->setMinimum(minCorner.y);
    m_dlg->spbZA_2->setMinimum(minCorner.z);
    m_dlg->spbXA_2->setMaximum(maxCorner.x);
    m_dlg->spbYA_2->setMaximum(maxCorner.y);
    m_dlg->spbZA_2->setMaximum(maxCorner.z);

    m_dlg->spbXB_2->setMinimum(minCorner.x);
    m_dlg->spbYB_2->setMinimum(minCorner.y);
    m_dlg->spbZB_2->setMinimum(minCorner.z);
    m_dlg->spbXB_2->setMaximum(maxCorner.x);
    m_dlg->spbYB_2->setMaximum(maxCorner.y);
    m_dlg->spbZB_2->setMaximum(maxCorner.z);

    // random vector
    m_dlg->spbXA->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbYA->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbZA->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbXB->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbYB->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbZB->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));

    m_dlg->spbXA_2->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbYA_2->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbZA_2->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbXB_2->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbYB_2->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbZB_2->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));

    // vector point distance coefficient
    m_dlg->coefSlider->setValue(COEF_INIT);
    m_dlg->spbCoef->setValue(COEF_INIT);

    m_dlg->coefSlider_2->setValue(COEF_INIT);
    m_dlg->spbCoef_2->setValue(COEF_INIT);

    this->m_coef = COEF_INIT;

    // tolerance
    m_dlg->toleranceSpb->setValue(TOLERANCE_INIT);
    m_dlg->toleranceSpb_2->setValue(TOLERANCE_INIT);

    m_dlg->tabP2->setEnabled(false);
    m_dlg->rdb2Plan->setEnabled(false);

    // algorithm selection
    QStringList algos;
    algos << "Octree" << "Linear";
    m_dlg->cmbAlgo->addItems(algos);
}

/* Initialize draw settings for normalized vector, point and plan display */
void ccExametrics::initializeDrawSettings()
{
    // Create directory "Exametrics" in DB tree
    this->m_exametricsGroup = new ccHObject("Exametrics");
    m_app->addToDB(this->m_exametricsGroup, false, true, false, false);

    if(DEBUG_BOX_POINTS || DEBUG_PLAN)
    {
        if(DEBUG_BOX_POINTS && DEBUG_PLAN)
        {
            logger->logInfo("Debug enabled for box points and positionning plan.");
        }
        else if(DEBUG_BOX_POINTS)
        {
            logger->logInfo("Debug enabled for box points.");
        }
        else
        {
            logger->logInfo("Debug enabled for positionning plan.");
        }
    }


    if(DEBUG_BOX_POINTS)
    {
        tmpPointCloudList = new QList<ccPointCloud*>();
        tmpPointList = new QList<cc2DLabel*>();

        for(int i = 0; i < N_point; i++)
        {
            tmpPointCloudList->append(new ccPointCloud("Cloud Point " + QString::number(i)));
            tmpPointCloudList->at(i)->reserve(1);
            tmpPointCloudList->at(i)->addPoint(CCVector3(0,0,0));

            tmpPointList->append(new cc2DLabel("Point " + QString::number(i)));
            tmpPointList->at(i)->setVisible(true);
            tmpPointList->at(i)->setDisplay(m_app->getActiveGLWindow());
            tmpPointList->at(i)->addPoint(tmpPointCloudList->at(i), tmpPointCloudList->at(i)->size() - 1);

            this->m_exametricsGroup->addChild(tmpPointList->at(i));
        }
    }


    /* Normalized vector */
    this->initVector();

    /* Vector point */
    this->initPoint();

    /* Plan */
    this->updatePlan();

    /* Box */
    this->updateBox();

    /* Update Graphics*/
    this->canUpdateGraphics = true;
}

// Init vector display
void ccExametrics::initVector()
{
    // new 2 points cloud
    this->normalizedVectorCloud = new ccPointCloud("Normalized vector");

    // reserve 2 points
    this->normalizedVectorCloud->reserve(2);

    // add points
    this->normalizedVectorCloud->addPoint(Utils::ccVectorDoubleToFloat(getNormalizedVectorPointA()));
    this->normalizedVectorCloud->addPoint(Utils::ccVectorDoubleToFloat(getNormalizedVectorPointB()));

    // create the polyline with the 2 points cloud
    this->normalizedVectorPoly = new ccPolyline(this->normalizedVectorCloud);
    // connect the two first points of the cloud
    this->normalizedVectorPoly->addPointIndex(0,2);
    // color
    ccColor::Rgb lineColor = ccColor::red ;
    this->normalizedVectorPoly->setColor(lineColor);
    this->normalizedVectorPoly->showColors(true);
    this->normalizedVectorPoly->setWidth(2.0);
    // where to display
    this->normalizedVectorPoly->setDisplay(m_app->getActiveGLWindow());
    // save in DB tree
    this->m_exametricsGroup->addChild(this->normalizedVectorPoly);
}

// Update vector display
void ccExametrics::updateVector()
{
    if(!this->normalizedVectorCloud)
        return;

    // clear old points
    this->normalizedVectorCloud->clear();

    // reserve 2 points
    this->normalizedVectorCloud->reserve(2);

    // add points
    this->normalizedVectorCloud->addPoint(Utils::ccVectorDoubleToFloat(getNormalizedVectorPointA()));
    this->normalizedVectorCloud->addPoint(Utils::ccVectorDoubleToFloat(getNormalizedVectorPointB()));
}

// Init vector point display
void ccExametrics::initPoint()
{
    this->vectorPointCloud = new ccPointCloud("Vector point");
    this->vectorPointCloud->reserve(1);

    CCVector3d pointA = getNormalizedVectorPointA();
    CCVector3d pointB = getNormalizedVectorPointB();
    double k = (double)this->m_coef / 100;

    this->m_vectorPoint = CCVector3d(k * (pointB.x - pointA.x) + pointA.x, k * (pointB.y - pointA.y) + pointA.y, k * (pointB.z - pointA.z) + pointA.z);

    this->vectorPointCloud->addPoint(Utils::ccVectorDoubleToFloat(getVectorPoint()));

    this->vectorPoint2DLabel = new cc2DLabel("Vector point");
    this->vectorPoint2DLabel->setVisible(true);
    this->vectorPoint2DLabel->setDisplay(m_app->getActiveGLWindow());

    this->vectorPoint2DLabel->addPoint(this->vectorPointCloud, this->vectorPointCloud->size() - 1);

    this->m_exametricsGroup->addChild(this->vectorPoint2DLabel);
}

// Update vector point display
void ccExametrics::updatePoint()
{
    if(!this->vectorPointCloud)
        return;

    this->vectorPointCloud->clear();
    this->vectorPointCloud->reserve(1);

    // calcul du point en utilisant le vecteur
    CCVector3d pointA = getNormalizedVectorPointA();
    CCVector3d pointB = getNormalizedVectorPointB();
    double k = (double)this->m_coef / 100;

    this->m_vectorPoint = CCVector3d(k * (pointB.x - pointA.x) + pointA.x, k * (pointB.y - pointA.y) + pointA.y, k * (pointB.z - pointA.z) + pointA.z);

    this->vectorPointCloud->addPoint(Utils::ccVectorDoubleToFloat(getVectorPoint()));

}

// Update plan display
void ccExametrics::updatePlan()
{
    if(!this->planCloud)
    {
        this->planCloud = new ccPointCloud("Plan");
    }

    if(planeIsInDBTree && this->pPlane)
    {
        this->m_exametricsGroup->removeChild(this->pPlane);

        planeIsInDBTree = false;
    }


    // Generate Orthogonal vectors to create our plan (4 vectors)
    CCVector3d Mediatrice = getVectorMediator();
    CCVector3d N = getVectorPoint() - getNormalizedVectorPointA();

    // clear points
    this->planCloud->clear();
    // reserve 4 points in memory
    this->planCloud->reserve(4);

    // assign points
    CCVector3d planPoint = CCVector3d(0,-N.z, N.y);
    CCVector3d planPoint1 = CCVector3d(-N.z, 0, N.x);
    CCVector3d planPoint2 = CCVector3d(N.y, -N.x, 0);
    CCVector3d planPoint3 = CCVector3d(Mediatrice.x, Mediatrice.y, Mediatrice.z);

    this->planCloud->addPoint(Utils::ccVectorDoubleToFloat(planPoint));
    this->planCloud->addPoint(Utils::ccVectorDoubleToFloat(planPoint1));
    this->planCloud->addPoint(Utils::ccVectorDoubleToFloat(planPoint2));
    this->planCloud->addPoint(Utils::ccVectorDoubleToFloat(planPoint3));

    double rms = 0.0;
    // create plan by fit method on plancloud
    this->pPlane = ccPlane::Fit(this->planCloud, &rms);

    CCVector3d vectorPoint = this->getVectorPoint();
    CCVector3 planCenter = this->pPlane->getCenter();
    ccGLMatrix* transfo = new ccGLMatrix();
    CCVector3d translation = CCVector3d(vectorPoint.x - planCenter.x,
                                        vectorPoint.y - planCenter.y,
                                        vectorPoint.z - planCenter.z);

    transfo->setTranslation(translation);

    this->pPlane->applyGLTransformation_recursive(transfo);

    delete transfo;


    if(this->pPlane)
    {
        // setrgb ?
        pPlane->setColor(ccColor::yellow);
        pPlane->showColors(true);
        pPlane->showNormalVector(true);
        pPlane->setXWidth(this->m_boxXWidth);
        pPlane->setYWidth(this->m_boxYWidth);
        //make plane to add to display
        pPlane->setVisible(DEBUG_PLAN);
        //pPlane->setSelectionBehavior(ccHObject::SELECTION_IGNORED);

        pPlane->setDisplay(m_app->getActiveGLWindow());
        //pPlane->prepareDisplayForRefresh_recursive(); //not sure what this does, but it looks like fun

        this->m_exametricsGroup->addChild(this->pPlane);

        planeIsInDBTree = true;

    }
    else
    {
        logger->logError("Failed to create plane.");
    }
}

// Update box display
void ccExametrics::updateBox()
{
    //return;
    if(this->box)
    {
        this->m_exametricsGroup->removeChild(this->box);
    }

    // dimensions définissant la box
    CCVector3 dimensions = CCVector3(this->m_boxXWidth, this->m_boxYWidth, this->getTolerance());
    // Matrice liée
    ccGLMatrix matBox;

    //get the plane transformation to apply it for the box
    matBox = this->pPlane->getTransformation();


    this->box = new ccBox(dimensions, &matBox);

    if(this->box)
    {
        box->showNormals(false);
        box->setColor(ccColor::blue);
        box->showColors(true);
        box->enableStippling(false);
        box->getBB_recursive(true);

        //make box to add to display
		box->setVisible(true);
		box->setDisplay(m_app->getActiveGLWindow());
		this->m_exametricsGroup->addChild(this->box);
    }
}


/* Called when xA spinbox changed value */
void ccExametrics::onSpbXAChanged(double value)
{
    onParameterChanged(m_dlg->spbXA, value);
    onNormalizedVectorChanged();
}
/* Called when yA spinbox changed value */
void ccExametrics::onSpbYAChanged(double value)
{
    onParameterChanged(m_dlg->spbYA, value);
    onNormalizedVectorChanged();
}
/* Called when zA spinbox changed value */
void ccExametrics::onSpbZAChanged(double value)
{
    onParameterChanged(m_dlg->spbZA, value);
    onNormalizedVectorChanged();
}
/* Called when xB spinbox changed value */
void ccExametrics::onSpbXBChanged(double value)
{
    onParameterChanged(m_dlg->spbXB, value);
    onNormalizedVectorChanged();
}
/* Called when yB spinbox changed value */
void ccExametrics::onSpbYBChanged(double value)
{
    onParameterChanged(m_dlg->spbYB, value);
    onNormalizedVectorChanged();
}
/* Called when zB spinbox changed value */
void ccExametrics::onSpbZBChanged(double value)
{
    onParameterChanged(m_dlg->spbZB, value);
    onNormalizedVectorChanged();
}
/* Called when distance coefficient slider changed value */
void ccExametrics::onCoefSliderChanged(int value)
{
    onVectorPointChanged(value);
}
/* Called when distance coefficient spinbox changed value */
void ccExametrics::onCoefSpinBoxChanged(int value)
{
    onVectorPointChanged(value);
}

/* Called when tolerance spinbox changed value */
void ccExametrics::onToleranceSpbChanged(double value)
{
    onParameterChanged(m_dlg->toleranceSpb, value);

    if(!this->canUpdateGraphics)
        return;

    this->updateBox();
}



/* Called when the parameters of the normalized vector are changing */
void ccExametrics::onNormalizedVectorChanged()
{
    if(!this->canUpdateGraphics)
        return;

    this->updateVector();

    /* Compute vector point display */
    this->updatePoint();

    this->updatePlan();

    /* Then, compute box display */
    this->updateBox();
}

/* Called when the parameter of the vector point is changing */
void ccExametrics::onVectorPointChanged(int coef)
{
    this->m_coef = coef;
    //m_dlg->lblCoef->setText(QString::number(this->m_coef) + "%");
    m_dlg->coefSlider->setValue(this->m_coef);
    m_dlg->spbCoef->setValue(this->m_coef);

    if(!this->canUpdateGraphics)
        return;

    /* Compute vector point display */
    this->updatePoint();

    this->updatePlan();

    /* Compute box display */
    this->updateBox();
}

/* Called when a parameters of the plan is changing */
void ccExametrics::onParameterChanged(QWidget* w, double value)
{

}

// Get box tolerance
double ccExametrics::getTolerance()
{
    if(!m_dlg) return 0 ;
    return m_dlg->toleranceSpb->value();
}
/* Return the norm on X axis for the normalized vector */
double ccExametrics::getNormX()
{
    if(!m_dlg) /*warn("normX");*/ return 0;
    return abs(m_dlg->spbXB->value() - m_dlg->spbXA->value());
}
/* Return the norm on Y axis for the normalized vector */
double ccExametrics::getNormY()
{
    if(!m_dlg) /*warn("normY");*/ return 0;
    return abs(m_dlg->spbYB->value() - m_dlg->spbYA->value());
}
/* Return the norm on Z axis for the normalized vector */
double ccExametrics::getNormZ()
{
    if(!m_dlg) /*warn("normZ");*/ return 0;
    return abs(m_dlg->spbZB->value() - m_dlg->spbZA->value());
}
/* Return point A coordinates of the normalized vector */
CCVector3d ccExametrics::getNormalizedVectorPointA()
{
    if(!m_dlg) /*warn("pointA");*/ return CCVector3d(0,0,0);
    return CCVector3d(m_dlg->spbXA->value(), m_dlg->spbYA->value(), m_dlg->spbZA->value());
}
/* Return point B coordinates of the normalized vector */
CCVector3d ccExametrics::getNormalizedVectorPointB()
{
    if(!m_dlg) /*warn("pointB");*/ return CCVector3d(0,0,0);
    return CCVector3d(m_dlg->spbXB->value(), m_dlg->spbYB->value(), m_dlg->spbZB->value());
}
/* Return the normalized vector */
CCVector3d ccExametrics::getNormalizedVector()
{
    if(!m_dlg) /*warn("vector");*/ return CCVector3d(0,0,0);
    return CCVector3d(getNormX(), getNormY(), getNormZ());
}
/* Return the point on the vector (compute from coef distance) */
CCVector3d ccExametrics::getVectorPoint()
{
    return m_vectorPoint;
}
// Get the vector center coordinates
CCVector3d ccExametrics::getVectorCenter()
{
    CCVector3d A = getNormalizedVectorPointA();
    CCVector3d B = getNormalizedVectorPointB();
    return CCVector3d ( (A.x + B.x) / 2,
                        (A.y + B.y) / 2,
                        (A.z + B.z) / 2);
}
// Get the vector bisector
CCVector3d ccExametrics::getVectorMediator()
{
    CCVector3d AB = getNormalizedVectorPointB() - getNormalizedVectorPointA();
    return CCVector3d(getVectorCenter() * AB);
}
