#define _CRT_SEUCRE_NO_WARNINGS

/*
* 제   목 : 어느 세월에 그림판으로 다 그리고 앉아있어~!~!
* 기   능 : Auto_Curved(국책과제) 촬상용 이미지 (결함시료) 자동으로 그려줌
* 파일이름 : MFC_OpenCV
* 수정날짜 : 2023-05-12
* 작 성 자 : 에프디스플레이 선행개발팀 김영진
*/

/*
* 2023-04-20 Progress bar 추가.
* 2023-05-10 '불량위치 확인용' 이미지 그리는 기능 추가.
* 2023-05-11 이미지 저장 시 경로 세분화
*			 국책과제 컨셉에 맞게 무결함시료도 생성 및 저장.
*			 Save image Mono & Color 통합.
*			 Save 버튼 UI 수정.
* 2023-05-12 레벨별 결함의 강도를 주변밝기 대비 비율로 자동 연산하도록 수정.
*/

// MFC_OpenCVDlg.cpp: 구현 파일

#include "pch.h"
#include "framework.h"
#include "MFC_OpenCV.h"
#include "MFC_OpenCVDlg.h"
#include "afxdialogex.h"
#include <direct.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

#define PanelSizeX 2940 // 캔버스 x축 px
#define PanelSizeY 816 // 캔버스 y축 px

enum Defect_Percent { // 주변대비 결함의 수준 %
	// 2023-05-12 KYJ 기존 enum 삭제하고 새로만든 넘
	
	// PD 결함
	PD_Defect_Percent_LEVEL_1 = 10,
	PD_Defect_Percent_LEVEL_2 = 30,
	PD_Defect_Percent_LEVEL_3 = 50,
	PD_Defect_Percent_LEVEL_4 = 70,
	PD_Defect_Percent_LEVEL_5 = 90,

	// LD 결함
	LD_Defect_Percent_LEVEL_1 = 3,
	LD_Defect_Percent_LEVEL_2 = 33,
	LD_Defect_Percent_LEVEL_3 = 53,
	LD_Defect_Percent_LEVEL_4 = 73,
	LD_Defect_Percent_LEVEL_5 = 93,

	// Stain 결함
	Stain_Defect_Percent_LEVEL_1 = 3,
	Stain_Defect_Percent_LEVEL_2 = 13,
	Stain_Defect_Percent_LEVEL_3 = 23,
	Stain_Defect_Percent_LEVEL_4 = 33,
	Stain_Defect_Percent_LEVEL_5 = 43
};

// 2023-05-12 KYJ 레벨별 결함의 강도를 주변밝기 대비 비율로 자동 연산하도록 수정.
int PD_127_Bright_LEVEL[] = { 127 + (127 * (PD_Defect_Percent_LEVEL_1 / 100.0)),
								127 + (127 * (PD_Defect_Percent_LEVEL_2 / 100.0)),
								127 + (127 * (PD_Defect_Percent_LEVEL_3 / 100.0)),
								127 + (127 * (PD_Defect_Percent_LEVEL_4 / 100.0)),
								127 + (127 * (PD_Defect_Percent_LEVEL_5 / 100.0)) };
int PD_127_Dark_LEVEL[] = { 127 - (127 * (PD_Defect_Percent_LEVEL_1 / 100.0)),
								127 - (127 * (PD_Defect_Percent_LEVEL_2 / 100.0)),
								127 - (127 * (PD_Defect_Percent_LEVEL_3 / 100.0)),
								127 - (127 * (PD_Defect_Percent_LEVEL_4 / 100.0)),
								127 - (127 * (PD_Defect_Percent_LEVEL_5 / 100.0)) };
int PD_BLACK_Bright_LEVEL[] = { 15, // 블랙패턴의 결함은 아직.. 값을 어떻게 가져갈지 논리가 부족함.. 2023-05-12 KYJ
								33,
								53,
								73,
								93 };
int PD_G32_Bright_LEVEL[] = { 32 + (32 * (PD_Defect_Percent_LEVEL_1 / 100.0)),
								32 + (32 * (PD_Defect_Percent_LEVEL_2 / 100.0)),
								32 + (32 * (PD_Defect_Percent_LEVEL_3 / 100.0)),
								32 + (32 * (PD_Defect_Percent_LEVEL_4 / 100.0)),
								32 + (32 * (PD_Defect_Percent_LEVEL_5 / 100.0)) };
int PD_G32_Dark_LEVEL[] = { 32 - (32 * (PD_Defect_Percent_LEVEL_1 / 100.0)),
								32 - (32 * (PD_Defect_Percent_LEVEL_2 / 100.0)),
								32 - (32 * (PD_Defect_Percent_LEVEL_3 / 100.0)),
								32 - (32 * (PD_Defect_Percent_LEVEL_4 / 100.0)),
								32 - (32 * (PD_Defect_Percent_LEVEL_5 / 100.0)) };
int PD_G64_Bright_LEVEL[] = { 64 + (64 * (PD_Defect_Percent_LEVEL_1 / 100.0)),
								64 + (64 * (PD_Defect_Percent_LEVEL_2 / 100.0)),
								64 + (64 * (PD_Defect_Percent_LEVEL_3 / 100.0)),
								64 + (64 * (PD_Defect_Percent_LEVEL_4 / 100.0)),
								64 + (64 * (PD_Defect_Percent_LEVEL_5 / 100.0)) };
int PD_G64_Dark_LEVEL[] = { 64 - (64 * (PD_Defect_Percent_LEVEL_1 / 100.0)),
								64 - (64 * (PD_Defect_Percent_LEVEL_2 / 100.0)),
								64 - (64 * (PD_Defect_Percent_LEVEL_3 / 100.0)),
								64 - (64 * (PD_Defect_Percent_LEVEL_4 / 100.0)),
								64 - (64 * (PD_Defect_Percent_LEVEL_5 / 100.0)) };
int PD_WHITE_Dark_LEVEL[] = { 255 - (255 * (PD_Defect_Percent_LEVEL_1 / 100.0)),
								255 - (255 * (PD_Defect_Percent_LEVEL_2 / 100.0)),
								255 - (255 * (PD_Defect_Percent_LEVEL_3 / 100.0)),
								255 - (255 * (PD_Defect_Percent_LEVEL_4 / 100.0)),
								255 - (255 * (PD_Defect_Percent_LEVEL_5 / 100.0)) };
int LD_127_Bright_LEVEL[] = { 127 + (127 * (LD_Defect_Percent_LEVEL_1 / 100.0)),
								127 + (127 * (LD_Defect_Percent_LEVEL_2 / 100.0)),
								127 + (127 * (LD_Defect_Percent_LEVEL_3 / 100.0)),
								127 + (127 * (LD_Defect_Percent_LEVEL_4 / 100.0)),
								127 + (127 * (LD_Defect_Percent_LEVEL_5 / 100.0)) };
int LD_127_Dark_LEVEL[] = { 127 - (127 * (LD_Defect_Percent_LEVEL_1 / 100.0)),
								127 - (127 * (LD_Defect_Percent_LEVEL_2 / 100.0)),
								127 - (127 * (LD_Defect_Percent_LEVEL_3 / 100.0)),
								127 - (127 * (LD_Defect_Percent_LEVEL_4 / 100.0)),
								127 - (127 * (LD_Defect_Percent_LEVEL_5 / 100.0)) };
int LD_BLACK_Bright_LEVEL[] = { 1, // 블랙패턴의 결함은 아직.. 값을 어떻게 가져갈지 논리가 부족함.. 2023-05-12 KYJ
								33,
								53,
								73,
								93 };
int LD_G32_Bright_LEVEL[] = { 32 + (32 * (LD_Defect_Percent_LEVEL_1 / 100.0)),
								32 + (32 * (LD_Defect_Percent_LEVEL_2 / 100.0)),
								32 + (32 * (LD_Defect_Percent_LEVEL_3 / 100.0)),
								32 + (32 * (LD_Defect_Percent_LEVEL_4 / 100.0)),
								32 + (32 * (LD_Defect_Percent_LEVEL_5 / 100.0)) };
