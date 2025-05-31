#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"
#include <random>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDRESET, &CMFCApplication1Dlg::OnBnClickedReset)
	ON_BN_CLICKED(IDRANMOVE, &CMFCApplication1Dlg::OnBnClickedRanMove)
END_MESSAGE_MAP()

CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);
}

CMFCApplication1Dlg::~CMFCApplication1Dlg() {
	GdiplusShutdown(m_gdiplusToken);
	StopRandomAnimation();
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RADIUS, m_editRadius);
	DDX_Control(pDX, IDC_EDIT_THICK, m_editThick);
}

BOOL CMFCApplication1Dlg::OnInitDialog() {
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	return TRUE;
}

void CMFCApplication1Dlg::OnPaint() {
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(&rect, RGB(255, 255, 255));

	Graphics graphics(dc);

	if (m_clickPoints.size() <= 3) {
		for (const auto& pt : m_clickPoints) {
			SolidBrush brush(Color(255, 0, 0, 0));
			graphics.FillEllipse(&brush, pt.x - m_radius, pt.y - m_radius, m_radius * 2, m_radius * 2);
		}
	}

	if (m_clickPoints.size() == 3) {
		DrawCircumCircle(&graphics);
	}
}

void CMFCApplication1Dlg::OnLButtonDown(UINT nFlags, CPoint point) {
	if (m_clickPoints.size() < 3) {
		CString strRadius;
		m_editRadius.GetWindowTextW(strRadius);
		m_radius = _ttoi(strRadius);

		m_clickPoints.push_back({ point.x, point.y });
		UpdateUI();
		Invalidate();
		return;
	}

	for (int i = 0; i < 3; ++i) {
		int dx = point.x - m_clickPoints[i].x;
		int dy = point.y - m_clickPoints[i].y;
		if (dx * dx + dy * dy <= m_radius * m_radius) {
			m_dragging = true;
			m_dragIndex = i;
			break;
		}
	}
}

void CMFCApplication1Dlg::OnLButtonUp(UINT nFlags, CPoint point) {
	m_dragging = false;
	m_dragIndex = -1;
}

void CMFCApplication1Dlg::OnMouseMove(UINT nFlags, CPoint point) {
	if (m_dragging && m_dragIndex != -1) {
		m_clickPoints[m_dragIndex].x = point.x;
		m_clickPoints[m_dragIndex].y = point.y;
		UpdateUI();
		Invalidate();
	}
}

void CMFCApplication1Dlg::ResetPoints() {
	m_clickPoints.clear();
	m_dragging = false;
	m_dragIndex = -1;
	UpdateUI();
	Invalidate();
}

void CMFCApplication1Dlg::OnBnClickedReset() {
	ResetPoints();
}

void CMFCApplication1Dlg::OnBnClickedRanMove() {
	if (m_animating.load()) return;
	ResetPoints();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> disX(50, 500);
	std::uniform_int_distribution<> disY(50, 400);

	for (int i = 0; i < 3; ++i) {
		m_clickPoints.push_back({ disX(gen), disY(gen) });
	}
	UpdateUI();
	Invalidate();
	StartRandomAnimation();
}

void CMFCApplication1Dlg::DrawCircumCircle(Graphics* graphics) {
	if (m_clickPoints.size() != 3) return;

	auto& A = m_clickPoints[0];
	auto& B = m_clickPoints[1];
	auto& C = m_clickPoints[2];

	double D = 2 * (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y));
	if (D == 0) return;

	double Ux = ((A.x * A.x + A.y * A.y) * (B.y - C.y) +
		(B.x * B.x + B.y * B.y) * (C.y - A.y) +
		(C.x * C.x + C.y * C.y) * (A.y - B.y)) / D;
	double Uy = ((A.x * A.x + A.y * A.y) * (C.x - B.x) +
		(B.x * B.x + B.y * B.y) * (A.x - C.x) +
		(C.x * C.x + C.y * C.y) * (B.x - A.x)) / D;

	double r = sqrt((A.x - Ux) * (A.x - Ux) + (A.y - Uy) * (A.y - Uy));

	CString thickStr;
	m_editThick.GetWindowTextW(thickStr);
	m_thickness = _ttoi(thickStr);

	Pen pen(Color(255, 0, 0, 0), (REAL)m_thickness);
	graphics->DrawEllipse(&pen, (REAL)(Ux - r), (REAL)(Uy - r), (REAL)(r * 2), (REAL)(r * 2));
}

void CMFCApplication1Dlg::UpdateUI() {
	for (int i = 0; i < 3; ++i) {
		UpdateSingleUI(i);
	}
}

void CMFCApplication1Dlg::UpdateSingleUI(int idx) {
	if (idx >= m_clickPoints.size()) return;

	CString str;
	str.Format(_T("Centerpoint%d : (%d, %d)"), idx + 1, m_clickPoints[idx].x, m_clickPoints[idx].y);
	switch (idx) {
	case 0:
		GetDlgItem(IDCENTERPOINT1)->SetWindowTextW(str);
		break;
	case 1:
		GetDlgItem(IDCENTERPOINT2)->SetWindowTextW(str);
		break;
	case 2:
		GetDlgItem(IDCENTERPOINT3)->SetWindowTextW(str);
		break;
	}
}

void CMFCApplication1Dlg::StartRandomAnimation() {
	m_animating = true;
	m_animThread = std::thread(&CMFCApplication1Dlg::AnimateRandomMove, this);
	m_animThread.detach();
}

void CMFCApplication1Dlg::StopRandomAnimation() {
	m_animating = false;
	if (m_animThread.joinable()) m_animThread.join();
}

void CMFCApplication1Dlg::AnimateRandomMove() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> disX(50, 500);
	std::uniform_int_distribution<> disY(50, 400);

	for (int i = 0; i < 10 && m_animating; ++i) {
		for (auto& pt : m_clickPoints) {
			pt.x = disX(gen);
			pt.y = disY(gen);
		}
		UpdateUI();
		Invalidate(FALSE);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	m_animating = false;
}
