// MFC_OpenCVDlg.h: 헤더 파일
//

#pragma once

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <direct.h>

using namespace cv;

// CMFCOpenCVDlg 대화 상자
class CMFCOpenCVDlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCOpenCVDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
private:
	Mat RED, // Canvas 선언
		GREEN,
		BLUE,
		BLACK,
		G32,
		G64,
		G127,
		WHITE,
		Color_WHITE_red,
		Color_WHITE_green,
		Color_WHITE_blue;

	bool AutoRun = false; // AutoRun 유무.. MFC 에서 Button Event로 처리할 것이기 때문에 필요 없을듯??? // 응 아니야 필요해~
	bool Location = false; // 결함위치 확인용 시료의 저장위치를 별도로 관리하기 위해
	int Cycle = 0; // AutoRun 사용 시 Position 자동선택에 관여.
	int Defect_Type = 0; // Defect Type ComboBox 에서 선택된 Item을 구분하는데 사용.
	int Position = 0; // Image 저장 시 몇 번째 Position을 사용했는지 확인하는 용도. ex)01_PD_G127.bmp

public: // 사용자 함수 선언
	void Canvas_Clear(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE, Mat Color_WHITE_red, Mat Color_WHITE_green, Mat Color_WHITE_blue);
	void Point_Stain_Defect_Position_Select(int* PosX, int* Dark_PosY, int* Bright_PosY, int* Interval, int* Position, bool* AutoRun, int* Cycle);
	void Line_Defect_Position_Select(int* Dark_PosX, int* Bright_PosX, int* Interval, int* Position, bool* AutoRun, int* Cycle);
	void Draw_Point(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE, int* Defect_Type, int* Position, bool* AutoRun, int* Cycle);
	void Draw_Line(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE, int* Defect_Type, int* Position, bool* AutoRun, int* Cycle);
	void Draw_Stain(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE, int* Defect_Type, int* Position, bool* AutoRun, int* Cycle);
	void Draw_Stain_Color(Mat Color_WHITE_red, Mat Color_WHITE_green, Mat Color_WHITE_blue, int* Defect_Type, int* Position, bool* AutoRun, int* Cycle);
	void Show_Image(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE);
	// 2023-05-11 KYJ Mono_Save_Image 에서 그냥 Save_Image 로 이름 변경.
	void Save_Image(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE, Mat Color_WHITE_red, Mat Color_WHITE_green, Mat Color_WHITE_blue, int* Defect_Type, int* Position);
	// 2023-05-11 KYJ Color Save Image 함수는 Mono와 통합되었기에 이제 사용 안 함.
	//void Color_Save_Image(Mat Color_WHITE_red, Mat Color_WHITE_green, Mat Color_WHITE_blue, int* Defect_Type, int* Position);
	void Draw_Point_Stain_Location(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE, int* Defect_Type, int* Position, bool* AutoRun, int* Cycle, bool* Location); // 2023-05-10 추가 (결함위치 확인용 드로잉 함수)
	void Draw_Line_Location(Mat RED, Mat GREEN, Mat BLUE, Mat BLACK, Mat G32, Mat G64, Mat G127, Mat WHITE, int* Defect_Type, int* Position, bool* AutoRun, int* Cycle, bool* Location);
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_OPENCV_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_Combo_DefectType;
	CComboBox m_Combo_DefectPosition;
	afx_msg void OnBnClickedButtonCreate();
	afx_msg void OnBnClickedButtonPreview();
	CComboBox m_Combo_Pattern;
	afx_msg void OnBnClickedButtonMonosave();
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedButtonCanvasclear();
	CProgressCtrl m_progressBar;
	afx_msg void OnBnClickedButtonAutorun();
	CEdit m_Edit_SavePath;
};
