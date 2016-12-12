/*
 Copyright 2012-2014 Eigenlabs Ltd.  http://www.eigenlabs.com

 This file is part of EigenD.

 EigenD is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 EigenD is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with EigenD.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <piw/piw_tsd.h>
#include <picross/pic_resources.h>
#include "testgui.h"
// #include "AboutComponent.h"

#include "ejuce.h"
#include "epython.h"

#include "MainComponent.h"
// #include "WorkspaceManager.h"
// #include "WorkbenchFrame.h"
// #include "MenuManager.h"

class BackendFactory
{
    public:
    virtual ~BackendFactory(){};
    virtual epython::PythonBackend *getBackend()=0;

};


const static String TESTGUI_VERSION="0.0.1";


class TestGuiApp;
class EigenLogger
{
    public:
        EigenLogger(const char *prefix, const pic::f_string_t &logger);
        EigenLogger(const EigenLogger &l);
        EigenLogger &operator=(const EigenLogger &l);
        bool operator==(const EigenLogger &l) const;
        static pic::f_string_t create(const char *prefix, const pic::f_string_t &logger);
        void operator()(const char *msg) const;

    private:
        juce::String prefix_;
        pic::f_string_t logger_;
};

class MainWindow  : public DocumentWindow,public BackendFactory
{
    public:
        MainWindow(TestGuiApp* app, const pic::f_string_t &log); 
        ~MainWindow();
        pic::f_string_t make_logger(const char *prefix);
        void closeButtonPressed();
        virtual void handleCommandMessage(int commandId);
        epython::PythonBackend *getBackend();

    private:
        // WorkbenchFrame *component_;
        // WorkspaceManager* wsm_;
        // MenuManager* menuManager_;
        pic::f_string_t logger_;
        TestGuiApp* app_;
};

class TestGuiApp : public ejuce::Application, public BackendFactory, virtual public pic::tracked_t
{
public:
    TestGuiApp();
    ~TestGuiApp();
    void initialise (const String& commandLine);
    void shutdown();
    const String getApplicationName();
    const String getApplicationVersion();
    bool moreThanOneInstanceAllowed();
    void anotherInstanceStarted (const String& commandLine);
    void log(const char *msg);
    epython::PythonBackend *getBackend();

private:
    MainWindow* main_window_;
    epython::PythonInterface *python_interp_;
    epython::PythonBackend *python_backend0_;
    pia::context_t context_;
    FILE *logfile_;
};

MainWindow::MainWindow( TestGuiApp* app, const pic::f_string_t &log) : DocumentWindow ("TestGui", Colours::lightgrey, DocumentWindow::allButtons, true), logger_(log), app_(app)

{
    // component_ = new WorkbenchFrame();
    // ApplicationCommandManager *manager = new ApplicationCommandManager();
    // menuManager_=new MenuManager(manager,component_->get_tabbedComponent());
    // wsm_ =new WorkspaceManager(this,component_,menuManager_);
    // wsm_->setRootWorkspaceComponent();
    // setContentComponent (component_, true, true);

    setUsingNativeTitleBar (true);
    setContentOwned (new MainContentComponent(), true);

    centreWithSize (getWidth(), getHeight());
    setVisible (true);

// #ifdef JUCE_MAC
//     MenuBarModel::setMacMainMenu(menuManager_);
// #else
//     setMenuBar(menuManager_);
// #endif

    setUsingNativeTitleBar(true);
    getLookAndFeel().setColour(ProgressBar::foregroundColourId,Colour(0xff777777));
    int h=786;
    if(h>getParentMonitorArea().getHeight()-30)
    {
        h=getParentMonitorArea().getHeight()-30;
    }
    int w=982;
    if(w>getParentMonitorArea().getWidth()-20)
    {
        w=getParentMonitorArea().getWidth()-20;
    }

    centreWithSize (w, h);
    setResizeLimits(600,320,4000,3000);
    setResizable(true,true);
    setVisible (true);
}

epython::PythonBackend *MainWindow::getBackend()
{
    return app_->getBackend();
}

MainWindow::~MainWindow()
{
}

pic::f_string_t MainWindow::make_logger(const char *prefix)
{
    JUCE_AUTORELEASEPOOL
    return EigenLogger::create(prefix,logger_);
}

void MainWindow::closeButtonPressed()
{
    JUCEApplication::quit();
}

void MainWindow::handleCommandMessage(int commandId)
{
    // if(commandId==1020)
    // {
    //     AboutComponent* dc=new AboutComponent();
    //     dc->setVersionText(TESTGUI_VERSION);
    //     DialogWindow::showModalDialog("About",dc,this,Colour (0xffababab),true);
    // }
}

TestGuiApp::TestGuiApp(): main_window_ (0),logfile_(0)
{
}

TestGuiApp::~TestGuiApp()
{
}

void TestGuiApp::initialise (const String& commandLine)
{
    pic::f_string_t primary_logger = pic::f_string_t::method(this,&TestGuiApp::log);
    pic::f_string_t testgui_logger = EigenLogger::create("testgui",primary_logger);

    ejuce::Application::eInitialise(commandLine,testgui_logger,false,false);
    python_interp_ = new epython::PythonInterface();
    context_ = scaffold()->context("main",pic::status_t(),testgui_logger,"testgui");
    piw::tsd_setcontext(context_.entity());
    python_interp_->py_startup();

    python_backend0_ = new epython::PythonBackend(python_interp_);

    if(python_backend0_->init_python("app_testgui.testgui","main0"))
    {
        testgui::c2p0_t *backend = (testgui::c2p0_t *)python_backend0_->mediator();
        if(backend)
        {
            backend->set_args(commandLine.toUTF8());
            std::string logfile=backend->get_logfile();
            if(logfile.length()>0)
            {
                logfile_ = pic::fopen(logfile,"w");
            }
        }
    }

    main_window_ = new MainWindow(this, primary_logger);
}

epython::PythonBackend* TestGuiApp::getBackend()
{
    epython::PythonBackend *b0 = new epython::PythonBackend(python_interp_);

    if(b0->init_python("app_testgui.testgui","main"))
    {
        return b0;
    }

    delete b0;
    return 0;
}

void TestGuiApp::shutdown()
{
    if (main_window_ != 0)
        delete main_window_;
}

const String TestGuiApp::getApplicationName()
{
    return "TestGui";
}

const String TestGuiApp::getApplicationVersion()
{
    return TESTGUI_VERSION;
}

bool TestGuiApp::moreThanOneInstanceAllowed()
{
    return false;
}

void TestGuiApp::anotherInstanceStarted (const String& commandLine)
{
}

void TestGuiApp::log(const char *msg)
{
    if(logfile_)
    {
        fprintf(logfile_,"%s\n",msg);
        fflush(logfile_);
    }
    else
    {
        printf("%s\n",msg);
        fflush(stdout);
    }
}

EigenLogger::EigenLogger(const char *prefix, const pic::f_string_t &logger): prefix_(prefix), logger_(logger)
{
}

EigenLogger::EigenLogger(const EigenLogger &l): prefix_(l.prefix_), logger_(l.logger_)
{
}

EigenLogger &EigenLogger::operator=(const EigenLogger &l)
{
    prefix_=l.prefix_;
    logger_=l.logger_;
    return *this;
}

bool EigenLogger::operator==(const EigenLogger &l) const
{
    return (logger_==l.logger_) && (prefix_.compare(l.prefix_)==0);
}

pic::f_string_t EigenLogger::create(const char *prefix, const pic::f_string_t &logger)
{
    return pic::f_string_t::callable(EigenLogger(prefix,logger));
}

void EigenLogger::operator()(const char *msg) const
{
    juce::String buffer(prefix_);
    buffer += ": "; buffer+=msg;
    logger_(std::string(buffer.getCharPointer()).c_str());
}


START_JUCE_APPLICATION (TestGuiApp)