int LD_G32_Dark_LEVEL[] = { 32 - (32 * (LD_Defect_Percent_LEVEL_1 / 100.0)),
								32 - (32 * (LD_Defect_Percent_LEVEL_2 / 100.0)),
								32 - (32 * (LD_Defect_Percent_LEVEL_3 / 100.0)),
								32 - (32 * (LD_Defect_Percent_LEVEL_4 / 100.0)),
								32 - (32 * (LD_Defect_Percent_LEVEL_5 / 100.0)) };
int LD_G64_Bright_LEVEL[] = { 64 + (64 * (LD_Defect_Percent_LEVEL_1 / 100.0)),
								64 + (64 * (LD_Defect_Percent_LEVEL_2 / 100.0)),
								64 + (64 * (LD_Defect_Percent_LEVEL_3 / 100.0)),
								64 + (64 * (LD_Defect_Percent_LEVEL_4 / 100.0)),
								64 + (64 * (LD_Defect_Percent_LEVEL_5 / 100.0)) };
int LD_G64_Dark_LEVEL[] = { 64 - (64 * (LD_Defect_Percent_LEVEL_1 / 100.0)),
								64 - (64 * (LD_Defect_Percent_LEVEL_2 / 100.0)),
								64 - (64 * (LD_Defect_Percent_LEVEL_3 / 100.0)),
								64 - (64 * (LD_Defect_Percent_LEVEL_4 / 100.0)),
								64 - (64 * (LD_Defect_Percent_LEVEL_5 / 100.0)) };
int LD_WHITE_Dark_LEVEL[] = { 255 - (255 * (LD_Defect_Percent_LEVEL_1 / 100.0)),
								255 - (255 * (LD_Defect_Percent_LEVEL_2 / 100.0)),
								255 - (255 * (LD_Defect_Percent_LEVEL_3 / 100.0)),
								255 - (255 * (LD_Defect_Percent_LEVEL_4 / 100.0)),
								255 - (255 * (LD_Defect_Percent_LEVEL_5 / 100.0)) };

 //Stain Defect 수준별 밝기값 설정. 1 2 3 4 5 수준
 //ex)G127 1수준 3% = 3.81 
 //30번 그리면서 그라데이션 효과를 줄 것이기 때문에
 //3.81 / 30 = 0.127
 //=> (127 * 0.03) / 30 = 0.127
 //
 //30 은 Stain Drawing 시 for문 반복횟수.
 //흑백 얼룩불량 수준
float Stain_127_Bright_LEVEL[] = { 0.127, 0.55, 0.9736, 1.397, 1.82 };
float Stain_127_Dark_LEVEL[] = { -0.127, -0.55, -0.96, -1.397, -1.82 };
float Stain_BLACK_Bright_LEVEL[] = { 0.03, 0.13, 0.23, 0.33, 0.43 };
float Stain_G32_Bright_LEVEL[] = { 0.03, 0.13, 0.24, 0.35, 0.45 };
float Stain_G32_Dark_LEVEL[] = { -0.03, -0.13, -0.24, -0.35, -0.45 };
float Stain_G64_Bright_LEVEL[] = { 0.064, 0.277, 0.49, 0.704, 0.9173 };
float Stain_G64_Dark_LEVEL[] = { -0.064, -0.277, -0.49, -0.704, -0.9173 };
float Stain_WHITE_Dark_LEVEL[] = { -0.255, -1.105, -1.955, -2.805, -3.655 };
// 컬러 얼룩불량 수준
float Stain_Color_WHITE_Dark_LEVEL[] = { -0.255, -1.105, -1.955, -2.805, -3.655 };

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCOpenCVDlg 대화 상자
CMFCOpenCVDlg::CMFCOpenCVDlg(CWnd* pParent /*=nullptr*/) : CDialogEx(IDD_MFC_OPENCV_DIALOG, pParent) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCOpenCVDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DefectType, m_Combo_DefectType);
	DDX_Control(pDX, IDC_COMBO_DefectPosition, m_Combo_DefectPosition);
	DDX_Control(pDX, IDC_COMBO_Pattern, m_Combo_Pattern);
	DDX_Control(pDX, IDC_PROGRESS2, m_progressBar);
	DDX_Control(pDX, IDC_EDIT_SavePath, m_Edit_SavePath);
}

BEGIN_MESSAGE_MAP(CMFCOpenCVDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Create, &CMFCOpenCVDlg::OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_Preview, &CMFCOpenCVDlg::OnBnClickedButtonPreview)
	ON_BN_CLICKED(IDC_BUTTON_MonoSave, &CMFCOpenCVDlg::OnBnClickedButtonMonosave)
	ON_BN_CLICKED(IDExit, &CMFCOpenCVDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_BUTTON_CanvasClear, &CMFCOpenCVDlg::OnBnClickedButtonCanvasclear)
	ON_BN_CLICKED(IDC_BUTTON_AutoRun, &CMFCOpenCVDlg::OnBnClickedButtonAutorun)
END_MESSAGE_MAP()

