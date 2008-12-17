//////////////////////////////////////////////
// container.cpp
//  Definitions for the CContainer class

#include "../Win32++/gdi.h"
#include "container.h"
#include "ContainerApp.h"
#include "resource.h"

/////////////////////////////////////
// Declaration of the CTabPage class
BOOL CTabPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	 return (BOOL)::SendMessage(m_hWndParent, WM_COMMAND, wParam, lParam);
}

void CTabPage::OnCreate()
{
//	if ((m_pwndView) &&(NULL == m_pwndView->GetHwnd()))
	{	
		m_pwndView->Create(m_hWnd);	
	}
}

void CTabPage::PreRegisterClass(WNDCLASS &wc)
{
	wc.lpszClassName = _T("Win32++ TabPage");
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
}

void CTabPage::RecalcLayout()
{
	CRect rc = GetClientRect();
	CRect rcToolbar = m_wndToolbar.GetClientRect();
	rc.top += rcToolbar.Height();
	GetToolbar().SendMessage(TB_AUTOSIZE, 0, 0);
	GetView()->SetWindowPos(NULL, rc, SWP_SHOWWINDOW);
}

void CTabPage::SetView(CWnd& wndView)
// Sets or changes the View window displayed within the frame
{	
	// Assign the view window
	m_pwndView = &wndView;
		
	if (m_hWnd)
	{
		// The container is already created, so create and position the new view too
		GetView()->Create(m_hWnd);
		RecalcLayout();
	}
}

LRESULT CTabPage::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		RecalcLayout();
		break;
	}

	// pass unhandled messages on for default processing
	return WndProcDefault(hWnd, uMsg, wParam, lParam);
}

//////////////////////////////////////
// Declaration of the CContainer class
CContainer::CContainer() : m_iCurrentPage(0)
{
	m_himlTab = ImageList_Create(16, 16, ILC_MASK|ILC_COLOR32, 0, 0);

	// Set the Resource IDs for the toolbar buttons
//	AddToolbarButton( IDM_FILE_NEW   );
//	AddToolbarButton( IDM_FILE_OPEN  );
//	AddToolbarButton( IDM_FILE_SAVE  );
//	AddToolbarButton( 0 );				// Separator
//	AddToolbarButton( IDM_EDIT_CUT   );
//	AddToolbarButton( IDM_EDIT_COPY  );
//	AddToolbarButton( IDM_EDIT_PASTE );
//	AddToolbarButton( 0 );				// Separator
//	AddToolbarButton( IDM_FILE_PRINT );
//	AddToolbarButton( 0 );				// Separator
//	AddToolbarButton( IDM_HELP_ABOUT );
}

CContainer::~CContainer()
{
	ImageList_Destroy(m_himlTab);
}

void CContainer::AddContainer(CContainer* pwndContainer, LPCTSTR szTitle, HICON hIcon)
// Adds an existing container to this one
{
	TabPageInfo tbi = {0};
	tbi.pwndContainer = pwndContainer;
	lstrcpy(tbi.szTitle, szTitle);
	tbi.hIcon = hIcon;
	int iNewPage = m_vTabPageInfo.size();
	
	m_vTabPageInfo.push_back(tbi);
	ImageList_AddIcon(m_himlTab, hIcon);

	if (m_hWnd)
	{
		TCITEM tie = {0}; 
		tie.mask = TCIF_TEXT | TCIF_IMAGE; 
		tie.iImage = iNewPage; 
		tie.pszText = m_vTabPageInfo[iNewPage].szTitle; 
		TabCtrl_InsertItem(m_hWnd, iNewPage, &tie);
	}
}

void CContainer::AddToolbarButton(UINT nID)
// Adds Resource IDs to toolbar buttons.
// A resource ID of 0 is a separator
{
	m_vToolbarData.push_back(nID);
}

int CContainer::FindPage(CWnd* pwndPage)
{
	int iPage = -1;

/*	for (int i = 0; i < (int)m_vTabPageInfo.size(); ++i)
	{
		if (m_vTabPageInfo[i].pwndView == pwndPage)
		{
			iPage = i;
			break;
		}
	}*/

	return iPage;
}

SIZE CContainer::GetMaxTabTextSize()
{
	CSize Size;

	// Allocate an iterator for the TabPageInfo vector
	std::vector<TabPageInfo>::iterator iter;

	for (iter = m_vTabPageInfo.begin(); iter != m_vTabPageInfo.end(); ++iter)
	{
		CSize TempSize;
		CDC dc = GetDC();
		NONCLIENTMETRICS info = {0};
		info.cbSize = sizeof(info);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);		
		dc.CreateFontIndirect(&info.lfStatusFont);
		GetTextExtentPoint32(dc, iter->szTitle, lstrlen(iter->szTitle), &TempSize);
		if (TempSize.cx > Size.cx)
			Size = TempSize;
	}

	return Size;
}

