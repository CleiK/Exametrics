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
    //disable the main plugin icon if no entity is loaded
    m_action->setEnabled(m_app && m_app->dbRootObject() && m_app->dbRootObject()->getChildrenNumber() != 0);

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

    this->logInfo("Plugin initialized successfully.");
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
        this->logError("Could not find valid 3D window.");
        return;
    }

    //bind GUI events
    m_dlg = new ccExametricsDialog((QWidget*)m_app->getMainWindow());

    // initialize parameters widgets with the object in db tree
    this->rootLasFile = m_app->dbRootObject()->getChild(0);
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

    m_dlg = nullptr;

    ccStdPluginInterface::stop();
}

/* Return the plugin icon */
QIcon ccExametrics::getIcon() const
{
    return QIcon(":/CC/plugin/qExametrics/exametrics_icon.png");
}

/* Slot on Compute button click */
void ccExametrics::onCompute()
{
    this->logInfo("Compute!");

    //get point cloud
    ccPointCloud* cloud = static_cast<ccPointCloud*>(m_app->dbRootObject()); //cast to point cloud

    // Plan equation
    CCVector3 normalVector;
    PointCoordinateType d = 0;
    this->pPlane->getEquation(normalVector, d);

    // File name
    QString fileName = this->rootLasFile->getName();

    ccGenericPointCloud* boxCloud = this->box->getAssociatedCloud();
    for(int i = 0; i < 24; i++)
    {
        logInfo(Utils::ccVector3ToString(boxCloud->getPoint(i)));
    }

    /*logInfo("Plan equation: " + QString::number(normalVector.x) + "x + "
            + QString::number(normalVector.y) + "y + "
            + QString::number(normalVector.z) + "z + "
            + QString::number(d));
    logInfo("File name: " + fileName);
    logInfo("Cloud limits: " + QString::number(this->m_boxXWidth)
            + " x " + QString::number(this->m_boxYWidth)
            + " Tolerance: " + QString::number(this->getTolerance()));*/

    // 2 points

    /*if (!cloud)
    {
        this->logInfo("not a cloud :(");
        return;
    }
    else
    {
        this->logInfo("is a cloud :)");
    }*/
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
    this->logInfo("Initializing parameters widgets with informations from \"" + this->rootLasFile->getName() + "\"");

    ccBBox box = this->rootLasFile->getBB_recursive();
    CCVector3 minCorner = box.minCorner();
    CCVector3 maxCorner = box.maxCorner();

    float xBox = 0, yBox = 0, zBox = 0;
    xBox = maxCorner.x - minCorner.x;
    yBox = maxCorner.y - minCorner.y;
    zBox = maxCorner.z - minCorner.z;

    this->m_boxXWidth = xBox;
    this->m_boxYWidth = yBox;


    m_dlg->spbXA->setMinimum(minCorner.x); // minCorner.x
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

    // random vector
    m_dlg->spbXA->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbYA->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbZA->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbXB->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbYB->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
    m_dlg->spbZB->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));

    // vector point distance coefficient
    m_dlg->coefSlider->setValue(COEF_INIT);
    m_dlg->lblCoef->setText(QString::number(COEF_INIT) + "%");
    this->m_coef = COEF_INIT;

    // tolerance
    m_dlg->toleranceSpb->setValue(TOLERANCE_INIT);
}

