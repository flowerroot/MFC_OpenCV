// MFC_OpenCV.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "MFC_OpenCV.h"
#include "MFC_OpenCVDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCOpenCVApp
BEGIN_MESSAGE_MAP(CMFCOpenCVApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CMFCOpenCVApp 생성
CMFCOpenCVApp::CMFCOpenCVApp() {
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CMFCOpenCVApp 개체입니다.
CMFCOpenCVApp theApp;

// CMFCOpenCVApp 초기화
BOOL CMFCOpenCVApp::InitInstance() {
	CWinApp::InitInstance();

	CMFCOpenCVDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	return FALSE;
}