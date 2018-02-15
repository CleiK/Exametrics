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

	//m_app->dispToConsole("New Selection", ccMainAppInterface::STD_CONSOLE_MESSAGE);
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

	m_app->dispToConsole("[ccExametrics] ccExametrics plugin initialized successfully.", ccMainAppInterface::STD_CONSOLE_MESSAGE);
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
		m_app->dispToConsole("[ccExametrics] Could not find valid 3D window.", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}

	//bind gui
	if (!m_dlg)
	{
		//bind GUI events
		m_dlg = new ccExametricsDialog((QWidget*)m_app->getMainWindow());

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



		m_dlg->linkWith(m_app->getActiveGLWindow());

		// initialize parameters widgets with the object in db tree
		initializeParameterWidgets(m_app->dbRootObject()->getBB_recursive());

		// initialize polylines etc
		initializeDrawSettings();

		//start GUI
		m_app->registerOverlayDialog(m_dlg, Qt::TopRightCorner);
		m_dlg->start();
	}


	/*m_app->dispToConsole("[ccExametrics] Hello world!",ccMainAppInterface::STD_CONSOLE_MESSAGE); //a standard message is displayed in the console
	m_app->dispToConsole("[qExametricsPlugin] Warning: dummy plugin shouldn't be used as is!",ccMainAppInterface::WRN_CONSOLE_MESSAGE); //a warning message is displayed in the console
	m_app->dispToConsole("Dummy plugin shouldn't be used as is!",ccMainAppInterface::ERR_CONSOLE_MESSAGE); //an error message is displayed in the console AND an error box will pop-up!*/
}

/* Called when the plugin is being stopped */
void ccExametrics::stop()
{
	/*//remove click listener
	if (m_app->getActiveGLWindow())
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
	m_app->dispToConsole("[ccExametrics] Compute!",ccMainAppInterface::STD_CONSOLE_MESSAGE);

	// vector point is on the normalized vector ?
	/*if(!pointIsOnVector(getNormalizedVectorPointA(), getNormalizedVectorPointB(), getVectorPoint()))
		return;*/


	//get point cloud
	ccPointCloud* cloud = static_cast<ccPointCloud*>(m_app->dbRootObject()); //cast to point cloud

	if (!cloud)
	{
		m_app->dispToConsole("[ccExametrics] not a cloud :(", ccMainAppInterface::WRN_CONSOLE_MESSAGE);
		return;
	}
	else
	{
		m_app->dispToConsole("[ccExametrics] is a cloud :)", ccMainAppInterface::STD_CONSOLE_MESSAGE);

	}
}

/* Slot on dialog closed */
void ccExametrics::onClose()
{
	stop();

	m_app->removeFromDB(this->normalizedVectorPoly);
	m_app->removeFromDB(this->vectorPoint2DLabel);
	m_app->removeFromDB(this->plan);
}

/* Initialize plan parameters at random values with min and max limits */
void ccExametrics::initializeParameterWidgets(ccBBox box)
{
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

	// random vector
	m_dlg->spbXA->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
	m_dlg->spbYA->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
	m_dlg->spbZA->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
	m_dlg->spbXB->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
	m_dlg->spbYB->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));
	m_dlg->spbZB->setValue(Utils::frand_a_b(minCorner.x, maxCorner.x));

    // vector point distance coefficient
	m_dlg->coefSlider->setValue(50);

	// tolerance
	m_dlg->toleranceSpb->setValue(0.01);


	QString boundariesString = "[ccExametrics] Boundaries: ("
								+ QString::number(xBox) + "; "
								+ QString::number(yBox) + "; "
								+ QString::number(zBox) + ")";

	m_app->dispToConsole(boundariesString, ccMainAppInterface::STD_CONSOLE_MESSAGE);
}

