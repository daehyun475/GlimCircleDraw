#pragma once
#include <vector>
#include <thread>
#include <atomic>


class CMFCApplication1Dlg : public CDialogEx
{

public:
	CMFCApplication1Dlg(CWnd* pParent = nullptr);
	~CMFCApplication1Dlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedRanMove();


protected:
	struct Point {
		int x;
		int y;
	};

	std::vector<Point> m_clickPoints;
	int m_radius = 10;
	int m_thickness = 2;
	bool m_dragging = false;
	int m_dragIndex = -1;

	CEdit m_editRadius;
	CEdit m_editThick;

	std::atomic<bool> m_animating;
	std::thread m_animThread;

	ULONG_PTR m_gdiplusToken;

	void DrawCircumCircle(Gdiplus::Graphics* graphics);
	void UpdateUI();
	void UpdateSingleUI(int idx);
	void StartRandomAnimation();
	void StopRandomAnimation();
	void AnimateRandomMove();
	void ResetPoints();
};
