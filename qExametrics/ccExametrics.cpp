//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qExametrics                 #
//#                                                                        #
//##########################################################################

//First: replace all occurrences of 'qDummyPlugin' by your own plugin class name in this file!
#include "ccExametrics.h"
#include "ccExametricsDialog.h"

//Qt
#include <QtGui>

//Default constructor: should mainly be used to initialize
//actions (pointers) and other members
ccExametrics::ccExametrics(QObject* parent/*=0*/)
	: QObject(parent)
	, m_action(0)
{
}

//deconstructor
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
	//if (m_action)
	//	m_action->setEnabled(!selectedEntities.empty());
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

	//put your code here
	//--> you may want to start by asking parameters (with a custom dialog, etc.)
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

	}

	m_dlg->linkWith(m_app->getActiveGLWindow());


	//This is how you can output messages
	m_app->dispToConsole("[ccExametrics] Hello world!",ccMainAppInterface::STD_CONSOLE_MESSAGE); //a standard message is displayed in the console
	/*m_app->dispToConsole("[qExametricsPlugin] Warning: dummy plugin shouldn't be used as is!",ccMainAppInterface::WRN_CONSOLE_MESSAGE); //a warning message is displayed in the console
	m_app->dispToConsole("Dummy plugin shouldn't be used as is!",ccMainAppInterface::ERR_CONSOLE_MESSAGE); //an error message is displayed in the console AND an error box will pop-up!*/


	//start GUI
	m_app->registerOverlayDialog(m_dlg, Qt::TopRightCorner);
	m_dlg->start();

	/*** HERE ENDS THE ACTION ***/

}

//called when the plugin is being stopped
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

//This method should return the plugin icon (it will be used mainly
//if your plugin as several actions in which case CC will create a
//dedicated sub-menu entry with this icon.
QIcon ccExametrics::getIcon() const
{
	//open qDummyPlugin.qrc (text file), update the "prefix" and the
	//icon(s) filename(s). Then save it with the right name (yourPlugin.qrc).
	//(eventually, remove the original qDummyPlugin.qrc file!)
	return QIcon(":/CC/plugin/qExametrics/exametrics_icon.png");
}

void ccExametrics::onCompute()
{
	m_app->dispToConsole("[ccExametrics] Compute!",ccMainAppInterface::STD_CONSOLE_MESSAGE);
}

void ccExametrics::onClose()
{
	stop();
}