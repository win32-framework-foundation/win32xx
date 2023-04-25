/////////////////////////////
// Mainfrm.h
//

#ifndef MAINFRM_H
#define MAINFRM_H

#include "View.h"


///////////////////////////////////////////////////////////
// CMainFrame manages the application's main window.
// The main window is a frame which has a menubar, toolbar,
// statusbar and view window.
class CMainFrame : public CFrame
{
public:
    CMainFrame();
    virtual ~CMainFrame();
    virtual HWND Create(HWND parent = 0);

    void DPIScaleToolBar();

protected:
    // Virtual functions that override base class functions
    virtual BOOL OnCommand(WPARAM wparam, LPARAM lparam);
    virtual int  OnCreate(CREATESTRUCT& cs);
    virtual LRESULT OnDPIChanged(UINT msg, WPARAM wparam, LPARAM lparam);
    virtual void SetupMenuIcons();
    virtual void SetupToolBar();
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam);

private:
    CMainFrame(const CMainFrame&);                // Disable copy construction
    CMainFrame& operator = (const CMainFrame&);   // Disable assignment operator

    // Message handlers
    LRESULT OnFileLoaded(LPCTSTR fileName);
    LRESULT OnWindowPosChanged(UINT msg, WPARAM wparam, LPARAM lparam);

    // Command handlers
    BOOL OnFileExit();
    BOOL OnFileMRU(WPARAM wparam);
    BOOL OnFileNew();
    BOOL OnFileOpen();
    BOOL OnFileSaveAs();

    // Member variables
    CView m_view;
    CImageList m_menuImages;
    CRect m_viewRect;
    CPoint m_scrollPos;
    bool m_isDPIChanging;
};

#endif //MAINFRM_H