void CContainer::OnCreate()
{
	// Create the page window
	m_wndPage.Create(m_hWnd);

	// Create the toolbar
	if (m_vToolbarData.size() > 0)
	{
		GetToolbar().Create(m_wndPage.GetHwnd());
		DWORD style = (DWORD)GetToolbar().GetWindowLongPtr(GWL_STYLE);
		style |= CCS_NODIVIDER ;//| CCS_NORESIZE;
		GetToolbar().SetWindowLongPtr(GWL_STYLE, style);
		int iButtons = GetToolbar().SetButtons(m_vToolbarData);

		// Set the toolbar images
		// A mask of 192,192,192 is compatible with AddBitmap (for Win95)
		GetToolbar().SetImages(iButtons, RGB(192,192,192), IDW_MAIN, 0, 0);
		GetToolbar().SendMessage(TB_AUTOSIZE, 0, 0);
	}

    for (int i = 0; i < (int)m_vTabPageInfo.size(); ++i)
	{
		// Add tabs for each view. 
		TCITEM tie = {0}; 
		tie.mask = TCIF_TEXT | TCIF_IMAGE; 
		tie.iImage = i; 
		tie.pszText = m_vTabPageInfo[i].szTitle; 
        TabCtrl_InsertItem(m_hWnd, i, &tie);
	}
}

LRESULT CContainer::OnNotifyReflect(WPARAM /*wParam*/, LPARAM lParam)
{
	switch (((LPNMHDR)lParam)->code)
	{
	case TCN_SELCHANGE: 
		{ 
			// Display the newly selected tab page
			int iPage = TabCtrl_GetCurSel(m_hWnd);
			SelectPage(iPage);
		} 
        break; 
	}

	return 0L;
}

void CContainer::OnPaint()
{
	// Microsoft's drawing for a tab control is rubbish, so we do our own.
	// We use double buffering and regions to eliminate flicker

	// Create the memory DC and bitmap
	CDC dcMem = CreateCompatibleDC(GetDC());
	CRect rcClient = GetClientRect();

	dcMem.CreateCompatibleBitmap(GetDC(), rcClient.Width(), rcClient.Height());

	// Create a clipping region. Its the overall tab window's region, 
	//  less the region belonging to the individual tab view's client area
	HRGN hrgnSrc1 = ::CreateRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
	CRect rcTab = GetClientRect();
	TabCtrl_AdjustRect(m_hWnd, FALSE, &rcTab);
	HRGN hrgnSrc2 = ::CreateRectRgn(rcTab.left, rcTab.top, rcTab.right, rcTab.bottom);
	HRGN hrgnClip = ::CreateRectRgn(0, 0, 0, 0); 
	::CombineRgn(hrgnClip, hrgnSrc1, hrgnSrc2, RGN_DIFF);
	
	// Use the region in the memory DC to paint the grey background
	dcMem.AttachRegion(hrgnClip);
	dcMem.CreateSolidBrush(RGB(230, 230, 230));
	::PaintRgn(dcMem, hrgnClip);

	// Draw the tab buttons:
	for (int i = 0; i < TabCtrl_GetItemCount(m_hWnd); ++i)
	{
		CRect rcItem;
		TabCtrl_GetItemRect(m_hWnd, i, &rcItem);
		if (i == TabCtrl_GetCurSel(m_hWnd))
		{
			dcMem.CreateSolidBrush(RGB(248,248,248));
			SetBkColor(dcMem, RGB(248,248,248));
		}
		else
		{
			dcMem.CreateSolidBrush(RGB(200,200,200));
			SetBkColor(dcMem, RGB(200,200,200));
		}
		dcMem.CreatePen(PS_SOLID, 1, RGB(160, 160, 160));
		RoundRect(dcMem, rcItem.left+1, rcItem.top, rcItem.right+2, rcItem.bottom, 6, 6);

		if (rcItem.Width() >= 24)
		{
			TCHAR szText[30];
			TCITEM tcItem = {0};
			tcItem.mask = TCIF_TEXT | TCIF_IMAGE;
			tcItem.cchTextMax = 30;
			tcItem.pszText = szText;
			TabCtrl_GetItem(m_hWnd, i, &tcItem);
			
			// Draw the icon
			ImageList_Draw(m_himlTab, tcItem.iImage, dcMem, rcItem.left+5, rcItem.top+2, ILD_NORMAL);  

			// Draw the text
			NONCLIENTMETRICS info = {0};
			info.cbSize = sizeof(info);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);		
			dcMem.CreateFontIndirect(&info.lfStatusFont);
			CRect rcText = rcItem;
			rcText.left += 24;
			::DrawText(dcMem, szText, -1, &rcText, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		}
	}

	// Draw a lighter rectangle touching the tab buttons
	CRect rcItem;
	TabCtrl_GetItemRect(m_hWnd, 0, &rcItem);
	int left = rcItem.left +1;
	int right = rcTab.right -1;
	int top = rcTab.bottom;
	int bottom = top + 3;
	dcMem.CreateSolidBrush(RGB(248,248,248));
	dcMem.CreatePen(PS_SOLID, 1, RGB(248,248,248));
	Rectangle(dcMem, left, top, right, bottom);
	
	// Draw a darker line below the rectangle
	dcMem.CreatePen(PS_SOLID, 1, RGB(160, 160, 160));
	MoveToEx(dcMem, left-1, bottom, NULL);
	LineTo(dcMem, right, bottom);
	dcMem.CreatePen(PS_SOLID, 1, RGB(248,248,248));
    
	// Draw a lighter line below the rectangle for the selected tab
	TabCtrl_GetItemRect(m_hWnd, TabCtrl_GetCurSel(m_hWnd), &rcItem);
	OffsetRect(&rcItem, 1, 1);
	MoveToEx(dcMem, rcItem.right, rcItem.top, NULL);
	LineTo(dcMem, rcItem.left, rcItem.top); 

	// Now copy our from our memory DC to the window DC
	dcMem.DetachRegion();
	CDC dcView = GetDC();
	dcView.AttachRegion(hrgnClip);
	BitBlt(dcView, 0, 0, rcClient.Width(), rcClient.Height(), dcMem, 0, 0, SRCCOPY);

	// Cleanup
	::DeleteObject(hrgnSrc1);
	::DeleteObject(hrgnSrc2);
	// hrgnClip is attached to dcView, so it will be deleted automatically
} 

