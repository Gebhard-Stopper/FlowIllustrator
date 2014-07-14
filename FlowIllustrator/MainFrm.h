/*
 *	Copyright (C) 2014, Max Planck Institut für Informatik, Saarbrücken.
 *	Implementation: 2014, Gebhard Stopper [ gebhard.stopper@gmail.com ]
 *	
 *	If you perform any changes on this file, please append your name to 
 *	the List of people who worked on this file.
 *
 *	If you add or modify functions or variable, please do not forget to
 *	add/update the doxygen documentation.
 *
 *	This file is part of FlowIllustrator.
 *
 *	FlowIllustrator is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	FlowIllustrator is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with FlowIllustrator.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "Resource.h"
#include "DrawingObjectDataTypes.h"
#include "ListCtrlEx.h"

const UINT WM_VIEW_UPDATE_SELECTED	= RegisterWindowMessage(_T("VIEW_UPDATE_SELECTED"));
const UINT DRAWINGOBJS_SELECT_ITEM	= RegisterWindowMessage(_T("DRAWINGOBJS_SELECT_ITEM"));

class COutlookBar : public CMFCOutlookBar
{
	virtual BOOL AllowShowOnPaneMenu() const { return TRUE; }
	virtual void GetPaneName(CString& strName) const { BOOL bNameValid = strName.LoadString(IDS_OUTLOOKBAR); ASSERT(bNameValid); if (!bNameValid) strName.Empty(); }
};

class CMainFrame : public CFrameWndEx
{
public:
	enum CONTEXT_CAT {NONE, VORTEX, TRIANGLE, RECTANGLE, ELLIPSE};

protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
	BOOL	m_bRectHatched;
	BOOL	m_bEllipseHatched;
	BOOL	m_bTriangleHatched;
	BOOL	m_bRectSolid;
	BOOL	m_bEllipseSolid;
	BOOL	m_bTriangleSolid;
	BOOL	m_bArrowsVortex;
	BOOL	m_bArrowsStreamline;
	BOOL	m_bArrowsPathline;
	BOOL	m_bArrowsStreakline;
	BOOL	m_bStreakLineAsParticles;
	BOOL	m_bVortexAutoSize;
	BOOL	m_bShowTrajectory;
	BOOL	m_bPathlineUseStartframe;
	BOOL	m_bStreaklineUseStartframe;
	BOOL	m_bIsPathlet;
	BOOL	m_bIsStreamlet;
	BOOL	m_bIsStreaklet;
	BOOL	m_bTimelineUseStartFrame;
	BOOL	m_bTimelineShowSeedingline;
	BOOL	m_bTimeLineShowTrajectory;

	float	m_fPixelUnitRatio;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCRibbonBar				m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages			m_PanelImages;
	CMFCRibbonStatusBar			m_wndStatusBar;
	COutlookBar					m_wndNavigationBar;
	CListCtrlEx					m_ObjectList;

	CList<COLORREF,COLORREF>	m_lstMainColors;
	CList<COLORREF,COLORREF>	m_lstAdditionalColors;
	CList<COLORREF,COLORREF>	m_lstStandardColors;

	void (CMainFrame::*m_pActivateFunc)(CDrawingObjectParams *pParams);

	DWORD getCategoryID(int nCatID);

public:
	void UpdateStatusBar(const CString &str, const CString &str2);
	void ActivateContextCategory(int catID, CDrawingObjectParams *pParams, BOOL bAddToSelection);
	void UpdateRibbon(CDrawingObjectParams *pParams);
	void UpdateMainRibbonCategory(const CView *pView);

	void InitDisplayFunctionsDropdown(const CStringArray & arrNames);
	void InitDetectorFunctionsDropdown(const CStringArray & arrNames);
	void InitSpeedLineDropdowns();

	void SetCurrDisplayFunction(UINT nIndex);
	void SetCurrDetectorFunction(UINT nIndex);
	void SetMinMaxIsoValue(float fMin, float fMax);

	void SetCurrentFrame(int nFrame);
	void ActivateSpeedLineContextCategory(CDrawingObjectParams *pParams);

protected:
	void RibbonVortexPropertyInit(CDrawingObjectParams *pParams);
	void RibbonRectPropertyInit(CDrawingObjectParams *pParams);
	void RibbonEllipsePropertyInit(CDrawingObjectParams *pParams);
	void RibbonTrianglePropertyInit(CDrawingObjectParams *pParams);
	void RibbonStreamlinePropertyInit(CDrawingObjectParams *pParams);
	void RibbonPathlinePropertyInit(CDrawingObjectParams *pParams);
	void RibbonStreaklinePropertyInit(CDrawingObjectParams *pParams);
	void RibbonTimelinePropertyInit(CDrawingObjectParams *pParams);

	void InitRibbonDropdown(UINT nID, const CStringArray &arrValues);
	void InitRibbonDropdown(UINT nID, const CStringArray &arrValues, int dwData[]);
	void SetRibbonDropdownCurSel(UINT nID, UINT nIndex);

	CString _getObjectName(CDrawingObject* pObj) const;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMClickListDrawingObjs(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT OnDrawingObjsChanged(WPARAM wParam, LPARAM lParam);

	BOOL CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, int nInitialWidth);

	int FindFocusedOutlookWnd(CMFCOutlookBarTabCtrl** ppOutlookWnd);

	void InitColors(void);
	void InitRibbon(void);
	void _initColorDropDown(UINT nID);

	CMFCOutlookBarTabCtrl* FindOutlookParent(CWnd* pWnd);
	CMFCOutlookBarTabCtrl* m_pCurrOutlookWnd;
	CMFCOutlookBarPane*    m_pCurrOutlookPage;

private:
	void _setRibbonEditValue(DWORD dwCtrlID, float fVal);
	void _setRibbonEditValue(DWORD dwCtrlID, int nVal);
	void _setRibbonEditValue(DWORD dwCtrlID, const CString& str);
	void _setRibbonEditCoordinate2D(DWORD dwCtrlID, float fX, float fY);
	void _setRibbonEditCoordinate2D(DWORD dwCtrlID, const CPointf &point);
	void _setRibbonEditCoordinate2D(DWORD dwCtrlID, const CVector2D &vec);

	void _setRibbonColorValue(DWORD dwCtrlID, const floatColor &color);
	void _setRibbonDropdownCurSel(DWORD dwCtrlID, DWORD_PTR dwItemData);
	void _setRibbonCheckValue(DWORD dwCtrlID, BOOL bCheck);

	BOOL _getRibbonEditValue(DWORD dwCtrlID, float &fVal, float fDefault = 0);
	BOOL _getRibbonEditValue(DWORD dwCtrlID, int &nVal, int nDefault = 0);
	BOOL _getRibbonEditValue(DWORD dwCtrlID, CString &str);

	BOOL _getRibbonEditCoordinate2D(DWORD dwCtrlID, float &fX, float &fY);
	BOOL _getRibbonColorValue(DWORD dwCtrlID, LPFLOATCOLOR pColor);
	BOOL _getRibbonDropdownCurSel(DWORD dwCtrlID, DWORD &nIndex);
	void _getRibbonCheckValue(DWORD dwCtrlID, bool &bCheck);

	void _objectPropertyChanged(CDrawingObjectParams *pParams);

	void (CMainFrame::*_getRibbonUpdateFunction(DWORD dwCatID))(CDrawingObjectParams *pParams);

	void _notifyCild(UINT nMsgId, WPARAM wParam = NULL, LPARAM lParam = NULL);

public:
	afx_msg void OnAnimBeginning();
	afx_msg void OnAnimPrev();
	afx_msg void OnAnimPlay();
	afx_msg void OnAnimNext();
	afx_msg void OnAnimEnd();
	afx_msg void OnUpdateAnimPlay(CCmdUI *pCmdUI);
	afx_msg void OnColorAngle();
	afx_msg void OnColStreamline();
	afx_msg void OnDisplayMode();
	afx_msg void OnFlowToolSelect();
	afx_msg void OnVortexDetector();

	afx_msg void OnRibbonVortexColor();
	afx_msg void OnRibbonVortexThickness();
	afx_msg void OnRibbonVortexCenter();
	afx_msg void OnRibbonVortexRadius1();
	afx_msg void OnRibbonVortexRadius2();
	afx_msg void OnRibbonVortexRotation();
	afx_msg void OnRibbonVortexRevolutions();
	afx_msg void OnRibbonVortexArrowSize();
	afx_msg void OnRibbonVortexAutoSize();
	afx_msg void OnRibbonVortexThreshold();
	afx_msg void OnRibbonVortexNumArrows();
	afx_msg void OnRibbonVortexArrowDir();
	afx_msg void OnRibbonVortexTrajectorySteps();
	afx_msg void OnRibbonVortexSmoothTrajectory();
	afx_msg void OnRibbonVortexStyle();

	afx_msg void OnRibbonRectangleColor();
	afx_msg void OnRibbonRectangleThickness();
	afx_msg void OnRibbonRectSolid();
	afx_msg void OnRibbonRectangleWidth();
	afx_msg void OnRibbonRectangleHeight();
	afx_msg void OnRibbonRectangleCenter();
	afx_msg void OnRibbonRectangleRotation();

	afx_msg void OnRibbonTriangleColor();
	afx_msg void OnRibbonTriangleThickness();
	afx_msg void OnRibbonTriangleSolid();
	afx_msg void OnRibbonTrianglePoint1();
	afx_msg void OnRibbonTrianglePoint2();
	afx_msg void OnRibbonTrianglePoint3();
	afx_msg void OnRibbonTriangleRotation();

	afx_msg void OnRibbonEllipseColor();
	afx_msg void OnRibbonEllipseThickness();
	afx_msg void OnRibbonEllipseSolid();
	afx_msg void OnRibbonEllipseCenter();
	afx_msg void OnRibbonEllipseRadius1();
	afx_msg void OnRibbonEllipseRadius2();
	afx_msg void OnRibbonEllipseRotation();

	afx_msg void OnRibbonStreamLineColor();
	afx_msg void OnRibbonStreamLineThickness();
	afx_msg void OnRibbonStreamLineNumArrows();
	afx_msg void OnRibbonStreamlineArrows();
	afx_msg void OnRibbonStreamLineOrigin();
	afx_msg void OnRibbonStreamLineNumSteps();
	afx_msg void OnRibbonStreamLineStepLen();
	afx_msg void OnRibbonStreamLineArrowSize();
	afx_msg void OnRibbonStreamLineSmoothness();

	afx_msg void OnRibbonPathLineColor();
	afx_msg void OnRibbonPathLineThickness();
	afx_msg void OnRibbonPathLineNumArrows();
	afx_msg void OnRibbonPathlineArrows();
	afx_msg void OnRibbonPathLineOrigin();
	afx_msg void OnRibbonPathLineNumSteps();
	afx_msg void OnRibbonPathLineStepLen();
	afx_msg void OnRibbonPathLineArrowSize();
	afx_msg void OnRibbonPathLineSmoothness();

	afx_msg void OnRibbonStreakLineColor();
	afx_msg void OnRibbonStreakLineThickness();
	afx_msg void OnRibbonStreakLineNumArrows();
	afx_msg void OnRibbonStreaklineArrows();
	afx_msg void OnRibbonStreakLineOrigin();
	afx_msg void OnRibbonStreakLineNumSteps();
	afx_msg void OnRibbonStreakLineStepLen();
	afx_msg void OnRibbonStreakLineArrowSize();
	afx_msg void OnRibbonStreakLineSmoothness();

	afx_msg void OnRibbonPathLinePropertyChanged();

	afx_msg void OnSliderIsoVal();
	afx_msg void OnUpdateRibbonRectSolid(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRibbonEllipseSolid(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRibbonTriangleSolid(CCmdUI *pCmdUI);

	afx_msg void OnRibbonGotoFrame();
	afx_msg void OnStreamlineIntegrationSteps();
	afx_msg void OnStreamlineStepLenght();
	afx_msg void OnVortexThreshold();
	afx_msg void OnUpdateRibbonStreamlineShowArrows(CCmdUI *pCmdUI);
	afx_msg void OnRibbonArrowSize();
	afx_msg void OnRibbonVortexSnapToCore();
	afx_msg void OnRibbonResetViewport();
	afx_msg void OnRibbonToBackground();
	afx_msg void OnRibbinBackward();
	afx_msg void OnRibbonToFront();
	afx_msg void OnRibbinForward();
	afx_msg void OnRibbonLicWidth();
	afx_msg void OnRibbonLicHeight();
	afx_msg void OnRibbonLicKHalfsize();
	afx_msg void OnRibbonLicSeed();
	afx_msg void OnRibbonLicRecalc();
	afx_msg void OnUpdateRibbonVortexAutoAdjustSize(CCmdUI *pCmdUI);
	afx_msg void OnRibbonVortexTrajectoryShow();
	afx_msg void OnUpdateRibbonVortexTrajectoryShow(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRibbonPathlineShowArrows(CCmdUI *pCmdUI);
	afx_msg void OnRibbonAutoupdateVortexTrajectory();
	afx_msg void OnUpdateRibbonAutoupdateVortexTrajectory(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRibbonStreaklineShowArrows(CCmdUI *pCmdUI);
	afx_msg void OnStreaklineAsParticles();
	afx_msg void OnUpdateRibbonStreaklineAsParticles(CCmdUI *pCmdUI);
	afx_msg void OnRibbonStreaklinesAsParticles();
	afx_msg void OnUpdateRibbonStreaklinesAsParticles(CCmdUI *pCmdUI);
	afx_msg void OnRibbonVortexAppearance();
	afx_msg void OnAnimPlayRev();
	afx_msg void OnUpdateAnimPlayRev(CCmdUI *pCmdUI);
	afx_msg void OnRibbonStreaklineStartframe();
	afx_msg void OnRibbonStreaklineUseStartframe();
	afx_msg void OnUpdateRibbonStreaklineUseStartframe(CCmdUI *pCmdUI);
	afx_msg void OnRibbonPathlineStartframe();
	afx_msg void OnRibbonPathlineUseStartframe();
	afx_msg void OnUpdateRibbonPathlineUseStartframe(CCmdUI *pCmdUI);
	afx_msg void OnRibbonStreamlineIsstreamlet();
	afx_msg void OnUpdateRibbonStreamlineIsstreamlet(CCmdUI *pCmdUI);
	afx_msg void OnRibbonPathlineIspathlet();
	afx_msg void OnUpdateRibbonPathlineIspathlet(CCmdUI *pCmdUI);
	afx_msg void OnRibbonStreamlineHalocolor();
	afx_msg void OnRibbonPathlineHalocolor();
	afx_msg void OnRibbonVortexHalocolor();
	afx_msg void OnRibbonVortexArrowColor();
	afx_msg void OnRibbonStreamlineArrowColor();
	afx_msg void OnRibbonPathlineArrowColor();
	afx_msg void OnRibbonShowObjectsWindow();
	afx_msg void OnRibbonStreaklineHalocolor();
	afx_msg void OnRibbonStreaklineArrowcolor();
	afx_msg void OnRibbonRectHatched();
	afx_msg void OnUpdateRibbonRectHatched(CCmdUI *pCmdUI);
	afx_msg void OnRibbonEllipseHatched();
	afx_msg void OnUpdateRibbonEllipseHatched(CCmdUI *pCmdUI);
	afx_msg void OnRibbonTriangleHatched();
	afx_msg void OnUpdateRibbonTriangleHatched(CCmdUI *pCmdUI);
	afx_msg void OnRibbonTimelineColor();
	afx_msg void OnRibbonTimelineHalo();
	afx_msg void OnRibbonTimelineThickness();
	afx_msg void OnRibbonTimelineSmoothness();
	afx_msg void OnRibbonTimelineSeedStart();
	afx_msg void OnRibbonTimelineSeedEnd();
	afx_msg void OnRibbonTimelineSeedlineSamples();
	afx_msg void OnRibbonTimelineStepSize();
	afx_msg void OnRibbonTimelineStartFrame();
	afx_msg void OnRibbonTimelineFixedStartFrame();
	afx_msg void OnUpdateRibbonTimelineFixedStartFrame(CCmdUI *pCmdUI);
	afx_msg void OnRibbonTimelineShowSeedingline();
	afx_msg void OnUpdateRibbonTimelineShowSeedingline(CCmdUI *pCmdUI);
	afx_msg void OnRibbonTimelineIntegrationsteps();
	afx_msg void OnRibbonTimelineShowTrajectory();
	afx_msg void OnUpdateRibbonTimelineShowTrajectory(CCmdUI *pCmdUI);
	afx_msg void OnRibbonSpeedlineColor();
	afx_msg void OnRibbonSpeedlineHalo();
	afx_msg void OnRibbonSpeedlineTransparencymode();
	afx_msg void OnRibbonSpeedlineAlphaMax();
	afx_msg void OnRibbonSpeedlineAlphaMin();
	afx_msg void OnRibbonSpeedlineThicknessmode();
	afx_msg void OnRibbonSpeedlineThicknessMax();
	afx_msg void OnRibbonSpeedlineThicknessMin();
	afx_msg void OnRibbonSpeedlineTimesteps();
	afx_msg void OnRibbonSpeedlineSmooth();
	afx_msg void OnRibbonStreamlineNumstreamlets();
	afx_msg void OnRibbonPathlineNumpathlets();
	afx_msg void OnRibbonStreaklineIsstreaklet();
	afx_msg void OnUpdateRibbonStreaklineIsstreaklet(CCmdUI *pCmdUI);
	afx_msg void OnRibbonStreaklineNumstreaklets();
};