// CMFCOpenCVDlg 메시지 처리기
BOOL CMFCOpenCVDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// Program Window Name
	SetWindowText(L"곰두리ㅎㅇ");

	// Canvas 초기화 (PanelSizeY, PanelSizeX, 유형)
	RED = Mat::zeros(PanelSizeY, PanelSizeX, CV_8UC3);
	GREEN = Mat::zeros(PanelSizeY, PanelSizeX, CV_8UC3);
	BLUE = Mat::zeros(PanelSizeY, PanelSizeX, CV_8UC3);
	BLACK = Mat::zeros(PanelSizeY, PanelSizeX, CV_8UC3);
	G32 = Mat::zeros(PanelSizeY, PanelSizeX, CV_8UC3);
	G64 = Mat::zeros(PanelSizeY, PanelSizeX, CV_8UC3);
	G127 = Mat::zeros(PanelSizeY, PanelSizeX, CV_8UC3);
	WHITE = Mat::zeros(PanelSizeY, PanelSizeX, CV_8UC3);
	Color_WHITE_red = Mat::zeros(PanelSizeY, PanelSizeX, CV_8UC3);
	Color_WHITE_green = Mat::zeros(PanelSizeY, PanelSizeX, CV_8UC3);
	Color_WHITE_blue = Mat::zeros(PanelSizeY, PanelSizeX, CV_8UC3);

	// Canvas 초기화 (Blue, Green, Red)
	RED = Scalar(0, 0, 127);
	GREEN = Scalar(0, 127, 0);
	BLUE = Scalar(127, 0, 0);
	BLACK = Scalar(0, 0, 0);
	G32 = Scalar(32, 32, 32);
	G64 = Scalar(64, 64, 64);
	G127 = Scalar(127, 127, 127);
	WHITE = Scalar(255, 255, 255);
	Color_WHITE_red = Scalar(255, 255, 255);
	Color_WHITE_green = Scalar(255, 255, 255);
	Color_WHITE_blue = Scalar(255, 255, 255);

	// Defect Type ComboBox 항목 추가
	m_Combo_DefectType.AddString(_T("Point (Mono)"));
	m_Combo_DefectType.AddString(_T("Line (Mono)"));
	m_Combo_DefectType.AddString(_T("Stain (Mono)"));
	m_Combo_DefectType.AddString(_T("Stain (Color)"));
	m_Combo_DefectType.AddString(_T("Point Stain Location (Mono)")); // 2023-05-10 추가
	m_Combo_DefectType.AddString(_T("Line Location (Mono)")); // 2023-05-10 추가
	m_Combo_DefectType.SetCurSel(0); // 초기값

	// Defect Position ComboBox 항목 추가
	m_Combo_DefectPosition.AddString(_T("1"));
	m_Combo_DefectPosition.AddString(_T("2"));
	m_Combo_DefectPosition.AddString(_T("3"));
	m_Combo_DefectPosition.AddString(_T("4"));
	m_Combo_DefectPosition.AddString(_T("5"));
	m_Combo_DefectPosition.AddString(_T("6"));
	m_Combo_DefectPosition.AddString(_T("7"));
	m_Combo_DefectPosition.AddString(_T("8"));
	m_Combo_DefectPosition.SetCurSel(0);

	// Pattern ComboBox 항목 추가
	m_Combo_Pattern.AddString(_T("RED"));
	m_Combo_Pattern.AddString(_T("GREEN"));
	m_Combo_Pattern.AddString(_T("BLUE"));
	m_Combo_Pattern.AddString(_T("BLACK"));
	m_Combo_Pattern.AddString(_T("G32"));
	m_Combo_Pattern.AddString(_T("G64"));
	m_Combo_Pattern.AddString(_T("G127"));
	m_Combo_Pattern.AddString(_T("WHITE"));
	m_Combo_Pattern.AddString(_T("Color_WHITE_red"));
	m_Combo_Pattern.AddString(_T("Color_WHITE_green"));
	m_Combo_Pattern.AddString(_T("Color_WHITE_blue"));
	m_Combo_Pattern.SetCurSel(0); // 초기값

	m_progressBar.SetRange(0, 280); // ProgressBar 초기화 0~216

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCOpenCVDlg::OnPaint() {
	if (IsIconic()) {
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else {
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCOpenCVDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

// 클래스 멤버 사용자 함수 구현
void CMFCOpenCVDlg::Canvas_Clear(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE, Mat Color_WHITE_red, Mat Color_WHITE_green, Mat Color_WHITE_blue) {
	RED = Scalar(0, 0, 127); // Scalar 함수를 이용해 배경을 다시 덧칠한다.
	GREEN = Scalar(0, 127, 0);
	BLUE = Scalar(127, 0, 0);
	BLACK = Scalar(0, 0, 0);
	G32 = Scalar(32, 32, 32);
	G64 = Scalar(64, 64, 64);
	G127 = Scalar(127, 127, 127);
	WHITE = Scalar(255, 255, 255);

	Color_WHITE_red = Scalar(255, 255, 255);
	Color_WHITE_green = Scalar(255, 255, 255);
	Color_WHITE_blue = Scalar(255, 255, 255);
}
void CMFCOpenCVDlg::Point_Stain_Defect_Position_Select(int* PosX, int* Dark_PosY, int* Bright_PosY, int* Interval, int* Position, bool* AutoRun, int* Cycle) {
	int select = 0; // 아래 switch 문에서 사용, Defect Position을 설정하는 용도.

	if (*AutoRun == true) { // AutoRun 일 경우엔
		select = *Cycle; // Cycle 로 부터 Select 값을 받아온다
	}
	else { // AutoRun이 아니라면 ComboBox에서 선택된 값을 읽어온다.
		select = m_Combo_DefectPosition.GetCurSel();
	}

	switch (select) {
	case 0: // 초기 설정값을 그대로 사용.
		*PosX = 300;
		*Dark_PosY = 200;
		*Bright_PosY = 600;
		*Interval = 350;
		*Position = 1;
		break;
	case 1: // 간격을 350 → 250 변경 & x축 시작위치 +300
		*PosX = 600;
		*Dark_PosY = 200;
		*Bright_PosY = 600;
		*Interval = 250;
		*Position = 2;
		break;
	case 2: // 간격을 350 → 150 변경 & x축 시작위치 +700
		*PosX = 1000;
		*Dark_PosY = 200;
		*Bright_PosY = 600;
		*Interval = 150;
		*Position = 3;
		break;
	case 3: // 간격을 그대로, 암점과 휘점의 Y축 거리를 200px만큼 더 가깝게
		*PosX = 300;
		*Dark_PosY = 300;
		*Bright_PosY = 500;
		*Interval = 350;
		*Position = 4;
		break;
	case 4: // 간격을 350 → 250 변경, 암점과 휘점의 Y축 거리를 200px만큼 더 가깝게 & x축 시작위치 +300
		*PosX = 600;
		*Dark_PosY = 300;
		*Bright_PosY = 500;
		*Interval = 250;
		*Position = 5;
		break;
	case 5: // 간격을 350 → 150 변경, 암점과 휘점의 Y축 거리를 200px만큼 더 가깝게 & x축 시작위치 +700
		*PosX = 1000;
		*Dark_PosY = 300;
		*Bright_PosY = 500;
		*Interval = 150;
		*Position = 6;
		break;
	case 6: // x축 시작위치를 -100px.
		*PosX = 200;
		*Dark_PosY = 200;
		*Bright_PosY = 600;
		*Interval = 350;
		*Position = 7;
		break;
	case 7: // x축 시작위치를 +100px.
		*PosX = 400;
		*Dark_PosY = 200;
		*Bright_PosY = 600;
		*Interval = 350;
		*Position = 8;
		break;
	default:
		MessageBox(_T("Point & Stain Defect Position Setting Error"));
		return;
		break;
	}
}
void CMFCOpenCVDlg::Line_Defect_Position_Select(int* Dark_PosX, int* Bright_PosX, int* Interval, int* Position, bool* AutoRun, int* Cycle) {
	int select = 0;

	if (*AutoRun == true) { // AutoRun 일 경우엔
		select = *Cycle; // Cycle 로 부터 Select 값을 받아온다
	}
	else { // AutoRun이 아니라면 ComboBox에서 선택된 값을 읽어온다.
		select = m_Combo_DefectPosition.GetCurSel();
	}

	switch (select) {
	case 0: // 기본값 그대로 사용.
		*Dark_PosX = 200;
		*Bright_PosX = 1100;
		*Interval = 150;
		*Position = 1;
		break;
	case 1: // 간격 150 → 200 변경. & x축 시작위치 -100
		*Dark_PosX = 100;
		*Bright_PosX = 1000;
		*Interval = 200;
		*Position = 2;
		break;
	case 2: // 간격 150 → 100 변경. & x축 시작위치 +100
		*Dark_PosX = 300;
		*Bright_PosX = 1200;
		*Interval = 100;
		*Position = 3;
		break;
	case 3: // 휘선암선 시작위치 변경 간격 그대로
		*Dark_PosX = 1100;
		*Bright_PosX = 200;
		*Interval = 150;
		*Position = 4;
		break;
	case 4: // 휘선암선 시작위치 변경 간격 150 → 200 & x축 시작위치 -100
		*Dark_PosX = 1000;
		*Bright_PosX = 100;
		*Interval = 200;
		*Position = 5;
		break;
	case 5: // 휘선암선 시작위치 변경 간격 150 → 100 & x축 시작위치 +100
		*Dark_PosX = 1200;
		*Bright_PosX = 300;
		*Interval = 100;
		*Position = 6;
		break;
	case 6: // 시작위치 암200 휘 350 간격 300 & x축 시작위치 +200
		*Dark_PosX = 400;
		*Bright_PosX = 550;
		*Interval = 300;
		*Position = 7;
		break;
	case 7: // 시작위치 암200 휘 350 간격 400 & x축 시작위치 +150
		*Dark_PosX = 350;
		*Bright_PosX = 700;
		*Interval = 400;
		*Position = 8;
		break;
	default:
		MessageBox(_T("Line Defect Position Setting Error"));
		return;
		break;
	}
}
void CMFCOpenCVDlg::Draw_Point(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE, int* Defect_Type, int* Position, bool* AutoRun, int* Cycle) { // PD
	*Defect_Type = 1; // Point Defect 의 Type 은 1
	*Position = 0; // Position 은 0으로 초기화

	int PosX = 300; // Position X축 처음 위치
	int Dark_PosY = 200; // Position 암점의 Y축 처음 위치
	int Bright_PosY = 600; // Position 휘점의 Y축 처음 위치
	int Interval = 350; // Defect 간의 X축 간격
	int Point_Size = 1; // rectangle Size를 2*2로 설정 (아래에서 상세설명)

	/*                                 시작위치            종료위치
	1로 설정함으로써 rectangle (image, (Point(x, y), Point(x + 1, y + 1), scalar(0, 0, 0), 1);
	두 번째 Point 인자값(종료위치)에 1씩 더해주는 역할을 수행한다.
	이는 곧 X축, Y축 의 종료위치에 1씩 추가되므로
	2*2 의 rectangle (사각형) 을 Drawing 한다.
	*/

	// Position Select 함수를 통해서 Defect의 처음위치, 간격 등을 설정한다.
	Point_Stain_Defect_Position_Select(&PosX, &Dark_PosY, &Bright_PosY, &Interval, Position, AutoRun, Cycle);

	for (int i = 0; i < 5; i++, PosX += Interval) { // Interval 값 PosX에 더해 불량의 간격을 증가.
		// RED
		rectangle(RED, Point(PosX, Dark_PosY), Point(PosX + Point_Size, Dark_PosY + Point_Size), Scalar(0, 0, PD_127_Dark_LEVEL[i]), FILLED);
		rectangle(RED, Point(PosX, Bright_PosY), Point(PosX + Point_Size, Bright_PosY + Point_Size), Scalar(0, 0, PD_127_Bright_LEVEL[i]), FILLED);
		// GREEN
		rectangle(GREEN, Point(PosX, Dark_PosY), Point(PosX + Point_Size, Dark_PosY + Point_Size), Scalar(0, PD_127_Dark_LEVEL[i], 0), FILLED);
		rectangle(GREEN, Point(PosX, Bright_PosY), Point(PosX + Point_Size, Bright_PosY + Point_Size), Scalar(0, PD_127_Bright_LEVEL[i], 0), FILLED);
		// BLUE
		rectangle(BLUE, Point(PosX, Dark_PosY), Point(PosX + Point_Size, Dark_PosY + Point_Size), Scalar(PD_127_Dark_LEVEL[i], 0, 0), FILLED);
		rectangle(BLUE, Point(PosX, Bright_PosY), Point(PosX + Point_Size, Bright_PosY + Point_Size), Scalar(PD_127_Bright_LEVEL[i], 0, 0), FILLED);
		// BLACK
		rectangle(BLACK, Point(PosX, Bright_PosY), Point(PosX + Point_Size, Bright_PosY + Point_Size), Scalar(PD_BLACK_Bright_LEVEL[i], PD_BLACK_Bright_LEVEL[i], PD_BLACK_Bright_LEVEL[i]), FILLED);
		// G32
		rectangle(G32, Point(PosX, Dark_PosY), Point(PosX + Point_Size, Dark_PosY + Point_Size), Scalar(PD_G32_Dark_LEVEL[i], PD_G32_Dark_LEVEL[i], PD_G32_Dark_LEVEL[i]), FILLED);
		rectangle(G32, Point(PosX, Bright_PosY), Point(PosX + Point_Size, Bright_PosY + Point_Size), Scalar(PD_G32_Bright_LEVEL[i], PD_G32_Bright_LEVEL[i], PD_G32_Bright_LEVEL[i]), FILLED);
		// G64
		rectangle(G64, Point(PosX, Dark_PosY), Point(PosX + Point_Size, Dark_PosY + Point_Size), Scalar(PD_G64_Dark_LEVEL[i], PD_G64_Dark_LEVEL[i], PD_G64_Dark_LEVEL[i]), FILLED);
		rectangle(G64, Point(PosX, Bright_PosY), Point(PosX + Point_Size, Bright_PosY + Point_Size), Scalar(PD_G64_Bright_LEVEL[i], PD_G64_Bright_LEVEL[i], PD_G64_Bright_LEVEL[i]), FILLED);
		// G127
		rectangle(G127, Point(PosX, Dark_PosY), Point(PosX + Point_Size, Dark_PosY + Point_Size), Scalar(PD_127_Dark_LEVEL[i], PD_127_Dark_LEVEL[i], PD_127_Dark_LEVEL[i]), FILLED);
		rectangle(G127, Point(PosX, Bright_PosY), Point(PosX + Point_Size, Bright_PosY + Point_Size), Scalar(PD_127_Bright_LEVEL[i], PD_127_Bright_LEVEL[i], PD_127_Bright_LEVEL[i]), FILLED);
		// WHITE
		rectangle(WHITE, Point(PosX, Dark_PosY), Point(PosX + Point_Size, Dark_PosY + Point_Size), Scalar(PD_WHITE_Dark_LEVEL[i], PD_WHITE_Dark_LEVEL[i], PD_WHITE_Dark_LEVEL[i]), FILLED);
	}
}
void CMFCOpenCVDlg::Draw_Line(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE, int* Defect_Type, int* Position, bool* AutoRun, int* Cycle) { // LD
	*Defect_Type = 2; // Line Defect 의 Type 은 2
	*Position = 0; // Position 은 0으로 초기화

	int Dark_PosX = 200; // 암선 x축 위치
	int Bright_PosX = 1100; // 휘선 y축 위치
	int Interval = 150; // Defect 간의 X축 간격

	// Position Select 함수를 통해서 Defect의 처음위치, 간격 등을 설정한다.
	Line_Defect_Position_Select(&Dark_PosX, &Bright_PosX, &Interval, Position, AutoRun, Cycle);

	for (int i = 0; i < 5; i++, Dark_PosX += Interval, Bright_PosX += Interval) { // x축 150px 씩 증가
		// RED
		line(RED, Point(Dark_PosX, 0), Point(Dark_PosX, 2940), Scalar(0, 0, LD_127_Dark_LEVEL[i]), 1);
		line(RED, Point(Bright_PosX, 0), Point(Bright_PosX, 2940), Scalar(0, 0, LD_127_Bright_LEVEL[i]), 1);
		// GREEN
		line(GREEN, Point(Dark_PosX, 0), Point(Dark_PosX, 2940), Scalar(0, LD_127_Dark_LEVEL[i], 0), 1);
		line(GREEN, Point(Bright_PosX, 0), Point(Bright_PosX, 2940), Scalar(0, LD_127_Bright_LEVEL[i], 0), 1);
		// BLUE
		line(BLUE, Point(Dark_PosX, 0), Point(Dark_PosX, 2940), Scalar(LD_127_Dark_LEVEL[i], 0, 0), 1);
		line(BLUE, Point(Bright_PosX, 0), Point(Bright_PosX, 2940), Scalar(LD_127_Bright_LEVEL[i], 0, 0), 1);
		// BLACK
		line(BLACK, Point(Bright_PosX, 0), Point(Bright_PosX, 2940), Scalar(LD_BLACK_Bright_LEVEL[i], LD_BLACK_Bright_LEVEL[i], LD_BLACK_Bright_LEVEL[i]), 1);
		// G32
		line(G32, Point(Dark_PosX, 0), Point(Dark_PosX, 2940), Scalar(LD_G32_Dark_LEVEL[i], LD_G32_Dark_LEVEL[i], LD_G32_Dark_LEVEL[i]), 1);
		line(G32, Point(Bright_PosX, 0), Point(Bright_PosX, 2940), Scalar(LD_G32_Bright_LEVEL[i], LD_G32_Bright_LEVEL[i], LD_G32_Bright_LEVEL[i]), 1);
		// G64
		line(G64, Point(Dark_PosX, 0), Point(Dark_PosX, 2940), Scalar(LD_G64_Dark_LEVEL[i], LD_G64_Dark_LEVEL[i], LD_G64_Dark_LEVEL[i]), 1);
		line(G64, Point(Bright_PosX, 0), Point(Bright_PosX, 2940), Scalar(LD_G64_Bright_LEVEL[i], LD_G64_Bright_LEVEL[i], LD_G64_Bright_LEVEL[i]), 1);
		// G127
		line(G127, Point(Dark_PosX, 0), Point(Dark_PosX, 2940), Scalar(LD_127_Dark_LEVEL[i], LD_127_Dark_LEVEL[i], LD_127_Dark_LEVEL[i]), 1);
		line(G127, Point(Bright_PosX, 0), Point(Bright_PosX, 2940), Scalar(LD_127_Bright_LEVEL[i], LD_127_Bright_LEVEL[i], LD_127_Bright_LEVEL[i]), 1);
		// WHITE
		line(WHITE, Point(Dark_PosX, 0), Point(Dark_PosX, 2940), Scalar(LD_WHITE_Dark_LEVEL[i], LD_WHITE_Dark_LEVEL[i], LD_WHITE_Dark_LEVEL[i]), 1);
	}
}
void CMFCOpenCVDlg::Draw_Stain(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE, int* Defect_Type, int* Position, bool* AutoRun, int* Cycle) {
	*Defect_Type = 3; // Stain Defect 의 Type 은 3
	*Position = 0; // Position 은 0으로 초기화

	int PosX = 300; // Position X축 처음 위치
	int Dark_PosY = 200; // Position 어두운 얼룩의 Y축 처음 위치
	int Bright_PosY = 600; // Position 밝은 얼룩의 Y축 처음 위치
	int Interval = 350; // Defect 간의 X축 간격
	int Radius = 30; // Stain의 반지름..
	/*
	Stain Defect 에 그라데이션 효과를 주기 위해서 Circle 함수를 반복하는 형식.
	반복문이 30회 돌면서 Radius 값을 1씩 감소시킨다.
	Radius 는 Circle 함수의 인자값으로 사용되며 Circle의 크기가 1px씩 감소한다.
	*/

	// Position Select 함수를 통해서 Defect의 처음위치, 간격 등을 설정한다.
	Point_Stain_Defect_Position_Select(&PosX, &Dark_PosY, &Bright_PosY, &Interval, Position, AutoRun, Cycle);

	for (int i = 0; i < 5; i++, PosX += Interval) { // Interval 값 PosX에 더해 불량의 간격을 증가.
		// RED
		for (int j = 1; j <= 30; j++, Radius -= 1) { // Circle 함수를 반복실행하여 그라데이션 효과를 준다.
			circle(RED, Point(PosX, Dark_PosY), Radius, Scalar(0, 0, 127 + Stain_127_Dark_LEVEL[i] * j), -1);
			circle(RED, Point(PosX, Bright_PosY), Radius, Scalar(0, 0, 127 + Stain_127_Bright_LEVEL[i] * j), -1);
		}
		Radius = 30;
		// GREEN
		for (int j = 1; j <= 30; j++, Radius -= 1) {
			circle(GREEN, Point(PosX, Dark_PosY), Radius, Scalar(0, 127 + Stain_127_Dark_LEVEL[i] * j, 0), -1);
			circle(GREEN, Point(PosX, Bright_PosY), Radius, Scalar(0, 127 + Stain_127_Bright_LEVEL[i] * j, 0), -1);
		}
		Radius = 30;
		// BLUE
		for (int j = 1; j <= 30; j++, Radius -= 1) {
			circle(BLUE, Point(PosX, Dark_PosY), Radius, Scalar(127 + Stain_127_Dark_LEVEL[i] * j, 0, 0), -1);
			circle(BLUE, Point(PosX, Bright_PosY), Radius, Scalar(127 + Stain_127_Bright_LEVEL[i] * j, 0, 0), -1);
		}
		Radius = 30;
		// BLACK
		for (int j = 1; j <= 30; j++, Radius -= 1) {
			circle(BLACK, Point(PosX, Bright_PosY), Radius, Scalar(Stain_BLACK_Bright_LEVEL[i] * j, Stain_BLACK_Bright_LEVEL[i] * j, Stain_BLACK_Bright_LEVEL[i] * j), -1);
		}
		Radius = 30;
		// G32
		for (int j = 1; j <= 30; j++, Radius -= 1) {
			circle(G32, Point(PosX, Dark_PosY), Radius, Scalar(32 + Stain_G32_Dark_LEVEL[i] * j, 32 + Stain_G32_Dark_LEVEL[i] * j, 32 + Stain_G32_Dark_LEVEL[i] * j), -1);
			circle(G32, Point(PosX, Bright_PosY), Radius, Scalar(32 + Stain_G32_Bright_LEVEL[i] * j, 32 + Stain_G32_Bright_LEVEL[i] * j, 32 + Stain_G32_Bright_LEVEL[i] * j), -1);
		}
		Radius = 30;
		// G64
		for (int j = 1; j <= 30; j++, Radius -= 1) {
			circle(G64, Point(PosX, Dark_PosY), Radius, Scalar(64 + Stain_G64_Dark_LEVEL[i] * j, 64 + Stain_G64_Dark_LEVEL[i] * j, 64 + Stain_G64_Dark_LEVEL[i] * j), -1);
			circle(G64, Point(PosX, Bright_PosY), Radius, Scalar(64 + Stain_G64_Bright_LEVEL[i] * j, 64 + Stain_G64_Bright_LEVEL[i] * j, 64 + Stain_G64_Bright_LEVEL[i] * j), -1);
		}
		Radius = 30;
		// G127
		for (int j = 1; j <= 30; j++, Radius -= 1) {
			circle(G127, Point(PosX, Dark_PosY), Radius, Scalar(127 + Stain_127_Dark_LEVEL[i] * j, 127 + Stain_127_Dark_LEVEL[i] * j, 127 + Stain_127_Dark_LEVEL[i] * j), -1);
			circle(G127, Point(PosX, Bright_PosY), Radius, Scalar(127 + Stain_127_Bright_LEVEL[i] * j, 127 + Stain_127_Bright_LEVEL[i] * j, 127 + Stain_127_Bright_LEVEL[i] * j), -1);
		}
		Radius = 30;
		// WHITE
		for (int j = 1; j <= 30; j++, Radius -= 1) {
			circle(WHITE, Point(PosX, Dark_PosY), Radius, Scalar(255 + Stain_WHITE_Dark_LEVEL[i] * j, 255 + Stain_WHITE_Dark_LEVEL[i] * j, 255 + Stain_WHITE_Dark_LEVEL[i] * j), -1);
		}
		Radius = 30;
	}
}
void CMFCOpenCVDlg::Draw_Stain_Color(Mat Color_WHITE_red, Mat Color_WHITE_green, Mat Color_WHITE_blue, int* Defect_Type, int* Position, bool* AutoRun, int* Cycle) {
	*Defect_Type = 6; // Stain Defect 의 Type 은 6
	*Position = 0; // Position 은 0으로 초기화

	int PosX = 300; // Position X축 처음 위치
	int Dark_PosY = 200; // Position 어두운 얼룩의 Y축 처음 위치이나.. Color Stain 에서는 밝은 얼룩을 Drawing 하고있다.
	int Bright_PosY = 600; // Position 밝은 얼룩의 Y축 처음 위치
	int Interval = 350; // Defect 간의 X축 간격
	int Radius = 30; // Stain의 반지름..
	/*
	Stain Defect 에 그라데이션 효과를 주기 위해서 Circle 함수를 반복하는 형식.
	반복문이 30회 돌면서 Radius 값을 1씩 감소시킨다.
	Radius 는 Circle 함수의 인자값으로 사용되며 Circle의 크기가 1px씩 감소한다.
	*/

	// Position Select 함수를 통해서 Defect의 처음위치, 간격 등을 설정한다.
	Point_Stain_Defect_Position_Select(&PosX, &Dark_PosY, &Bright_PosY, &Interval, Position, AutoRun, Cycle);

	for (int i = 0; i < 5; i++, PosX += Interval) { // Interval 값 PosX에 더해 불량의 간격을 증가.
		// RED
		for (int j = 1; j <= 30; j++, Radius -= 1) {
			circle(Color_WHITE_red, Point(PosX, Dark_PosY), Radius, Scalar(255, 255, 255 + Stain_Color_WHITE_Dark_LEVEL[i] * j), -1);
			circle(Color_WHITE_red, Point(PosX, Bright_PosY), Radius, Scalar(255, 255, 255 + Stain_Color_WHITE_Dark_LEVEL[i] * j), -1);
		}
		Radius = 30;
		// GREEN
		for (int j = 1; j <= 30; j++, Radius -= 1) {
			circle(Color_WHITE_green, Point(PosX, Dark_PosY), Radius, Scalar(255, 255 + Stain_Color_WHITE_Dark_LEVEL[i] * j, 255), -1);
			circle(Color_WHITE_green, Point(PosX, Bright_PosY), Radius, Scalar(255, 255 + Stain_Color_WHITE_Dark_LEVEL[i] * j, 255), -1);
		}
		Radius = 30;
		// BLUE
		for (int j = 1; j <= 30; j++, Radius -= 1) {
			circle(Color_WHITE_blue, Point(PosX, Dark_PosY), Radius, Scalar(255 + Stain_Color_WHITE_Dark_LEVEL[i] * j, 255, 255), -1);
			circle(Color_WHITE_blue, Point(PosX, Bright_PosY), Radius, Scalar(255 + Stain_Color_WHITE_Dark_LEVEL[i] * j, 255, 255), -1);
		}
		Radius = 30;
	}
}
void CMFCOpenCVDlg::Show_Image(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE) {
	// 현재 사용 안 하는 함수
	imshow("RED", RED);
	waitKey(0);
	imshow("GREEN", GREEN);
	waitKey(0);
	imshow("BLUE", BLUE);
	waitKey(0);
	imshow("BLACK", BLACK);
	waitKey(0);
	imshow("G32", G32);
	waitKey(0);
	imshow("G64", G64);
	waitKey(0);
	imshow("G127", G127);
	waitKey(0);
	imshow("WHITE", WHITE);
	waitKey(0);
}
void CMFCOpenCVDlg::Save_Image(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE, Mat Color_WHITE_red, Mat Color_WHITE_green, Mat Color_WHITE_blue, int* Defect_Type, int* Position) {

	// 기본 저장경로
	string save_path = "D:\\Auto_Curved(국책)_그래픽_이미지";
	string temp; // 경로 합치는 용도
	if (Location == true) { // 결함위치 확인용 시료의 저장위치를 따로 설정하기 위해서
		save_path = "D:\\Auto_Curved(국책)_그래픽_이미지\\Location(결함위치 확인용)";
	}

	// make directory 함수에서의 사용을 위해 string -> const char* 형식으로 형변환해준다.
	//char* Create_Directory = const_cast<char*>(save_path.c_str());

	_mkdir(save_path.c_str()); // 적용

	string Defect_Type_path; // Defect Type에 따른 image naming을 위한 변수.
	string Position_path; // Defect Position에 따른 image naming을 위한 변수.	

	switch (*Defect_Type) {
	case 1:
		Defect_Type_path = "\\PD";
		break;
	case 2:
		Defect_Type_path = "\\LD";
		break;
	case 3:
		Defect_Type_path = "\\Stain_Mono";
		break;
	case 4:
		Defect_Type_path = "\\PD_Stain_Location";
		break;
	case 5:
		Defect_Type_path = "\\LD_Location";
		break;
	case 6:
		Defect_Type_path = "\\Stain_Color";
		break;
	default:
		MessageBox(_T("Mono Save Image Defect Type Error"));
		//return;
		break;
	}

	temp = save_path + Defect_Type_path;
	_mkdir(temp.c_str()); // 중간경로 생성.

	switch (*Position) {
	case 1:
		Position_path = "\\01";
		break;
	case 2:
		Position_path = "\\02";
		break;
	case 3:
		Position_path = "\\03";
		break;
	case 4:
		Position_path = "\\04";
		break;
	case 5:
		Position_path = "\\05";
		break;
	case 6:
		Position_path = "\\06";
		break;
	case 7:
		Position_path = "\\07";
		break;
	case 8:
		Position_path = "\\08";
		break;
	default:
		MessageBox(_T("Mono Save Image Defect Position Error"));
		//return;
		break;
	}

	temp = save_path + Defect_Type_path + Position_path;
	_mkdir(temp.c_str()); // 중간경로 생성.

	// bmp 형식으로 지정.
	string RED_path = save_path + Defect_Type_path + Position_path + "\\RED.bmp";
	string GREEN_path = save_path + Defect_Type_path + Position_path + "\\GREEN.bmp";
	string BLUE_path = save_path + Defect_Type_path + Position_path + "\\BLUE.bmp";
	string BLACK_path = save_path + Defect_Type_path + Position_path + "\\BLACK.bmp";
	string G32_path = save_path + Defect_Type_path + Position_path + "\\G32.bmp";
	string G64_path = save_path + Defect_Type_path + Position_path + "\\G64.bmp";
	string G127_path = save_path + Defect_Type_path + Position_path + "\\G127.bmp";
	string WHITE_path = save_path + Defect_Type_path + Position_path + "\\WHITE.bmp";
	
	string Color_WHITE_red_path = save_path + Defect_Type_path + Position_path + "\\WHITE_red.bmp";
	string Color_WHITE_green_path = save_path + Defect_Type_path + Position_path + "\\WHITE_green.bmp";
	string Color_WHITE_blue_path = save_path + Defect_Type_path + Position_path + "\\WHITE_blue.bmp";
	
	// 저장
	imwrite(RED_path, RED);
	imwrite(GREEN_path, GREEN);
	imwrite(BLUE_path, BLUE);
	imwrite(BLACK_path, BLACK);
	imwrite(G32_path, G32);
	imwrite(G64_path, G64);
	imwrite(G127_path, G127);
	imwrite(WHITE_path, WHITE);

	if (Location == false) { // 결함위치 확인용이 아닐 때 Color 이미지를 저장한다.
		imwrite(Color_WHITE_red_path, Color_WHITE_red);
		imwrite(Color_WHITE_green_path, Color_WHITE_green);
		imwrite(Color_WHITE_blue_path, Color_WHITE_blue);
	}

	Location = false; // 저장을 완료하면 false로 바꿔준다.
}

// 2023-05-11 KYJ Color_Save_Image 기능을 Mono 와 통합 및 Mono_Save_Image 함수는 그냥 Save_Image 로 이름 변경
//void CMFCOpenCVDlg::Color_Save_Image(Mat Color_WHITE_red, Mat Color_WHITE_green, Mat Color_WHITE_blue, int* Defect_Type, int* Position) {
//
//	// 기본 저장경로
//	string save_path = "D:\\Auto_Curved(국책)_그래픽_이미지";
//	// make directory 함수에서의 사용을 위해 string -> const char* 형식으로 형변환해준다.
//	char* Create_Directory = const_cast<char*>(save_path.c_str());
//	_mkdir(Create_Directory); // 적용
//
//	string Defect_Type_path;  // Defect Type에 따른 image naming을 위한 변수.
//	string Position_path; // Defect Position에 따른 image naming을 위한 변수.
//
//	switch (*Position) {
//	case 1:
//		Position_path = "\\01_";
//		break;
//	case 2:
//		Position_path = "\\02_";
//		break;
//	case 3:
//		Position_path = "\\03_";
//		break;
//	case 4:
//		Position_path = "\\04_";
//		break;
//	case 5:
//		Position_path = "\\05_";
//		break;
//	case 6:
//		Position_path = "\\06_";
//		break;
//	case 7:
//		Position_path = "\\07_";
//		break;
//	case 8:
//		Position_path = "\\08_";
//		break;
//	default:
//		MessageBox(_T("Color Save Image Defect Position Error"));
//		//return;
//		break;
//	}
//
//	switch (*Defect_Type) {
//	case 3:
//		Defect_Type_path = "Stain_";
//		break;
//	default:
//		MessageBox(_T("Mono Save Image Defect Type Error"));
//		//return;
//		break;
//	}
//
//	// bmp 형식으로 지정.
//	string Color_WHITE_red_path = save_path + Position_path + Defect_Type_path + "WHITE_red.bmp";
//	string Color_WHITE_green_path = save_path + Position_path + Defect_Type_path + "WHITE_green.bmp";
//	string Color_WHITE_blue_path = save_path + Position_path + Defect_Type_path + "WHITE_blue.bmp";
//
//	// 저장
//	imwrite(Color_WHITE_red_path, Color_WHITE_red);
//	imwrite(Color_WHITE_green_path, Color_WHITE_green);
//	imwrite(Color_WHITE_blue_path, Color_WHITE_blue);
//}
void CMFCOpenCVDlg::Draw_Point_Stain_Location(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE, int* Defect_Type, int* Position, bool* AutoRun, int* Cycle, bool* Location) {
	// 2023-05-10 추가된 함수. 결함위치 확인용 드로잉 함수.
	*Defect_Type = 4; // Point Stain Location 의 Type 은 4
	*Position = 0; // Position 은 0으로 초기화
	*Location = true; // 결함 위치 확인용 시료임을 뜻함.

	int PosX = 300; // Position X축 처음 위치
	int Dark_PosY = 200; // Position 어두운 얼룩의 Y축 처음 위치
	int Bright_PosY = 600; // Position 밝은 얼룩의 Y축 처음 위치
	int Interval = 350; // Defect 간의 X축 간격
	int Radius = 15; // Stain의 반지름..

	// Position Select 함수를 통해서 Defect의 처음위치, 간격 등을 설정한다.
	Point_Stain_Defect_Position_Select(&PosX, &Dark_PosY, &Bright_PosY, &Interval, Position, AutoRun, Cycle);

	for (int i = 0; i < 5; i++, PosX += Interval) { // Interval 값 PosX에 더해 불량의 간격을 증가.

		string text = to_string(i+1); // i+1 의 값을 문자열로 변환하여 putText의 인자값으로 사용한다.
		putText(RED, text, Point(PosX - 15, Dark_PosY - 50), 5, 2, Scalar(0, 0, 0), 2, 8);
		putText(GREEN, text, Point(PosX - 15, Dark_PosY - 50), 5, 2, Scalar(0, 0, 0), 2, 8);
		putText(BLUE, text, Point(PosX - 15, Dark_PosY - 50), 5, 2, Scalar(0, 0, 0), 2, 8);
		putText(G32, text, Point(PosX - 15, Dark_PosY - 50), 5, 2, Scalar(0, 0, 0), 2, 8);
		putText(G64, text, Point(PosX - 15, Dark_PosY - 50), 5, 2, Scalar(0, 0, 0), 2, 8);
		putText(G127, text, Point(PosX - 15, Dark_PosY - 50), 5, 2, Scalar(0, 0, 0), 2, 8);
		putText(WHITE, text, Point(PosX - 15, Dark_PosY - 50), 5, 2, Scalar(0, 0, 0), 2, 8);

		text = to_string(i + 6); // i+6 의 값을 문자열로 변환하여 putText의 인자값으로 사용한다.
		putText(RED, text, Point(PosX - 15, Bright_PosY - 50), 5, 2, Scalar(255, 255, 255), 2, 8);
		putText(GREEN, text, Point(PosX - 15, Bright_PosY - 50), 5, 2, Scalar(255, 255, 255), 2, 8);
		putText(BLUE, text, Point(PosX - 15, Bright_PosY - 50), 5, 2, Scalar(255, 255, 255), 2, 8);
		putText(BLACK, text, Point(PosX - 15, Bright_PosY - 50), 5, 2, Scalar(255, 255, 255), 2, 8);
		putText(G32, text, Point(PosX - 15, Bright_PosY - 50), 5, 2, Scalar(255, 255, 255), 2, 8);
		putText(G64, text, Point(PosX - 15, Bright_PosY - 50), 5, 2, Scalar(255, 255, 255), 2, 8);
		putText(G127, text, Point(PosX - 15, Bright_PosY - 50), 5, 2, Scalar(255, 255, 255), 2, 8);

		// RED
		circle(RED, Point(PosX, Dark_PosY), Radius, Scalar(0, 0, 0), -1);
		circle(RED, Point(PosX, Bright_PosY), Radius, Scalar(255, 255, 255), -1);

		// GREEN
		circle(GREEN, Point(PosX, Dark_PosY), Radius, Scalar(0, 0, 0), -1);
		circle(GREEN, Point(PosX, Bright_PosY), Radius, Scalar(255, 255, 255), -1);
		// BLUE
		circle(BLUE, Point(PosX, Dark_PosY), Radius, Scalar(0, 0, 0), -1);
		circle(BLUE, Point(PosX, Bright_PosY), Radius, Scalar(255, 255, 255), -1);
		// BLACK
		circle(BLACK, Point(PosX, Bright_PosY), Radius, Scalar(255, 255, 255), -1);
		// G32
		circle(G32, Point(PosX, Dark_PosY), Radius, Scalar(0, 0, 0), -1);
		circle(G32, Point(PosX, Bright_PosY), Radius, Scalar(255, 255, 255), -1);
		// G64
		circle(G64, Point(PosX, Dark_PosY), Radius, Scalar(0, 0, 0), -1);
		circle(G64, Point(PosX, Bright_PosY), Radius, Scalar(255, 255, 255), -1);
		// G127
		circle(G127, Point(PosX, Dark_PosY), Radius, Scalar(0, 0, 0), -1);
		circle(G127, Point(PosX, Bright_PosY), Radius, Scalar(255, 255, 255), -1);
		// WHITE
		circle(WHITE, Point(PosX, Dark_PosY), Radius, Scalar(0, 0, 0), -1);
	}
}
void CMFCOpenCVDlg::Draw_Line_Location(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE, int* Defect_Type, int* Position, bool* AutoRun, int* Cycle, bool* Location) { // LD
	*Defect_Type = 5; // Line Location 의 Type 은 5
	*Position = 0; // Position 은 0으로 초기화
	*Location = true; // 결함 위치 확인용 시료임을 뜻함.
	
	int Dark_PosX = 200; // 암선 x축 위치
	int Bright_PosX = 1100; // 휘선 y축 위치
	int Interval = 150; // Defect 간의 X축 간격

	// Position Select 함수를 통해서 Defect의 처음위치, 간격 등을 설정한다.
	Line_Defect_Position_Select(&Dark_PosX, &Bright_PosX, &Interval, Position, AutoRun, Cycle);

	for (int i = 0; i < 5; i++, Dark_PosX += Interval, Bright_PosX += Interval) { // x축 150px 씩 증가

		string text = to_string(i + 1); // i+1 의 값을 문자열로 변환하여 putText의 인자값으로 사용한다.
		putText(RED, text, Point(Dark_PosX + 5, 200), 5, 2, Scalar(0, 0, 0), 2, 8);
		putText(GREEN, text, Point(Dark_PosX + 5, 200), 5, 2, Scalar(0, 0, 0), 2, 8);
		putText(BLUE, text, Point(Dark_PosX + 5, 200), 5, 2, Scalar(0, 0, 0), 2, 8);
		putText(G32, text, Point(Dark_PosX + 5, 200), 5, 2, Scalar(0, 0, 0), 2, 8);
		putText(G64, text, Point(Dark_PosX + 5, 200), 5, 2, Scalar(0, 0, 0), 2, 8);
		putText(G127, text, Point(Dark_PosX + 5, 200), 5, 2, Scalar(0, 0, 0), 2, 8);
		putText(WHITE, text, Point(Dark_PosX + 5, 200), 5, 2, Scalar(0, 0, 0), 2, 8);

		text = to_string(i + 6); // i+6 의 값을 문자열로 변환하여 putText의 인자값으로 사용한다.
		putText(RED, text, Point(Bright_PosX + 5, 600), 5, 2, Scalar(255, 255, 255), 2, 8);
		putText(GREEN, text, Point(Bright_PosX + 5, 600), 5, 2, Scalar(255, 255, 255), 2, 8);
		putText(BLUE, text, Point(Bright_PosX + 5, 600), 5, 2, Scalar(255, 255, 255), 2, 8);
		putText(BLACK, text, Point(Bright_PosX + 5, 600), 5, 2, Scalar(255, 255, 255), 2, 8);
		putText(G32, text, Point(Bright_PosX + 5, 600), 5, 2, Scalar(255, 255, 255), 2, 8);
		putText(G64, text, Point(Bright_PosX + 5, 600), 5, 2, Scalar(255, 255, 255), 2, 8);
		putText(G127, text, Point(Bright_PosX + 5, 600), 5, 2, Scalar(255, 255, 255), 2, 8);

		// RED
		line(RED, Point(Dark_PosX, 0), Point(Dark_PosX, 2940), Scalar(0, 0, 0), 1);
		line(RED, Point(Bright_PosX, 0), Point(Bright_PosX, 2940), Scalar(255, 255, 255), 1);
		// GREEN
		line(GREEN, Point(Dark_PosX, 0), Point(Dark_PosX, 2940), Scalar(0, 0, 0), 1);
		line(GREEN, Point(Bright_PosX, 0), Point(Bright_PosX, 2940), Scalar(255, 255, 255), 1);
		// BLUE
		line(BLUE, Point(Dark_PosX, 0), Point(Dark_PosX, 2940), Scalar(0, 0, 0), 1);
		line(BLUE, Point(Bright_PosX, 0), Point(Bright_PosX, 2940), Scalar(255, 255, 255), 1);
		// BLACK
		line(BLACK, Point(Bright_PosX, 0), Point(Bright_PosX, 2940), Scalar(255, 255, 255), 1);
		// G32
		line(G32, Point(Dark_PosX, 0), Point(Dark_PosX, 2940), Scalar(0, 0, 0), 1);
		line(G32, Point(Bright_PosX, 0), Point(Bright_PosX, 2940), Scalar(255, 255, 255), 1);
		// G64
		line(G64, Point(Dark_PosX, 0), Point(Dark_PosX, 2940), Scalar(0, 0, 0), 1);
		line(G64, Point(Bright_PosX, 0), Point(Bright_PosX, 2940), Scalar(255, 255, 255), 1);
		// G127
		line(G127, Point(Dark_PosX, 0), Point(Dark_PosX, 2940), Scalar(0, 0, 0), 1);
		line(G127, Point(Bright_PosX, 0), Point(Bright_PosX, 2940), Scalar(255, 255, 255), 1);
		// WHITE
		line(WHITE, Point(Dark_PosX, 0), Point(Dark_PosX, 2940), Scalar(0, 0, 0), 1);
	}
}

// MFC 컨트롤 함수
void CMFCOpenCVDlg::OnBnClickedButtonCreate() {

	bool create_check = true; // 2023-05-10 생성완료 확인용 추가.
	Defect_Type = m_Combo_DefectType.GetCurSel(); // 첫 번째 ComboBox에서 무엇을 선택했는지 확인
	switch (Defect_Type) {
	case 0: // Point (Mono)
		Draw_Point(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, &Defect_Type, &Position, &AutoRun, &Cycle);
		break;
	case 1: // Line (Mono)
		Draw_Line(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, &Defect_Type, &Position, &AutoRun, &Cycle);
		break;
	case 2: // Stain (Mono)
		Draw_Stain(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, &Defect_Type, &Position, &AutoRun, &Cycle);
		break;
	case 3: // Stain (Color)
		Draw_Stain_Color(Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue, &Defect_Type, &Position, &AutoRun, &Cycle);
		break;
	case 4: // 결함위치 확인용
		Draw_Point_Stain_Location(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, &Defect_Type, &Position, &AutoRun, &Cycle, &Location);
		break;
	case 5:
		Draw_Line_Location(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, &Defect_Type, &Position, &AutoRun, &Cycle, &Location);
		break;
	default: // ComboBox 에서 값을 가져오기 때문에 예외가 발생할 수 없음.
		MessageBox(_T("Defect Type ComboBox Error"));
		create_check = false;
		break;
	}
	if (create_check == true) {
		MessageBox(_T("Create complete!"));
	}
}
void CMFCOpenCVDlg::OnBnClickedButtonPreview() { // 미리보기
	int nPattern = m_Combo_Pattern.GetCurSel();
	switch (nPattern) {
	case 0: // RED
		imshow("RED", RED);
		break;
	case 1: // GREEN
		imshow("GREEN", GREEN);
		break;
	case 2: // BLUE
		imshow("BLUE", BLUE);
		break;
	case 3: // BLACK
		imshow("BLACK", BLACK);
		break;
	case 4: // G32
		imshow("G32", G32);
		break;
	case 5: // G64
		imshow("G64", G64);
		break;
	case 6: // G127
		imshow("G127", G127);
		break;
	case 7: // WHITE
		imshow("WHITE", WHITE);
		break;
	case 8: // Color_WHITE_red
		imshow("Color_WHITE_red", Color_WHITE_red);
		break;
	case 9: // Color_WHITE_green
		imshow("Color_WHITE_green", Color_WHITE_green);
		break;
	case 10: // Color_WHITE_blue
		imshow("Color_WHITE_blue", Color_WHITE_blue);
		break;
	default: // ComboBox 에서 값을 가져오기 때문에 예외가 발생할 수 없음.
		MessageBox(_T("Pattern ComboBox Error"));
		break;
	}
}
void CMFCOpenCVDlg::OnBnClickedButtonMonosave() {
	Save_Image(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue, &Defect_Type, &Position);
	Canvas_Clear(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue);
}

void CMFCOpenCVDlg::OnBnClickedExit() {
	EndDialog(IDD_MFC_OPENCV_DIALOG);
}
void CMFCOpenCVDlg::OnBnClickedButtonCanvasclear() {
	Canvas_Clear(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue);
}
void CMFCOpenCVDlg::OnBnClickedButtonAutorun() {
	AutoRun = true;
	float ProgressBar_Count = 0; // 진행상황 표기용.

	for (int i = 0; i < 8; i++) { // PD 자동 생성
		Draw_Point(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, &Defect_Type, &Position, &AutoRun, &Cycle);
		Save_Image(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue ,&Defect_Type, &Position);
		Canvas_Clear(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue);
		Cycle++; // Cycle에 따라 Position이 자동으로 선택된다.
		ProgressBar_Count += 7.16; // 이 값만큼 증가시키면서
		m_progressBar.SetPos(ProgressBar_Count); // 진행 상황을 보여준다.
	}
	Cycle = 0;

	for (int i = 0; i < 8; i++) { // LD 자동 생성
		Draw_Line(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, &Defect_Type, &Position, &AutoRun, &Cycle);
		Save_Image(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue ,&Defect_Type, &Position);
		Canvas_Clear(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue);
		Cycle++; // Cycle에 따라 Position이 자동으로 선택된다.
		ProgressBar_Count += 7.16; // 이 값만큼 증가시키면서
		m_progressBar.SetPos(ProgressBar_Count); // 진행 상황을 보여준다.
	}
	Cycle = 0;

	for (int i = 0; i < 8; i++) { // Stain 자동 생성
		Draw_Stain(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, &Defect_Type, &Position, &AutoRun, &Cycle);
		Save_Image(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue ,&Defect_Type, &Position);
		Canvas_Clear(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue);
		Cycle++; // Cycle에 따라 Position이 자동으로 선택된다.
		ProgressBar_Count += 7.16; // 이 값만큼 증가시키면서
		m_progressBar.SetPos(ProgressBar_Count); // 진행 상황을 보여준다.
	}
	Cycle = 0;

	for (int i = 0; i < 8; i++) { // Stain 자동 생성
		Draw_Stain_Color(Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue, &Defect_Type, &Position, &AutoRun, &Cycle);
		Save_Image(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue ,&Defect_Type, &Position);
		Canvas_Clear(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue);
		Cycle++; // Cycle에 따라 Position이 자동으로 선택된다.
		ProgressBar_Count += 7.16; // 이 값만큼 증가시키면서
		m_progressBar.SetPos(ProgressBar_Count); // 진행 상황을 보여준다.
	}
	Cycle = 0;

	for (int i = 0; i < 8; i++) { // Point Stain Location 자동 생성.
		Draw_Point_Stain_Location(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, &Defect_Type, &Position, &AutoRun, &Cycle, &Location);
		Save_Image(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue ,&Defect_Type, &Position);
		Canvas_Clear(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue);
		Cycle++; // Cycle에 따라 Position이 자동으로 선택된다.
		ProgressBar_Count += 7.16; // 이 값만큼 증가시키면서
		m_progressBar.SetPos(ProgressBar_Count); // 진행 상황을 보여준다.
	}
	Cycle = 0;

	for (int i = 0; i < 8; i++) { // LD Location 자동 생성
		Draw_Line_Location(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, &Defect_Type, &Position, &AutoRun, &Cycle, &Location);
		Save_Image(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue ,&Defect_Type, &Position);
		Canvas_Clear(RED, GREEN, BLUE, BLACK, G32, G64, G127, WHITE, Color_WHITE_red, Color_WHITE_green, Color_WHITE_blue);
		Cycle++; // Cycle에 따라 Position이 자동으로 선택된다.
		ProgressBar_Count += 7.16; // 이 값만큼 증가시키면서
		m_progressBar.SetPos(ProgressBar_Count); // 진행 상황을 보여준다.
	}
	Cycle = 0;

	MessageBox(_T("Auto Run Complete"));
	m_progressBar.SetPos(0); // AutoRun이 완료되고나면 ProgressBar를 초기화해준다.
	AutoRun = false; // 오토런이 종료되면 false
}