/* Initialize draw settings for normalized vector, point and plan display */
void ccExametrics::initializeDrawSettings()
{
    // Create directory "Exametrics" in DB tree
    this->m_exametricsGroup = new ccHObject("Exametrics");
    m_app->addToDB(this->m_exametricsGroup, false, true, false, false);


    /* Normalized vector */
    this->initVector();

    /* Vector point */
    this->initPoint();

    /* Plan */
    // no init method because the plan creation is a static method
    this->updatePlan();

    this->updateBox();

    this->canUpdateGraphics = true;
}


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
    // where to display
    this->normalizedVectorPoly->setDisplay(m_app->getActiveGLWindow());
    // save in DB tree
    this->m_exametricsGroup->addChild(this->normalizedVectorPoly);
}

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
        pPlane->setColor(ccColor::blue);
        pPlane->showColors(true);
        pPlane->showNormalVector(true);
        pPlane->setXWidth(this->m_boxXWidth);
        pPlane->setYWidth(this->m_boxYWidth);

        //make plane to add to display
        pPlane->setVisible(true);
        //pPlane->setSelectionBehavior(ccHObject::SELECTION_IGNORED);

        pPlane->setDisplay(m_app->getActiveGLWindow());
        //pPlane->prepareDisplayForRefresh_recursive(); //not sure what this does, but it looks like fun

        this->m_exametricsGroup->addChild(this->pPlane);

        planeIsInDBTree = true;
    }
    else
    {
        this->logError("Failed to create plane.");
    }
}

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
    // Création de la box
    ccBox* tmpBox = new ccBox(dimensions, &matBox, "tmpBox");
    // ClipBox associé
    ccClipBox* associatedClipBox = new ccClipBox("tmpBox");
    // Center
    CCVector3 boxCenter = associatedClipBox->getBox().getCenter();

    // Delete tmp variable
    delete tmpBox;
    delete associatedClipBox;

    CCVector3d vectorPoint = this->getVectorPoint();
    CCVector3d pointA = this->getNormalizedVectorPointA();
    CCVector3 boxTranslation = CCVector3(vectorPoint.x - boxCenter.x,
                                         vectorPoint.y - boxCenter.y,
                                         vectorPoint.z - boxCenter.z);

    CCVector3d boxVect = vectorPoint - pointA;

    matBox = ccGLMatrix(CCVector3(0                       , 50 * -boxVect.z / m_coef, 50 * boxVect.y / m_coef), // colonne 0 rotation x
                        CCVector3(50 * -boxVect.z / m_coef, 0                       , 50 * boxVect.x / m_coef), // colonne 1 rotation y
                        CCVector3(0                       , 0                       , 1                      ), // colonne 2 rotation z
                        boxTranslation);                                                                        // translation

    logInfo(Utils::ccVector3ToString(boxVect));

    this->box = new ccBox(dimensions, &matBox);

    if(this->box)
    {
        box->showNormals(false);
        box->setColor(ccColor::blue);
        box->showColors(true);
        box->enableStippling(true);

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

/* Called when tolerance spinbox changed value */
void ccExametrics::onToleranceSpbChanged(double value)
{
    onParameterChanged(m_dlg->toleranceSpb, value);
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

    /* Then, compute plan display */
    this->updatePlan();


    this->updateBox();
}

/* Called when the parameter of the vector point is changing */
void ccExametrics::onVectorPointChanged(int coef)
{
    this->m_coef = coef;
    m_dlg->lblCoef->setText(QString::number(this->m_coef) + "%");

    if(!this->canUpdateGraphics)
        return;

    /* Compute vector point display */
    this->updatePoint();

    /* Then, compute plan display */
    this->updatePlan();

    /* Compute box display */
    this->updateBox();
}

/* Called when a parameters of the plan is changing */
void ccExametrics::onParameterChanged(QWidget* w, double value)
{

}


void ccExametrics::logInfo(QString s)
{
    m_app->dispToConsole("[ccExametrics] " + s, ccMainAppInterface::STD_CONSOLE_MESSAGE);
}

void ccExametrics::logWarn(QString s)
{
    m_app->dispToConsole("[ccExametrics] " + s, ccMainAppInterface::WRN_CONSOLE_MESSAGE);
}

void ccExametrics::logError(QString s)
{
    m_app->dispToConsole("[ccExametrics] " + s, ccMainAppInterface::ERR_CONSOLE_MESSAGE);
}

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
/* Return point A of the normalized vector */
CCVector3d ccExametrics::getNormalizedVectorPointA()
{
    if(!m_dlg) /*warn("pointA");*/ return CCVector3d(0,0,0);
    return CCVector3d(m_dlg->spbXA->value(), m_dlg->spbYA->value(), m_dlg->spbZA->value());
}
/* Return point B of the normalized vector */
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

CCVector3d ccExametrics::getVectorCenter()
{
    CCVector3d A = getNormalizedVectorPointA();
    CCVector3d B = getNormalizedVectorPointB();
    return CCVector3d ( (A.x + B.x) / 2,
                        (A.y + B.y) / 2,
                        (A.z + B.z) / 2);
}

CCVector3d ccExametrics::getVectorMediator()
{
    CCVector3d AB = getNormalizedVectorPointB() - getNormalizedVectorPointA();
    return CCVector3d(getVectorCenter() * AB);
}