void CContainer::PreCreate(CREATESTRUCT& cs)
{
	cs.style = WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_OWNERDRAWFIXED | TCS_FIXEDWIDTH | TCS_BOTTOM ;
	cs.lpszClass = WC_TABCONTROL;

//	cs.dwExStyle = WS_EX_COMPOSITED;
}

void CContainer::RemoveContainer(CContainer& Wnd)
{
	// Allocate an iterator for the TabPageInfo vector
/*	std::vector<TabPageInfo>::iterator iter;

	for (iter = m_vTabPageInfo.begin(); iter != m_vTabPageInfo.end(); ++iter)
	{
		if (iter->pwndView == &Wnd)
		{
			m_vTabPageInfo.erase(iter);
			break;
		}
	} */		
}

void CContainer::SelectPage(int iPage)
{
	// Determine the size of the tab page's view area
	CRect rc = GetClientRect();
	TabCtrl_AdjustRect(m_hWnd, FALSE, &rc); 
	 
	// Swap the pages over
	m_vTabPageInfo[m_iCurrentPage].pwndContainer->GetPage().ShowWindow(SW_HIDE);
	m_vTabPageInfo[iPage].pwndContainer->GetPage().SetWindowPos(HWND_TOP, rc, SWP_SHOWWINDOW);
	m_vTabPageInfo[iPage].pwndContainer->GetPage().GetView()->SetFocus();

	m_iCurrentPage = iPage; 
}

LRESULT CContainer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH hbr = 0;
	
	switch (uMsg)
	{		
	case WM_PAINT:
		{	
			// Remove all pending paint requests
			PAINTSTRUCT ps;
			::BeginPaint(hWnd, &ps);
			::EndPaint(hWnd, &ps);

			// Now call our local OnPaint
			OnPaint();
		}
		break; 
	case WM_ERASEBKGND:
		return -1;
	case WM_SIZE: 
		{ 
			GetToolbar().SendMessage(TB_AUTOSIZE, 0, 0);
		
			int Width = GET_X_LPARAM(lParam);
			int Height = GET_Y_LPARAM(lParam);
			
			// Determine the size of the tab page's view area
			CRect rc(0, 0, Width, Height);
			TabCtrl_AdjustRect(m_hWnd, FALSE, &rc); 
			m_wndPage.SetWindowPos(HWND_TOP, rc, 0);
	 
			// Position and size the static control to fit the tab control's display area
			if ((int)m_vTabPageInfo.size() > m_iCurrentPage) 
			{
				m_vTabPageInfo[m_iCurrentPage].pwndContainer->GetPage().SetWindowPos(HWND_TOP, rc, SWP_SHOWWINDOW);	

				int nItemWidth = min(25 + GetMaxTabTextSize().cx, rc.Width()/(int)m_vTabPageInfo.size());
				SendMessage(TCM_SETITEMSIZE, 0, MAKELPARAM(nItemWidth, 20));
			} 
			
		} 
		break;   
	} 

	// pass unhandled messages on for default processing
	return WndProcDefault(hWnd, uMsg, wParam, lParam);
}