/* Initialize draw settings for normalized vector, point and plan display */
void ccExametrics::initializeDrawSettings()
{
	/* Normalized vector */

	// new 2 points cloud
	this->normalizedVectorCloud = new ccPointCloud("Normalized vector");

	// reserve 2 points
	this->normalizedVectorCloud->reserve(2);

	// add points
	this->normalizedVectorCloud->addPoint((CCVector3)getNormalizedVectorPointA());
	this->normalizedVectorCloud->addPoint((CCVector3)getNormalizedVectorPointB());

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
	m_app->addToDB(this->normalizedVectorPoly);

	/* Vector point */

	this->vectorPointCloud = new ccPointCloud("Vector point");
	this->vectorPointCloud->reserve(1);
	this->vectorPointCloud->addPoint(getVectorPoint());
	this->vectorPoint2DLabel = new cc2DLabel("Vector point");
	this->vectorPoint2DLabel->addPoint(this->vectorPointCloud, this->vectorPointCloud->size() - 1);
	this->vectorPoint2DLabel->setVisible(true);
	this->vectorPoint2DLabel->setDisplay(m_app->getActiveGLWindow());
	m_app->addToDB(this->vectorPoint2DLabel);

	/* Plan */
	/*this->planCloud = new ccPointCloud("Plan");
	this->planCloud->reserve(4);
	planTransformation = new ccGLMatrix();
	planTransformation->toIdentity();

	CCVector3 normalizedVector = getNormalizedVector();
	CCVector3 vectorPoint = getVectorPoint();
	double normalizedVectorNorm = normalizedVector.normd();
	//double normOB = sqrt(pow(m_dlg->spbXB->value(), 2) + pow(m_dlg->spbYB->value(), 2) + pow(m_dlg->spbZB->value(), 2));
	double phi = acos(normalizedVector.x / (normalizedVectorNorm * sin(acos(normalizedVector.z / normalizedVectorNorm)))); // (x)
	double theta = 0;
    double psi = acos(normalizedVector.z / normalizedVectorNorm); // (z)
	m_app->dispToConsole("composantes x: " + QString::number(normalizedVector.x)
                        + " y: " + QString::number(normalizedVector.y)
                        + " z: " + QString::number(normalizedVector.z)
                        + "\nnorme: " + QString::number(normalizedVectorNorm)
                        //+ "\n normOB: " + QString::number(normOB)
                        );
	m_app->dispToConsole("Phi = " + QString::number(phi) + " or = " + QString::number(phi * 180 / M_PI), ccMainAppInterface::STD_CONSOLE_MESSAGE);
	m_app->dispToConsole("Theta = " + QString::number(theta) + " or = " + QString::number(theta * 180 / M_PI), ccMainAppInterface::STD_CONSOLE_MESSAGE);
	m_app->dispToConsole("Psi = " + QString::number(psi) + " or = " + QString::number(psi * 180 / M_PI), ccMainAppInterface::STD_CONSOLE_MESSAGE);

    //CCVector3 orthoVector1 = normalizedVector.orthogonal();
    //m_app->dispToConsole("[ccExametrics] ortho vector 1:  " + QString::number(orthoVector1.x) + " " + QString::number(orthoVector1.y) + " " + QString::number(orthoVector1.z));
	// initFromParameters phi theta psi: projeter le vecteur normal pour obtenir les angles + translation depuis le point
	//planTransformation->initFromParameters(phi, theta, psi, Vector3Tpl<float>(0,0,0));
    planTransformation->initFromParameters(phi, theta, psi, vectorPoint);
	//planTransformation->initFromParameters(0,0,0,vectorPoint);

    //planTransformation->applyRotation(orthoVector1);
	this->plan = new ccPlane(m_boxXWidth, m_boxYWidth, planTransformation);
	this->plan->setDisplay(m_app->getActiveGLWindow());
	m_app->addToDB(this->plan);*/
}

/* Called when xA spinbox changed value */
void ccExametrics::onSpbXAChanged(double value){ onParameterChanged(m_dlg->spbXA, value); onNormalizedVectorChanged(); }
/* Called when yA spinbox changed value */
void ccExametrics::onSpbYAChanged(double value){ onParameterChanged(m_dlg->spbYA, value); onNormalizedVectorChanged(); }
/* Called when zA spinbox changed value */
void ccExametrics::onSpbZAChanged(double value){ onParameterChanged(m_dlg->spbZA, value); onNormalizedVectorChanged(); }
/* Called when xB spinbox changed value */
void ccExametrics::onSpbXBChanged(double value){ onParameterChanged(m_dlg->spbXB, value); onNormalizedVectorChanged(); }
/* Called when yB spinbox changed value */
void ccExametrics::onSpbYBChanged(double value){ onParameterChanged(m_dlg->spbYB, value); onNormalizedVectorChanged(); }
/* Called when zB spinbox changed value */
void ccExametrics::onSpbZBChanged(double value){ onParameterChanged(m_dlg->spbZB, value); onNormalizedVectorChanged(); }
/* Called when distance coefficient slider changed value */
void ccExametrics::onCoefSliderChanged(int value){ onParameterChanged(m_dlg->coefSlider, value); onVectorPointChanged(value); }
/* Called when tolerance spinbox changed value */
void ccExametrics::onToleranceSpbChanged(double value){ onParameterChanged(m_dlg->toleranceSpb, value); }

/* Called when the parameters of the normalized vector are changing */
void ccExametrics::onNormalizedVectorChanged()
{
	if(!this->normalizedVectorCloud)
		return;

	// clear old points
	this->normalizedVectorCloud->clear();

	// reserve 2 points
	this->normalizedVectorCloud->reserve(2);

	// add points
    this->normalizedVectorCloud->addPoint(getNormalizedVectorPointA());
	this->normalizedVectorCloud->addPoint(getNormalizedVectorPointB());

	//m_app->getActiveGLWindow()->moveCamera(1,0,0);
	//m_app->updateUI();
	//m_app->refreshAll();
}

