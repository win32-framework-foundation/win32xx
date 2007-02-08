///////////////////////////////////////
// DlgSubclassApp.cpp

#include "resource.h"
#include "DlgSubclassApp.h"


// Definitions for the CDialogApp class
CDlgSubclassApp::CDlgSubclassApp(HINSTANCE hInstance) : CWinApp(hInstance), m_MyDialog(IDD_DIALOG1)
{
    //Start Trace
    TRACE("Start Tracing");

	//Display the Modal Dialog
	m_MyDialog.DoModal();

	//End the program
	::PostQuitMessage(0);
}

CDlgSubclassApp::~CDlgSubclassApp()
{
}