/* Called when the parameter of the vector point is changing */
void ccExametrics::onVectorPointChanged(int coef)
{
    m_dlg->lblCoef->setText(QString::number(coef) + "%");

    // calcul du point en utilisant le vecteur
    CCVector3 pointA = getNormalizedVectorPointA();
    CCVector3 normalizedVector = getNormalizedVector();
    double k = (double)coef / 100;

    CCVector3 resultVector = CCVector3(normalizedVector.x * k, normalizedVector.y * k, normalizedVector.z * k);

    double l = 1, m = 0, n = 0;
    m = (l * (normalizedVector.y - pointA.y)) / (normalizedVector.x - pointA.x);
    n = (m * (normalizedVector.z - pointA.z)) / (normalizedVector.y - pointA.y);

    this->m_vectorPoint = CCVector3(resultVector.x * l + pointA.x, resultVector.y * m + pointA.y, resultVector.z * n + pointA.z);

    m_app->dispToConsole("[ccExametrics] k = " + QString::number(k), ccMainAppInterface::STD_CONSOLE_MESSAGE);
    m_app->dispToConsole("[ccExametrics] normalizedVector = " + Utils::ccVector3ToString(normalizedVector)
                        + " resultVector = " + Utils::ccVector3ToString(resultVector)
                        + " pointA = " + Utils::ccVector3ToString(pointA)
                        + " vectorPoint = " + Utils::ccVector3ToString(getVectorPoint())
                        + " l = " + QString::number(l)
                        + " m = " + QString::number(m)
                        + " n = " + QString::number(n), ccMainAppInterface::STD_CONSOLE_MESSAGE);


	// verif point est sur le vecteur (doit verifier [(Xb - Xa) * k + Xa])
	/*if(!pointIsOnVector(getNormalizedVectorPointA(), getNormalizedVectorPointB(), getVectorPoint()))
		m_app->dispToConsole("[ccExametrics] Defined point is not on the normalized vector.", ccMainAppInterface::WRN_CONSOLE_MESSAGE);*/

    if(!this->vectorPointCloud)
		return;

	this->vectorPointCloud->clear();
	this->vectorPointCloud->reserve(1);
	this->vectorPointCloud->addPoint(getVectorPoint());
}

/* Called when a parameters of the plan is changing */
void ccExametrics::onParameterChanged(QWidget* w, double value)
{
	//m_app->dispToConsole("[ccExametrics] onParameterChanged " + spb->objectName() + " " + QString::number(value), ccMainAppInterface::STD_CONSOLE_MESSAGE);
}

/* Return the norm on X axis for the normalized vector */
double ccExametrics::getNormX() { if(!m_dlg) return 0; return abs(m_dlg->spbXB->value() - m_dlg->spbXA->value()); }
/* Return the norm on Y axis for the normalized vector */
double ccExametrics::getNormY() { if(!m_dlg) return 0; return abs(m_dlg->spbYB->value() - m_dlg->spbYA->value()); }
/* Return the norm on Z axis for the normalized vector */
double ccExametrics::getNormZ() { if(!m_dlg) return 0; return abs(m_dlg->spbZB->value() - m_dlg->spbZA->value()); }
/* Return point A of the normalized vector */
CCVector3 ccExametrics::getNormalizedVectorPointA(){ if(!m_dlg) return CCVector3(0,0,0); return CCVector3(m_dlg->spbXA->value(), m_dlg->spbYA->value(), m_dlg->spbZA->value()); }
/* Return point B of the normalized vector */
CCVector3 ccExametrics::getNormalizedVectorPointB(){ if(!m_dlg) return CCVector3(0,0,0); return CCVector3(m_dlg->spbXB->value(), m_dlg->spbYB->value(), m_dlg->spbZB->value()); }
/* Return the normalized vector */
CCVector3 ccExametrics::getNormalizedVector() { if(!m_dlg) return CCVector3(0,0,0); return CCVector3(getNormX(), getNormY(), getNormZ()); }
/* Return the point on the vector (compute from coef distance) */
CCVector3 ccExametrics::getVectorPoint() { return m_vectorPoint; }


/*bool ccExametrics::pointIsOnVector(CCVector3 vectorPointA, CCVector3 vectorPointB, CCVector3 myPoint)
{
	double kx = 0, ky = 0, kz = 0;
	bool isOn = false;

	kx = (myPoint.x - vectorPointA.x) / (vectorPointB.x - vectorPointA.x);
	ky = (myPoint.y - vectorPointA.y) / (vectorPointB.y - vectorPointA.y);
	kz = (myPoint.z - vectorPointA.z) / (vectorPointB.z - vectorPointA.z);

	if(Utils::double_equals(kx, ky) && Utils::double_equals(kx, kz) && Utils::double_equals(ky, kz))
		isOn = true;

	m_app->dispToConsole("[ccExametrics] kx = " + QString::number(kx)
						+ " ky = " + QString::number(ky)
						+ " kz = " + QString::number(kz)
						+ " isOn = " + QString::number(isOn)
						, ccMainAppInterface::STD_CONSOLE_MESSAGE);

	return isOn;
}*/

