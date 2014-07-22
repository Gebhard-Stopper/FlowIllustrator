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

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "FlowIllustrator.h"
#include "FlowIllustratorView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
	Map of EDIT_MODE to the indices of the elements of the FlowTools Gallery in the ribbon bar.
*/
const CFlowIllustratorView::EDIT_MODE FlowToolEditMode [] =
{
	CFlowIllustratorView::EM_SELECT,
	CFlowIllustratorView::EM_VORTEX,
	CFlowIllustratorView::EM_STREAMLINE,
	CFlowIllustratorView::EM_STREAKLINE,
	CFlowIllustratorView::EM_PATHLINE,
	CFlowIllustratorView::EM_TIMELINE,
	CFlowIllustratorView::EM_RECTANGLE,
	CFlowIllustratorView::EM_TRIANGLE,
	CFlowIllustratorView::EM_ELLIPSE,
};

const DWORD LineStyle [] =
{
	SL_THICKNESS_CONST,
	SL_TRIANGLE,
	SL_ARROW,
	SL_DASHED,
	SL_DROPLET,
	SL_ELLIPSE,
};

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_NOTIFY(NM_CLICK, ID_DRAWING_OBJECT_LIST, &CMainFrame::OnNMClickListDrawingObjs)
	ON_REGISTERED_MESSAGE(DRAWINGOBJS_CHANGED, CMainFrame::OnDrawingObjsChanged)

	ON_COMMAND(ID_ANIM_BEGINNING, &CMainFrame::OnAnimBeginning)
	ON_COMMAND(ID_ANIM_PREV, &CMainFrame::OnAnimPrev)
	ON_COMMAND(ID_ANIM_PLAY, &CMainFrame::OnAnimPlay)
	ON_COMMAND(ID_ANIM_NEXT, &CMainFrame::OnAnimNext)
	ON_COMMAND(ID_ANIM_END, &CMainFrame::OnAnimEnd)
	ON_COMMAND(ID_COLOR_ANGLE, &CMainFrame::OnColorAngle)
	ON_COMMAND(ID_COL_STREAMLINE, &CMainFrame::OnColStreamline)
	ON_COMMAND(ID_DISPLAY_MODE, &CMainFrame::OnDisplayMode)
	ON_COMMAND(ID_FLOWTOOL_SELECT, &CMainFrame::OnFlowToolSelect)
	ON_COMMAND(ID_VORTEX_DETECTOR, &CMainFrame::OnVortexDetector)
	ON_COMMAND(ID_SLIDER_ISO_VAL, &CMainFrame::OnSliderIsoVal)
	ON_COMMAND(ID_RIBBON_GOTO_FRAME, &CMainFrame::OnRibbonGotoFrame)
	ON_COMMAND(ID_STREAMLINE_INTEGRATION_STEPS, &CMainFrame::OnStreamlineIntegrationSteps)	//Main category
	ON_COMMAND(ID_STREAMLINE_STEP_LENGHT, &CMainFrame::OnStreamlineStepLenght)				//Main category
	ON_COMMAND(ID_VORTEX_THRESHOLD, &CMainFrame::OnVortexThreshold)							//Main category

	ON_UPDATE_COMMAND_UI(ID_ANIM_PLAY, &CMainFrame::OnUpdateAnimPlay)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_RECT_SOLID, &CMainFrame::OnUpdateRibbonRectSolid)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_ELLIPSE_SOLID, &CMainFrame::OnUpdateRibbonEllipseSolid)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_TRIANGLE_SOLID, &CMainFrame::OnUpdateRibbonTriangleSolid)

	ON_COMMAND(ID_RIBBON_VORTEX_COLOR, &CMainFrame::OnRibbonVortexColor)
	ON_COMMAND(ID_RIBBON_VORTEX_THICKNESS, &CMainFrame::OnRibbonVortexThickness)
	ON_COMMAND(ID_RIBBON_VORTEX_CENTER, &CMainFrame::OnRibbonVortexCenter)
	ON_COMMAND(ID_RIBBON_VORTEX_RADIUS1, &CMainFrame::OnRibbonVortexRadius1)
	ON_COMMAND(ID_RIBBON_VORTEX_RADIUS2, &CMainFrame::OnRibbonVortexRadius2)
	ON_COMMAND(ID_RIBBON_VORTEX_ROTATION, &CMainFrame::OnRibbonVortexRotation)
	ON_COMMAND(ID_RIBBON_VORTEX_REVOLUTIONS, &CMainFrame::OnRibbonVortexRevolutions)
	ON_COMMAND(ID_RIBBON_VORTEX_ARROW_SIZE, &CMainFrame::OnRibbonVortexArrowSize)
	ON_COMMAND(ID_RIBBON_VORTEX_AUTO_ADJUST_SIZE, &CMainFrame::OnRibbonVortexAutoSize)
	ON_COMMAND(ID_RIBBON_VORTEX_THRESHOLD, &CMainFrame::OnRibbonVortexThreshold)
	ON_COMMAND(ID_RIBBON_VORTEX_NUM_ARROWS, &CMainFrame::OnRibbonVortexNumArrows)
	ON_COMMAND(ID_RIBBON_VORTEX_ARROW_DIR, &CMainFrame::OnRibbonVortexArrowDir)
	ON_COMMAND(ID_RIBBON_VORTEX_TRAJECTORY_TIMESTEPS, &CMainFrame::OnRibbonVortexTrajectorySteps)
	ON_COMMAND(ID_RIBBON_VORTEX_TRAJECTORY_CALCULATE, &CMainFrame::OnRibbonVortexTrajectorySteps)
	ON_COMMAND(ID_RIBBON_VORTEX_TRAJECTORY_SMOOTH, &CMainFrame::OnRibbonVortexSmoothTrajectory)
	ON_COMMAND(ID_RIBBON_VORTEX_STYLE, &CMainFrame::OnRibbonVortexStyle)
	
	ON_COMMAND(ID_RIBBON_RECT_COLOR, &CMainFrame::OnRibbonRectangleColor)
	ON_COMMAND(ID_RIBBON_RECT_THICKNESS, &CMainFrame::OnRibbonRectangleThickness)
	ON_COMMAND(ID_RIBBON_RECT_SOLID, &CMainFrame::OnRibbonRectSolid)
	ON_COMMAND(ID_RIBBON_RECT_WIDTH, &CMainFrame::OnRibbonRectangleWidth)
	ON_COMMAND(ID_RIBBON_RECT_HEIGHT, &CMainFrame::OnRibbonRectangleHeight)
	ON_COMMAND(ID_RIBBON_RECT_CENTER, &CMainFrame::OnRibbonRectangleCenter)
	ON_COMMAND(ID_RIBBON_RECT_ROTATION, &CMainFrame::OnRibbonRectangleRotation)

	ON_COMMAND(ID_RIBBON_ELLIPSE_COLOR, &CMainFrame::OnRibbonEllipseColor)
	ON_COMMAND(ID_RIBBON_ELLIPSE_THICKNESS, &CMainFrame::OnRibbonEllipseThickness)
	ON_COMMAND(ID_RIBBON_ELLIPSE_SOLID, &CMainFrame::OnRibbonEllipseSolid)
	ON_COMMAND(ID_RIBBON_ELLIPSE_CENTER, &CMainFrame::OnRibbonEllipseCenter)
	ON_COMMAND(ID_RIBBON_ELLIPSE_RADIUS1, &CMainFrame::OnRibbonEllipseRadius1)
	ON_COMMAND(ID_RIBBON_ELLIPSE_RADIUS2, &CMainFrame::OnRibbonEllipseRadius2)
	ON_COMMAND(ID_RIBBON_ELLIPSE_ROTATION, &CMainFrame::OnRibbonEllipseRotation)

	ON_COMMAND(ID_RIBBON_TRIANGLE_COLOR, &CMainFrame::OnRibbonTriangleColor)
	ON_COMMAND(ID_RIBBON_TRIANGLE_THICKNESS, &CMainFrame::OnRibbonTriangleThickness)
	ON_COMMAND(ID_RIBBON_TRIANGLE_SOLID, &CMainFrame::OnRibbonTriangleSolid)
	ON_COMMAND(ID_RIBBON_TRIANGLE_POINT1, &CMainFrame::OnRibbonTrianglePoint1)
	ON_COMMAND(ID_RIBBON_TRIANGLE_POINT2, &CMainFrame::OnRibbonTrianglePoint2)
	ON_COMMAND(ID_RIBBON_TRIANGLE_POINT3, &CMainFrame::OnRibbonTrianglePoint3)
	ON_COMMAND(ID_RIBBON_TRIANGLE_ROTATION, &CMainFrame::OnRibbonTriangleRotation)
	
	ON_COMMAND(ID_RIBBON_STREAMLINE_COLOR, &CMainFrame::OnRibbonStreamLineColor)
	ON_COMMAND(ID_RIBBON_STREAMLINE_THICKNESS, &CMainFrame::OnRibbonStreamLineThickness)
	ON_COMMAND(ID_RIBBON_STREAMLINE_NUM_ARROWS, &CMainFrame::OnRibbonStreamLineNumArrows)
	ON_COMMAND(ID_RIBBON_STREAMLINE_SHOW_ARROWS, &CMainFrame::OnRibbonStreamlineArrows)
	ON_COMMAND(ID_RIBBON_STREAMLINE_START, &CMainFrame::OnRibbonStreamLineOrigin)
	ON_COMMAND(ID_RIBBON_STREAMLINE_NUM_STEPS, &CMainFrame::OnRibbonStreamLineNumSteps)
	ON_COMMAND(ID_RIBBON_STREAMLINE_STEP_LEN, &CMainFrame::OnRibbonStreamLineStepLen)
	ON_COMMAND(ID_RIBBON_STREAMLINE_ARROW_SIZE, &CMainFrame::OnRibbonStreamLineArrowSize)
	ON_COMMAND(ID_RIBBON_STREAMLINE_SMOOTHNESS, &CMainFrame::OnRibbonStreamLineSmoothness)
	ON_COMMAND(ID_RIBBON_STREAMLINE_ISSTREAMLET, &CMainFrame::OnRibbonStreamlineIsstreamlet)
	ON_COMMAND(ID_RIBBON_STREAMLINE_HALOCOLOR, &CMainFrame::OnRibbonStreamlineHalocolor)
	ON_COMMAND(ID_RIBBON_STREAMLINE_ARROW_COLOR, &CMainFrame::OnRibbonStreamlineArrowColor)
	ON_COMMAND(ID_RIBBON_STREAMLINE_NUMSTREAMLETS, &CMainFrame::OnRibbonStreamlineNumstreamlets)
	ON_COMMAND(ID_RIBBON_STREAMLINE_STYLE, &CMainFrame::OnRibbonStreamlineStyle)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_STREAMLINE_SHOW_ARROWS, &CMainFrame::OnUpdateRibbonStreamlineShowArrows)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_STREAMLINE_ISSTREAMLET, &CMainFrame::OnUpdateRibbonStreamlineIsstreamlet)
	

	ON_COMMAND(ID_RIBBON_PATHLINE_COLOR, &CMainFrame::OnRibbonPathLineColor)
	ON_COMMAND(ID_RIBBON_PATHLINE_THICKNESS, &CMainFrame::OnRibbonPathLineThickness)
	ON_COMMAND(ID_RIBBON_PATHLINE_NUM_ARROWS, &CMainFrame::OnRibbonPathLineNumArrows)
	ON_COMMAND(ID_RIBBON_PATHLINE_SHOW_ARROWS, &CMainFrame::OnRibbonPathlineArrows)
	ON_COMMAND(ID_RIBBON_PATHLINE_START, &CMainFrame::OnRibbonPathLineOrigin)
	ON_COMMAND(ID_RIBBON_PATHLINE_NUM_STEPS, &CMainFrame::OnRibbonPathLineNumSteps)
	ON_COMMAND(ID_RIBBON_PATHLINE_STEP_LEN, &CMainFrame::OnRibbonPathLineStepLen)
	ON_COMMAND(ID_RIBBON_PATHLINE_ARROW_SIZE, &CMainFrame::OnRibbonPathLineArrowSize)
	ON_COMMAND(ID_RIBBON_PATHLINE_SMOOTHNESS, &CMainFrame::OnRibbonPathLineSmoothness)

	ON_COMMAND(ID_RIBBON_STREAKLINE_COLOR, &CMainFrame::OnRibbonStreakLineColor)
	ON_COMMAND(ID_RIBBON_STREAKLINE_THICKNESS, &CMainFrame::OnRibbonStreakLineThickness)
	ON_COMMAND(ID_RIBBON_STREAKLINE_NUM_ARROWS, &CMainFrame::OnRibbonStreakLineNumArrows)
	ON_COMMAND(ID_RIBBON_STREAKLINE_SHOW_ARROWS, &CMainFrame::OnRibbonStreaklineArrows)
	ON_COMMAND(ID_RIBBON_STREAKLINE_ORIGIN, &CMainFrame::OnRibbonStreakLineOrigin)
	ON_COMMAND(ID_RIBBON_STREAKLINE_NUM_STEPS, &CMainFrame::OnRibbonStreakLineNumSteps)
	ON_COMMAND(ID_RIBBON_STREAKLINE_STEP_LEN, &CMainFrame::OnRibbonStreakLineStepLen)
	ON_COMMAND(ID_RIBBON_STREAKLINE_ARROW_SIZE, &CMainFrame::OnRibbonStreakLineArrowSize)
	ON_COMMAND(ID_RIBBON_STREAKLINE_SMOOTHNESS, &CMainFrame::OnRibbonStreakLineSmoothness)




	ON_COMMAND(ID_RIBBON_ARROW_SIZE, &CMainFrame::OnRibbonArrowSize)
	ON_COMMAND(ID_RIBBON_VORTEX_SNAP_TO_CORE, &CMainFrame::OnRibbonVortexSnapToCore)
	ON_COMMAND(ID_RIBBON_RESET_VIEWPORT, &CMainFrame::OnRibbonResetViewport)
	ON_COMMAND(ID_RIBBON_TO_BACKGROUND, &CMainFrame::OnRibbonToBackground)
	ON_COMMAND(ID_RIBBON_BACKWARD, &CMainFrame::OnRibbinBackward)
	ON_COMMAND(ID_RIBBON_TO_FRONT, &CMainFrame::OnRibbonToFront)
	ON_COMMAND(ID_RIBBON_FORWARD, &CMainFrame::OnRibbinForward)
	ON_COMMAND(ID_RIBBON_LIC_WIDTH, &CMainFrame::OnRibbonLicWidth)
	ON_COMMAND(ID_RIBBON_LIC_HEIGHT, &CMainFrame::OnRibbonLicHeight)
	ON_COMMAND(ID_RIBBON_LIC_K_HALFSIZE, &CMainFrame::OnRibbonLicKHalfsize)
	ON_COMMAND(ID_RIBBON_LIC_SEED, &CMainFrame::OnRibbonLicSeed)
	ON_COMMAND(ID_RIBBON_LIC_RECALC, &CMainFrame::OnRibbonLicRecalc)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_VORTEX_AUTO_ADJUST_SIZE, &CMainFrame::OnUpdateRibbonVortexAutoAdjustSize)
	ON_COMMAND(ID_RIBBON_VORTEX_TRAJECTORY_SHOW, &CMainFrame::OnRibbonVortexTrajectoryShow)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_VORTEX_TRAJECTORY_SHOW, &CMainFrame::OnUpdateRibbonVortexTrajectoryShow)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_PATHLINE_SHOW_ARROWS, &CMainFrame::OnUpdateRibbonPathlineShowArrows)
	ON_COMMAND(ID_RIBBON_AUTOUPDATE_VORTEX_TRAJECTORY, &CMainFrame::OnRibbonAutoupdateVortexTrajectory)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_AUTOUPDATE_VORTEX_TRAJECTORY, &CMainFrame::OnUpdateRibbonAutoupdateVortexTrajectory)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_STREAKLINE_SHOW_ARROWS, &CMainFrame::OnUpdateRibbonStreaklineShowArrows)
	ON_COMMAND(ID_STREAKLINE_AS_PARTICLES, &CMainFrame::OnStreaklineAsParticles)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_STREAKLINE_AS_PARTICLES, &CMainFrame::OnUpdateRibbonStreaklineAsParticles)
	ON_COMMAND(ID_RIBBON_STREAKLINES_AS_PARTICLES, &CMainFrame::OnRibbonStreaklinesAsParticles)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_STREAKLINES_AS_PARTICLES, &CMainFrame::OnUpdateRibbonStreaklinesAsParticles)
	ON_COMMAND(ID_RIBBON_VORTEX_APPEARANCE, &CMainFrame::OnRibbonVortexAppearance)
	ON_COMMAND(ID_ANIM_PLAY_REV, &CMainFrame::OnAnimPlayRev)
	ON_UPDATE_COMMAND_UI(ID_ANIM_PLAY_REV, &CMainFrame::OnUpdateAnimPlayRev)
	ON_COMMAND(ID_RIBBON_STREAKLINE_STARTFRAME, &CMainFrame::OnRibbonStreaklineStartframe)
	ON_COMMAND(ID_RIBBON_STREAKLINE_USE_STARTFRAME, &CMainFrame::OnRibbonStreaklineUseStartframe)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_STREAKLINE_USE_STARTFRAME, &CMainFrame::OnUpdateRibbonStreaklineUseStartframe)
	ON_COMMAND(ID_RIBBON_PATHLINE_STARTFRAME, &CMainFrame::OnRibbonPathlineStartframe)
	ON_COMMAND(ID_RIBBON_PATHLINE_USE_STARTFRAME, &CMainFrame::OnRibbonPathlineUseStartframe)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_PATHLINE_USE_STARTFRAME, &CMainFrame::OnUpdateRibbonPathlineUseStartframe)
	
	
	ON_COMMAND(ID_RIBBON_PATHLINE_ISPATHLET, &CMainFrame::OnRibbonPathlineIspathlet)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_PATHLINE_ISPATHLET, &CMainFrame::OnUpdateRibbonPathlineIspathlet)
	
	ON_COMMAND(ID_RIBBON_PATHLINE_HALOCOLOR, &CMainFrame::OnRibbonPathlineHalocolor)
	ON_COMMAND(ID_RIBBON_VORTEX_HALOCOLOR, &CMainFrame::OnRibbonVortexHalocolor)
	ON_COMMAND(ID_RIBBON_VORTEX_ARROW_COLOR, &CMainFrame::OnRibbonVortexArrowColor)
	
	ON_COMMAND(ID_RIBBON_PATHLINE_ARROW_COLOR, &CMainFrame::OnRibbonPathlineArrowColor)
	ON_COMMAND(ID_RIBBON_SHOW_OBJECTS_WINDOW, &CMainFrame::OnRibbonShowObjectsWindow)
	ON_COMMAND(ID_RIBBON_STREAKLINE_HALOCOLOR, &CMainFrame::OnRibbonStreaklineHalocolor)
	ON_COMMAND(ID_RIBBON_STREAKLINE_ARROWCOLOR, &CMainFrame::OnRibbonStreaklineArrowcolor)
	ON_COMMAND(ID_RIBBON_RECT_HATCHED, &CMainFrame::OnRibbonRectHatched)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_RECT_HATCHED, &CMainFrame::OnUpdateRibbonRectHatched)
	ON_COMMAND(ID_RIBBON_ELLIPSE_HATCHED, &CMainFrame::OnRibbonEllipseHatched)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_ELLIPSE_HATCHED, &CMainFrame::OnUpdateRibbonEllipseHatched)
	ON_COMMAND(ID_RIBBON_TRIANGLE_HATCHED, &CMainFrame::OnRibbonTriangleHatched)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_TRIANGLE_HATCHED, &CMainFrame::OnUpdateRibbonTriangleHatched)
	ON_COMMAND(ID_RIBBON_TIMELINE_COLOR, &CMainFrame::OnRibbonTimelineColor)
	ON_COMMAND(ID_RIBBON_TIMELINE_HALOCOLOR, &CMainFrame::OnRibbonTimelineHalo)
	ON_COMMAND(ID_RIBBON_TIMELINE_THICKNESS, &CMainFrame::OnRibbonTimelineThickness)
	ON_COMMAND(ID_RIBBON_TIMELINE_SMOOTHNESS, &CMainFrame::OnRibbonTimelineSmoothness)
	ON_COMMAND(ID_RIBBON_TIMELINE_SEED_START, &CMainFrame::OnRibbonTimelineSeedStart)
	ON_COMMAND(ID_RIBBON_TIMELINE_SEED_END, &CMainFrame::OnRibbonTimelineSeedEnd)
	ON_COMMAND(ID_RIBBON_TIMELINE_SEEDLINE_SAMPLES, &CMainFrame::OnRibbonTimelineSeedlineSamples)
	ON_COMMAND(ID_RIBBON_TIMELINE_STEP_SIZE, &CMainFrame::OnRibbonTimelineStepSize)
	ON_COMMAND(ID_RIBBON_TIMELINE_START_FRAME, &CMainFrame::OnRibbonTimelineStartFrame)
	ON_COMMAND(ID_RIBBON_TIMELINE_FIXED_START_FRAME, &CMainFrame::OnRibbonTimelineFixedStartFrame)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_TIMELINE_FIXED_START_FRAME, &CMainFrame::OnUpdateRibbonTimelineFixedStartFrame)
	ON_COMMAND(ID_RIBBON_TIMELINE_SHOW_SEEDINGLINE, &CMainFrame::OnRibbonTimelineShowSeedingline)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_TIMELINE_SHOW_SEEDINGLINE, &CMainFrame::OnUpdateRibbonTimelineShowSeedingline)
	ON_COMMAND(ID_RIBBON_TIMELINE_INTEGRATIONSTEPS, &CMainFrame::OnRibbonTimelineIntegrationsteps)
	ON_COMMAND(ID_RIBBON_TIMELINE_SHOW_TRAJECTORY, &CMainFrame::OnRibbonTimelineShowTrajectory)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_TIMELINE_SHOW_TRAJECTORY, &CMainFrame::OnUpdateRibbonTimelineShowTrajectory)
	ON_COMMAND(ID_RIBBON_SPEEDLINE_COLOR, &CMainFrame::OnRibbonSpeedlineColor)
	ON_COMMAND(ID_RIBBON_SPEEDLINE_HALO, &CMainFrame::OnRibbonSpeedlineHalo)
	ON_COMMAND(ID_RIBBON_SPEEDLINE_TRANSPARENCYMODE, &CMainFrame::OnRibbonSpeedlineTransparencymode)
	ON_COMMAND(ID_RIBBON_SPEEDLINE_ALPHA_MAX, &CMainFrame::OnRibbonSpeedlineAlphaMax)
	ON_COMMAND(ID_RIBBON_SPEEDLINE_ALPHA_MIN, &CMainFrame::OnRibbonSpeedlineAlphaMin)
	ON_COMMAND(ID_RIBBON_SPEEDLINE_THICKNESSMODE, &CMainFrame::OnRibbonSpeedlineThicknessmode)
	ON_COMMAND(ID_RIBBON_SPEEDLINE_THICKNESS_MAX, &CMainFrame::OnRibbonSpeedlineThicknessMax)
	ON_COMMAND(ID_RIBBON_SPEEDLINE_THICKNESS_MIN, &CMainFrame::OnRibbonSpeedlineThicknessMin)
	ON_COMMAND(ID_RIBBON_SPEEDLINE_TIMESTEPS, &CMainFrame::OnRibbonSpeedlineTimesteps)
	ON_COMMAND(ID_RIBBON_SPEEDLINE_SMOOTH, &CMainFrame::OnRibbonSpeedlineSmooth)
	
	ON_COMMAND(ID_RIBBON_PATHLINE_NUMPATHLETS, &CMainFrame::OnRibbonPathlineNumpathlets)
	ON_COMMAND(ID_RIBBON_STREAKLINE_ISSTREAKLET, &CMainFrame::OnRibbonStreaklineIsstreaklet)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_STREAKLINE_ISSTREAKLET, &CMainFrame::OnUpdateRibbonStreaklineIsstreaklet)
	ON_COMMAND(ID_RIBBON_STREAKLINE_NUMSTREAKLETS, &CMainFrame::OnRibbonStreaklineNumstreaklets)
	
	ON_COMMAND(ID_RIBBON_STREAKLINE_STYLE, &CMainFrame::OnRibbonStreaklineStyle)
	ON_COMMAND(ID_RIBBON_PATHLINE_STYLE, &CMainFrame::OnRibbonPathlineStyle)
	END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_pActivateFunc		= nullptr;
	m_fPixelUnitRatio	= 1.0f;
}

CMainFrame::~CMainFrame()
{
	m_lstMainColors.RemoveAll();
	m_lstAdditionalColors.RemoveAll();
	m_lstStandardColors.RemoveAll();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	// set the visual manager used to draw all user interface elements
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);

	CMFCRibbonStatusBarPane* pElement = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE);
	pElement->SetAlmostLargeText(_T("DimX: 12345  DimY: 12345  DimZ: 12345  Streamline length: 1234567890"));
	m_wndStatusBar.AddElement(pElement, strTitlePane1);
	
	
	CMFCRibbonStatusBarPane* pElement1 = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE);
	pElement1->SetAlmostLargeText(_T("X:1234567890  Y:1234567890"));
	m_wndStatusBar.AddExtendedElement(pElement1, strTitlePane2);
	

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Navigation pane will be created at left, so temporary disable docking at the left side:
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

	// Create and setup "Outlook" navigation bar:
	if (!CreateOutlookBar(m_wndNavigationBar, ID_VIEW_NAVIGATION, 250))
	{
		TRACE0("Failed to create navigation pane\n");
		return -1;      // fail to create
	}

	// Outlook bar is created and docking on the left side should be allowed.
	EnableDocking(CBRS_ALIGN_LEFT);
	EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

	InitRibbon();


	DrawingObjNames.Init();
	

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, int nInitialWidth)
{
	bar.SetMode2003();

	BOOL bNameValid;
	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_SHORTCUTS);
	ASSERT(bNameValid);
	if (!bar.Create(strTemp, this, CRect(0, 0, nInitialWidth, 32000), uiID, WS_CHILD | WS_VISIBLE | CBRS_LEFT))
	{
		return FALSE; // fail to create
	}

	CMFCOutlookBarTabCtrl* pOutlookBar = (CMFCOutlookBarTabCtrl*)bar.GetUnderlyingWindow();

	if (pOutlookBar == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pOutlookBar->EnableInPlaceEdit(TRUE);

	static UINT uiPageID = 1;

	// can float, can autohide, can resize, CAN NOT CLOSE
	DWORD dwStyle = AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE;

	CRect rectDummy (0, 0, nInitialWidth, nInitialWidth);

	m_ObjectList.Create(	WS_CHILD | LBS_NOTIFY | WS_VISIBLE | WS_BORDER| LVS_OWNERDRAWFIXED | 
							LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_SHOWSELALWAYS,
							rectDummy, this, ID_DRAWING_OBJECT_LIST);

	m_ObjectList.SetExtendedStyle(m_ObjectList.GetExtendedStyle() | LVS_EX_DOUBLEBUFFER | LVS_EX_FLATSB);
		
	CRect r;
	m_ObjectList.GetClientRect(&r);
	m_ObjectList.InsertColumn(0, _T("Objects"), LVCFMT_LEFT, r.Width()-18);

	bNameValid = strTemp.LoadString(ID_DRAWING_OBJECT_LIST);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&m_ObjectList, strTemp, 2, TRUE, dwStyle);

	bar.SetPaneStyle(bar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	pOutlookBar->SetImageList(theApp.m_bHiColorIcons ? IDB_PAGES_HC : IDB_PAGES, 24);
	pOutlookBar->SetToolbarImageList(theApp.m_bHiColorIcons ? IDB_PAGES_SMALL_HC : IDB_PAGES_SMALL, 16);
	pOutlookBar->RecalcLayout();

	BOOL bAnimation = theApp.GetInt(_T("OutlookAnimation"), TRUE);
	CMFCOutlookBarTabCtrl::EnableAnimation(bAnimation);

	bar.SetButtonsFont(&afxGlobalData.fontBold);

	bar.ShowWindow(SW_SHOW);

	return TRUE;
}

void CMainFrame::OnNMClickListDrawingObjs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	BOOL bAddToSelection = (pNMItemActivate->uKeyFlags & LVKF_CONTROL);
	
	CView* pView = GetActiveView();
	if (pView) {
		pView->SendMessage(DRAWINGOBJS_SELECT_ITEM, WPARAM(pNMItemActivate->iItem), LPARAM(bAddToSelection));
	}

	*pResult = 0;
}

LRESULT CMainFrame::OnDrawingObjsChanged(WPARAM wParam, LPARAM /*lParam*/)
{
	m_ObjectList.DeleteAllItems();

	CDrawingObjectMngr* pMngr = reinterpret_cast<CDrawingObjectMngr*>(wParam);
	
	if (pMngr) 
	{
		for (size_t i=0; i<pMngr->Size(); i++)
		{
			shared_ptr<CDrawingObject> pObj = pMngr->GetAt(i);

			m_ObjectList.InsertItem(LVIF_TEXT | LVIF_STATE, i, _getObjectName(pObj.get()), 
				pObj->isSelected()? LVIS_SELECTED : 0, LVIS_SELECTED, 0, 0);
			m_ObjectList.SetItemData(i, pObj->GetColorWin32());
		}
	}

	return 0;
}

CString CMainFrame::_getObjectName(CDrawingObject* pObj) const
{
	switch(pObj->GetType())
	{
		case DO_VORTEX:		return _T("Vortex");
		case DO_STREAMLINE: return _T("Stream line");
		case DO_STREAKLINE: return _T("Streak line");
		case DO_PATHLINE:	return _T("Path line");
		case DO_TIMELINE:	return _T("Time line");
		case DO_RECTANGLE:	return _T("Rectangle");
		case DO_TRIANGLE:	return _T("Triangle");
		case DO_ELLIPSE:	return _T("Ellipse");
		case DO_LINE:		return _T("Line");
		case DO_POINT:		return _T("Point");
		case DO_BSPLINE:	return _T("B-Spline");
		case DO_ARROW:		return _T("Arrow");
		case DO_POLYLINE:	return _T("Poly line");
		case DO_SPEEDLINE:	return _T("Speed line");
		default: return _T("");
	}
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers


void CMainFrame::OnAnimBeginning()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	pView->JumpToFirstFrame();
}


void CMainFrame::OnAnimPrev()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	pView->JumpToPreviousFrame();
}


void CMainFrame::OnAnimPlay()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	pView->TogglePlayback(1);
}

void CMainFrame::OnAnimPlayRev()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	pView->TogglePlayback(2);
}


void CMainFrame::OnAnimNext()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	pView->JumpToNextFrame();
}


void CMainFrame::OnAnimEnd()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	pView->JumpToLastFrame();
}


void CMainFrame::OnUpdateAnimPlay(CCmdUI *pCmdUI)
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	pCmdUI->SetCheck(pView->IsPlayingFwd());
}

void CMainFrame::OnUpdateAnimPlayRev(CCmdUI *pCmdUI)
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	pCmdUI->SetCheck(pView->IsPlayingBckwd());
}

void CMainFrame::UpdateStatusBar(const CString &str, const CString &str2)
{
	CMFCRibbonStatusBarPane* pElement = reinterpret_cast<CMFCRibbonStatusBarPane*>(m_wndStatusBar.GetElement(0));
	if (pElement)
	{
		pElement->SetText(str);
		pElement->Redraw();
	}

	CMFCRibbonStatusBarPane* pElement1 = reinterpret_cast<CMFCRibbonStatusBarPane*>(m_wndStatusBar.GetExElement(0));
	if (pElement1)
	{
		pElement1->SetText(str2);
		pElement1->Redraw();
	}
}

void CMainFrame::OnColorAngle()
{
	CMFCRibbonBar *pRibbon = GetRibbonBar();
	if (pRibbon)
	{
		CMFCRibbonSlider *pSlider = reinterpret_cast<CMFCRibbonSlider*>(pRibbon->FindByID(ID_COLOR_ANGLE, FALSE));

		if (pSlider)
		{
			CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
			pView->SetColrOffset(pSlider->GetPos());
		}
	}
}

#define COLOR_STEPS 10

void CMainFrame::InitColors(void)
{
	typedef struct
	{
		COLORREF color;
		TCHAR* szName;
	}
	ColorTableEntry;

	int i = 0;
	int nNumColours = 0;

	static ColorTableEntry colorsMain [] =
	{
		{ RGB(255, 255, 255), _T("White, Background 1") },
		{ RGB(0, 0, 0), _T("Black, Text 1") },
		{ RGB(238, 236, 225), _T("Tan, Background 2") },
		{ RGB(31, 73, 125), _T("Dark Blue, Text 2") },
		{ RGB(79, 129, 189), _T("Blue, Accent 1") },
		{ RGB(192, 80, 77), _T("Red, Accent 2") },
		{ RGB(155, 187, 89), _T("Olive Green, Accent 3") },
		{ RGB(128, 100, 162), _T("Purple, Accent 4") },
		{ RGB(75, 172, 198), _T("Aqua, Accent 5") },
		{ RGB(245, 150, 70), _T("Orange, Accent 6") }
	};

	static ColorTableEntry colorsAdditional [] =
	{
		{ RGB(242, 242, 242), _T("White, Background 1, Darker 5%") },
		{ RGB(127, 127, 127), _T("Black, Text 1, Lighter 50%") },
		{ RGB(214, 212, 202), _T("Tan, Background 2, Darker 10%") },
		{ RGB(210, 218, 229), _T("Dark Blue, Text 2, Lighter 80%") },
		{ RGB(217, 228, 240), _T("Blue, Accent 1, Lighter 80%") },
		{ RGB(244, 219, 218), _T("Red, Accent 2, Lighter 80%") },
		{ RGB(234, 241, 221), _T("Olive Green, Accent 3, Lighter 80%")},
		{ RGB(229, 223, 235), _T("Purple, Accent 4, Lighter 80%") },
		{ RGB(216, 237, 242), _T("Aqua, Accent 5, Lighter 80%") },
		{ RGB(255, 234, 218), _T("Orange, Accent 6, Lighter 80%") },
		{ RGB(215, 215, 215), _T("White, Background 1, Darker 15%") },
		{ RGB(89, 89, 89), _T("Black, Text 1, Lighter 35%") },
		{ RGB(177, 176, 167), _T("Tan, Background 2, Darker 25%") },
		{ RGB(161, 180, 201), _T("Dark Blue, Text 2, Lighter 60%") },
		{ RGB(179, 202, 226), _T("Blue, Accent 1, Lighter 60%") },
		{ RGB(233, 184, 182), _T("Red, Accent 2, Lighter 60%") },
		{ RGB(213, 226, 188), _T("Olive Green, Accent 3, Lighter 60%")},
		{ RGB(203, 191, 215), _T("Purple, Accent 4, Lighter 60%") },
		{ RGB(176, 220, 231), _T("Aqua, Accent 5, Lighter 60%") },
		{ RGB(255, 212, 181), _T("Orange, Accent 6, Lighter 60%") },
		{ RGB(190, 190, 190), _T("White, Background 1, Darker 25%") },
		{ RGB(65, 65, 65), _T("Black, Text 1, Lighter 25%") },
		{ RGB(118, 117, 112), _T("Tan, Background 2, Darker 35%") },
		{ RGB(115, 143, 175), _T("Dark Blue, Text 2, Lighter 40%") },
		{ RGB(143, 177, 213), _T("Blue, Accent 1, Lighter 40%") },
		{ RGB(222, 149, 147), _T("Red, Accent 2, Lighter 40%") },
		{ RGB(192, 213, 155), _T("Olive Green, Accent 3, Lighter 40%")},
		{ RGB(177, 160, 197), _T("Purple, Accent 4, Lighter 40%") },
		{ RGB(137, 203, 218), _T("Aqua, Accent 5, Lighter 40%") },
		{ RGB(255, 191, 145), _T("Orange, Accent 6, Lighter 40%") },
		{ RGB(163, 163, 163), _T("White, Background 1, Darker 35%") },
		{ RGB(42, 42, 42), _T("Black, Text 1, Lighter 15%") },
		{ RGB(61, 61, 59), _T("Tan, Background 2, Darker 50%") },
		{ RGB(20, 57, 92), _T("Dark Blue, Text 2, Darker 25%") },
		{ RGB(54, 96, 139), _T("Blue, Accent 1, Darker 25%") },
		{ RGB(149, 63, 60), _T("Red, Accent 2, Darker 25%") },
		{ RGB(114, 139, 71), _T("Olive Green, Accent 3, Darker 25%") },
		{ RGB(97, 76, 119), _T("Purple, Accent 4, Darker 25%") },
		{ RGB(41, 128, 146), _T("Aqua, Accent 5, Darker 25%") },
		{ RGB(190, 112, 59), _T("Orange, Accent 6, Darker 25%") },
		{ RGB(126, 126, 126), _T("White, Background 1, Darker 50%") },
		{ RGB(20, 20, 20), _T("Black, Text 1, Lighter 5%") },
		{ RGB(29, 29, 28), _T("Tan, Background 2, Darker 90%") },
		{ RGB(17, 40, 64), _T("Dark Blue, Text 2, Darker 50%") },
		{ RGB(38, 66, 94), _T("Blue, Accent 1, Darker 50%") },
		{ RGB(100, 44, 43), _T("Red, Accent 2, Darker 50%") },
		{ RGB(77, 93, 49), _T("Olive Green, Accent 3, Darker 50%") },
		{ RGB(67, 53, 81), _T("Purple, Accent 4, Darker 50%") },
		{ RGB(31, 86, 99), _T("Aqua, Accent 5, Darker 50%") },
		{ RGB(126, 77, 42), _T("Orange, Accent 6, Darker 50%") },
	};

	static ColorTableEntry colorsStandard [] =
	{
		{ RGB(200, 15, 18), _T("Dark Red") },
		{ RGB(255, 20, 24), _T("Red") },
		{ RGB(255, 191, 40), _T("Orange") },
		{ RGB(255, 255, 49), _T("Yellow") },
		{ RGB(138, 207, 87), _T("Light Green") },
		{ RGB(0, 175, 84), _T("Green") },
		{ RGB(0, 174, 238), _T("Light Blue") },
		{ RGB(0, 111, 189), _T("Blue") },
		{ RGB(0, 36, 95), _T("Black") },
		{ RGB(114, 50, 157), _T("Purple") },
	};

	m_lstMainColors.RemoveAll();
	nNumColours = sizeof(colorsMain) / sizeof(ColorTableEntry);

	for (i = 0; i < nNumColours; i++)
	{
		m_lstMainColors.AddTail(colorsMain [i].color);
		CMFCRibbonColorButton::SetColorName(colorsMain [i].color, colorsMain [i].szName);
	};

	m_lstAdditionalColors.RemoveAll();
	nNumColours = sizeof(colorsAdditional) / sizeof(ColorTableEntry);

	for (i = 0; i < nNumColours; i++)
	{
		m_lstAdditionalColors.AddTail(colorsAdditional [i].color);
		CMFCRibbonColorButton::SetColorName(colorsAdditional [i].color, colorsAdditional [i].szName);
	};

	m_lstStandardColors.RemoveAll();
	nNumColours = sizeof(colorsStandard) / sizeof(ColorTableEntry);

	for (i = 0; i < nNumColours; i++)
	{
		m_lstStandardColors.AddTail(colorsStandard [i].color);
		CMFCRibbonColorButton::SetColorName(colorsStandard [i].color, colorsStandard [i].szName);
	};


	//Init the DropDown windows
	_initColorDropDown(ID_COL_STREAMLINE);
	_initColorDropDown(ID_RIBBON_VORTEX_COLOR);
	_initColorDropDown(ID_RIBBON_RECT_COLOR);
	_initColorDropDown(ID_RIBBON_ELLIPSE_COLOR);
	_initColorDropDown(ID_RIBBON_TRIANGLE_COLOR);
	_initColorDropDown(ID_RIBBON_STREAMLINE_COLOR);
	_initColorDropDown(ID_RIBBON_STREAKLINE_COLOR);
	_initColorDropDown(ID_RIBBON_PATHLINE_COLOR);
	_initColorDropDown(ID_RIBBON_STREAMLINE_HALOCOLOR);
	_initColorDropDown(ID_RIBBON_PATHLINE_HALOCOLOR);
	_initColorDropDown(ID_RIBBON_VORTEX_HALOCOLOR);
	_initColorDropDown(ID_RIBBON_STREAMLINE_ARROW_COLOR);
	_initColorDropDown(ID_RIBBON_PATHLINE_ARROW_COLOR);
	_initColorDropDown(ID_RIBBON_VORTEX_ARROW_COLOR);
	_initColorDropDown(ID_RIBBON_STREAKLINE_HALOCOLOR);
	_initColorDropDown(ID_RIBBON_STREAKLINE_ARROWCOLOR);
	_initColorDropDown(ID_RIBBON_TIMELINE_COLOR);
	_initColorDropDown(ID_RIBBON_TIMELINE_HALOCOLOR);
	_initColorDropDown(ID_RIBBON_SPEEDLINE_COLOR);
	_initColorDropDown(ID_RIBBON_SPEEDLINE_HALO);
}

void CMainFrame::_initColorDropDown(UINT nID)
{
	CMFCRibbonColorButton* pBtn = reinterpret_cast<CMFCRibbonColorButton*>(m_wndRibbonBar.FindByID(nID, FALSE));

	if (pBtn)
	{
		pBtn->AddColorsGroup(_T("Main colors"),m_lstMainColors);
		pBtn->AddColorsGroup(_T(""),m_lstAdditionalColors);
		pBtn->AddColorsGroup(_T("Standard colors"),m_lstStandardColors);
		pBtn->SetColumns(10);
		pBtn->SetColorBoxSize(CSize(24, 24));

		pBtn->EnableOtherButton(_T("More"));
		pBtn->SetColor(RGB(0,0,0));
		pBtn->UpdateColor(RGB(0,0,0));
	}
}

void CMainFrame::OnColStreamline()
{
	CMFCRibbonColorButton* pBtn = (CMFCRibbonColorButton*)m_wndRibbonBar.FindByID(ID_COL_STREAMLINE, FALSE);
	if(pBtn)
	{
		CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
		pView->SetStreamlineColor(pBtn->GetColor());
	}
}

void CMainFrame::InitRibbon(void)
{

	CMFCRibbonComboBox *pWnd = reinterpret_cast<CMFCRibbonComboBox*>(m_wndRibbonBar.FindByID(ID_RIBBON_VORTEX_STYLE, FALSE));

	if (pWnd)
	{
		pWnd->AddItem(_T("Elliptic"), 0);
		pWnd->AddItem(_T("Clockwise spiral"), 1);
		pWnd->AddItem(_T("Counter clockwise spiral"), 2);
	}

	pWnd = reinterpret_cast<CMFCRibbonComboBox*>(m_wndRibbonBar.FindByID(ID_RIBBON_VORTEX_ARROW_DIR, FALSE));
	if (pWnd)
	{
		pWnd->AddItem(_T("Cloclwise"), 0);
		pWnd->AddItem(_T("Counter Cloclwise"), 1);
	}

	pWnd = reinterpret_cast<CMFCRibbonComboBox*>(m_wndRibbonBar.FindByID(ID_RIBBON_VORTEX_APPEARANCE, FALSE));
	if (pWnd)
	{
		pWnd->AddItem(_T("Normal"), 0);
		pWnd->AddItem(_T("Double spiral"), 1);
	}
	 
	CMFCRibbonGallery *pGal = reinterpret_cast<CMFCRibbonGallery*>(m_wndRibbonBar.FindByID(ID_FLOWTOOL_SELECT, FALSE));
	if (pGal)
	{
		pGal->SelectItem(1);

		CStringArray toolTips;

		toolTips.Add(_T("Select"));
		toolTips.Add(_T("Add vortex"));
		toolTips.Add(_T("Add stream line"));
		toolTips.Add(_T("Add streak line"));
		toolTips.Add(_T("Add path line"));
		toolTips.Add(_T("Add time line"));
		toolTips.Add(_T("Add rectangle"));
		toolTips.Add(_T("Add triangle"));
		toolTips.Add(_T("Add ellipse"));

		for (int i=0; i < toolTips.GetSize(); i++) {
			pGal->SetItemToolTip(i, toolTips.GetAt(i));
		}
	}
	
	//Init colors for ribbon color dropdown
	InitColors();
}

void CMainFrame::InitDisplayFunctionsDropdown(const CStringArray & arrNames)
{
	InitRibbonDropdown(ID_DISPLAY_MODE, arrNames);
}

void CMainFrame::InitDetectorFunctionsDropdown(const CStringArray & arrNames)
{
	InitRibbonDropdown(ID_VORTEX_DETECTOR, arrNames);
}

void CMainFrame::InitSpeedLineDropdowns()
{
	CStringArray arrNamesT, arrNamesA;
	int dataT[] = {SL_THICKNESS_CONST , SL_THICKNESS_INCREASE, SL_THICKNESS_DECREASE};
	int dataA[] = {SL_TRANSPARENCY_CONST , SL_TRANSPARENCY_INCREASE, SL_TRANSPARENCY_DECREASE};

	arrNamesT.Add(_T("Constant thickness"));
	arrNamesT.Add(_T("Increasing thickness"));
	arrNamesT.Add(_T("Decreasing thickness"));

	arrNamesA.Add(_T("Constant alpha"));
	arrNamesA.Add(_T("Increasing alpha"));
	arrNamesA.Add(_T("Decreasing alpha"));

	InitRibbonDropdown(ID_RIBBON_SPEEDLINE_THICKNESSMODE, arrNamesT, dataT);
	InitRibbonDropdown(ID_RIBBON_SPEEDLINE_TRANSPARENCYMODE, arrNamesA, dataA);
}

void CMainFrame::SetCurrDisplayFunction(UINT nIndex)
{
	SetRibbonDropdownCurSel(ID_DISPLAY_MODE, nIndex);
}

void CMainFrame::SetCurrDetectorFunction(UINT nIndex)
{
	SetRibbonDropdownCurSel(ID_VORTEX_DETECTOR, nIndex);
}

void CMainFrame::InitRibbonDropdown(UINT nID, const CStringArray &arrValues)
{
	CMFCRibbonComboBox *pWnd = reinterpret_cast<CMFCRibbonComboBox*>(m_wndRibbonBar.FindByID(nID, FALSE));

	if (pWnd)
	{
		for (int i=0; i < arrValues.GetSize(); i++)
		{
			pWnd->AddItem(arrValues[i], i);
		}
	}
}

void CMainFrame::InitRibbonDropdown(UINT nID, const CStringArray &arrValues, int dwData[])
{
	CMFCRibbonComboBox *pWnd = reinterpret_cast<CMFCRibbonComboBox*>(m_wndRibbonBar.FindByID(nID, FALSE));

	if (pWnd)
	{
		for (int i=0; i < arrValues.GetSize(); i++)
		{
			pWnd->AddItem(arrValues[i], dwData[i]);
		}
	}
}

void CMainFrame::SetRibbonDropdownCurSel(UINT nID, UINT nIndex)
{
	CMFCRibbonComboBox *pWnd = reinterpret_cast<CMFCRibbonComboBox*>(m_wndRibbonBar.FindByID(nID, FALSE));

	if (pWnd)
	{
		pWnd->SelectItem(static_cast<int>(nIndex));
	}
}

void CMainFrame::OnDisplayMode()
{
	CMFCRibbonComboBox *pWnd = reinterpret_cast<CMFCRibbonComboBox*>(m_wndRibbonBar.FindByID(ID_DISPLAY_MODE, FALSE));

	if (pWnd)
	{
		CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
		if(pView)
		{
			pView->SetDisplayFunction( static_cast<int>(pWnd->GetItemData(pWnd->GetCurSel())) );
		}
	}
}

void CMainFrame::OnFlowToolSelect()
{
	CMFCRibbonGallery *pWnd = reinterpret_cast<CMFCRibbonGallery*>(m_wndRibbonBar.FindByID(ID_FLOWTOOL_SELECT, FALSE));
	if ( pWnd )
	{
		CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
		if(pView)
		{
			pView->SetEditMode( FlowToolEditMode[ pWnd->GetSelectedItem() ]  );
		}
	}
}

void CMainFrame::OnVortexDetector()
{
	CMFCRibbonComboBox *pWnd = reinterpret_cast<CMFCRibbonComboBox*>(m_wndRibbonBar.FindByID(ID_VORTEX_DETECTOR, FALSE));

	if (pWnd)
	{
		CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
		if(pView)
		{
			pView->SetDetectorFunction( static_cast<int>(pWnd->GetItemData(pWnd->GetCurSel())) );
		}
	}
}

DWORD CMainFrame::getCategoryID(int nCatID)
{
	switch(nCatID)
	{
		case DO_VORTEX:
			return ID_CONTEXT_VORTEX;
		case DO_RECTANGLE:
			return ID_CONTEXT_RECTANGLE;
		case DO_ELLIPSE:
			return ID_CONTEXT_ELLIPSE;
		case DO_TRIANGLE:
			return ID_CONTEXT_TRIANGLE;
		case DO_PATHLINE:
			return ID_CONTEXT_PATHLINE;
		case DO_STREAMLINE:
			return ID_CONTEXT_STREAMLINE;
		case DO_STREAKLINE:
			return ID_CONTEXT_STREAKLINE;
		case DO_TIMELINE:
			return ID_CONTEXT_TIMELINE;
		default:
			return 0;
	}
}

void (CMainFrame::*CMainFrame::_getRibbonUpdateFunction(DWORD dwCatID))(CDrawingObjectParams *pParams)
{
	void (CMainFrame::*pActivateFunction)(CDrawingObjectParams *pParams) = nullptr;

	switch(dwCatID)
	{
		case ID_CONTEXT_VORTEX:
			pActivateFunction = &CMainFrame::RibbonVortexPropertyInit;
			break;
		case ID_CONTEXT_RECTANGLE:
			pActivateFunction = &CMainFrame::RibbonRectPropertyInit;
			break;
		case ID_CONTEXT_ELLIPSE:
			pActivateFunction = &CMainFrame::RibbonEllipsePropertyInit;
			break;
		case ID_CONTEXT_TRIANGLE:
			pActivateFunction = &CMainFrame::RibbonTrianglePropertyInit;
			break;
		case ID_CONTEXT_STREAMLINE:
			pActivateFunction = &CMainFrame::RibbonStreamlinePropertyInit;
			break;
		case ID_CONTEXT_PATHLINE:
			pActivateFunction = &CMainFrame::RibbonPathlinePropertyInit;
			break;
		case ID_CONTEXT_STREAKLINE:
			pActivateFunction = &CMainFrame::RibbonStreaklinePropertyInit;
			break;
		case ID_CONTEXT_TIMELINE:
			pActivateFunction = &CMainFrame::RibbonTimelinePropertyInit;
			break;
	}

	return pActivateFunction;
}

void CMainFrame::ActivateContextCategory(int catID, CDrawingObjectParams *pParams, BOOL bAddToSelection)
{
	if (catID > -1)
	{
		DWORD dwCatID = getCategoryID(catID);

		void (CMainFrame::*pActivateFunction)(CDrawingObjectParams *pParams) = _getRibbonUpdateFunction(dwCatID);

		if (pActivateFunction != m_pActivateFunc)
		{
			m_pActivateFunc = pActivateFunction;

			if (!bAddToSelection)
				m_wndRibbonBar.HideAllContextCategories();

			CMFCRibbonCategory *pCat = reinterpret_cast<CMFCRibbonCategory*>(m_wndRibbonBar.FindByID(dwCatID, FALSE));
			if (!pCat || ! pCat->IsActive() ) {
				m_wndRibbonBar.ShowContextCategories(dwCatID);
				m_wndRibbonBar.ActivateContextCategory(dwCatID);
			} 
		}

		if (m_pActivateFunc)
			(this->*m_pActivateFunc)(pParams);
	}
	else
	{
		m_wndRibbonBar.HideAllContextCategories();
		m_pActivateFunc = nullptr;
	}	
}

void CMainFrame::UpdateRibbon(CDrawingObjectParams *pParams)
{
	DWORD dwCatID = getCategoryID(pParams->GetValueInt(DOP_TYPE));

	void (CMainFrame::*pActivateFunction)(CDrawingObjectParams *pParams) = _getRibbonUpdateFunction(dwCatID);
	
	if (pActivateFunction)
		(this->*pActivateFunction)(pParams);
}

void CMainFrame::OnRibbonVortexColor()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_VORTEX_COLOR, &color)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_COLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexHalocolor()
{
	floatColor color;

	CDrawingObjectParams params(DO_VORTEX);

	if (_getRibbonColorValue(ID_RIBBON_VORTEX_HALOCOLOR, &color)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_HALOCOLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexArrowColor()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_VORTEX_ARROW_COLOR, &color)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_ARROWCOLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexThickness()
{
	float fThickness;

	if (_getRibbonEditValue(ID_RIBBON_VORTEX_THICKNESS, fThickness)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_THICKNESS, fThickness);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexCenter()
{
	CPointf center;

	if (_getRibbonEditCoordinate2D(ID_RIBBON_VORTEX_CENTER, center.x, center.y)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_CENTER, center);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexRadius1()
{
	float fRadius;

	if (_getRibbonEditValue(ID_RIBBON_VORTEX_RADIUS1, fRadius)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_RADIUS1, fRadius);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexRadius2()
{
	float fRadius;

	if (_getRibbonEditValue(ID_RIBBON_VORTEX_RADIUS2, fRadius)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_RADIUS2, fRadius);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexRotation()
{
	float fRotation;

	if (_getRibbonEditValue(ID_RIBBON_VORTEX_ROTATION, fRotation)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_ROTATION, fRotation);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexRevolutions()
{
	float fRevolutions;

	if (_getRibbonEditValue(ID_RIBBON_VORTEX_REVOLUTIONS, fRevolutions)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_REVOLUTIONS, fRevolutions);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexArrowSize()
{
	float fArrowSize;

	if (_getRibbonEditValue(ID_RIBBON_VORTEX_ARROW_SIZE, fArrowSize)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_ARROWSIZE, fArrowSize);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexAutoSize()
{
	m_bVortexAutoSize = !m_bVortexAutoSize;

	CDrawingObjectParams params(DO_VORTEX);
	params.SetValue(DOP_AUTOADJUSTSIZE, (m_bVortexAutoSize)? true : false);

	_objectPropertyChanged(&params);
}

void CMainFrame::OnRibbonVortexThreshold()
{
	CString str;
	if (_getRibbonEditValue(ID_RIBBON_VORTEX_THRESHOLD, str)) {
		CDrawingObjectParams params(DO_VORTEX);
		AfxExtractSubString(str, str, 0, _T('%'));
		params.SetValue(DOP_THRESHOLD, _tstof(str)/100.0f);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexNumArrows()
{
	int nNumArrows;

	if (_getRibbonEditValue(ID_RIBBON_VORTEX_NUM_ARROWS, nNumArrows)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_NUMARROWS, nNumArrows);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexArrowDir()
{
	DWORD dwDir;

	if (_getRibbonDropdownCurSel(ID_RIBBON_VORTEX_ARROW_DIR, dwDir)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_VORTEXDIR, static_cast<int>(dwDir));
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexTrajectorySteps()
{
	int nSteps;

	if (_getRibbonEditValue(ID_RIBBON_VORTEX_TRAJECTORY_TIMESTEPS, nSteps)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_TRAJECTORYSTEPS, nSteps);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexSmoothTrajectory()
{
	int nSmoothness;

	if (_getRibbonEditValue(ID_RIBBON_VORTEX_TRAJECTORY_SMOOTH, nSmoothness)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_SMOOTHNESS, nSmoothness);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexTrajectoryShow()
{
	m_bShowTrajectory = !m_bShowTrajectory;
	CDrawingObjectParams params(DO_VORTEX);
	params.SetValue(DOP_SHOWTRAJECTORY, (m_bShowTrajectory)? true : false);

	_objectPropertyChanged(&params);
}

void CMainFrame::OnRibbonVortexStyle()
{
	DWORD dwStyle;

	if (_getRibbonDropdownCurSel(ID_RIBBON_VORTEX_STYLE, dwStyle)) {	
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_VORTEXSTYLE, static_cast<int>(dwStyle));
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonVortexAppearance()
{
	DWORD dwAppearance;

	if (_getRibbonDropdownCurSel(ID_RIBBON_VORTEX_APPEARANCE, dwAppearance)) {
		CDrawingObjectParams params(DO_VORTEX);
		params.SetValue(DOP_APPEARANCE, static_cast<int>(dwAppearance));
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonRectangleColor()
{
	floatColor color;

	if (_getRibbonColorValue( ID_RIBBON_RECT_COLOR, &color) )
	{
		CDrawingObjectParams params(DO_RECTANGLE);
		params.SetValue( DOP_COLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonRectangleThickness()
{
	float fThickness;

	if (_getRibbonEditValue( ID_RIBBON_RECT_THICKNESS, fThickness) )
	{
		CDrawingObjectParams params(DO_RECTANGLE);
		params.SetValue( DOP_THICKNESS, fThickness);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonRectSolid()
{
	m_bRectSolid = !m_bRectSolid;

	CDrawingObjectParams params(DO_RECTANGLE);
	params.SetValue( DOP_ISSOLID, m_bRectSolid?true:false);
	_objectPropertyChanged(&params);
}

void CMainFrame::OnRibbonRectHatched()
{
	m_bRectHatched = !m_bRectHatched;

	CDrawingObjectParams params(DO_RECTANGLE);

	params.SetValue( DOP_HATCHED, m_bRectHatched?true:false);
	_objectPropertyChanged(&params);
}

void CMainFrame::OnUpdateRibbonRectHatched(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bRectHatched);
}

void CMainFrame::OnRibbonRectangleWidth()
{
	float fWidth;

	if (_getRibbonEditValue( ID_RIBBON_RECT_WIDTH, fWidth) ) {
		CDrawingObjectParams params(DO_RECTANGLE);
		params.SetValue( DOP_WIDTH, fWidth);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonRectangleHeight()
{
	float fHeight;

	if (_getRibbonEditValue( ID_RIBBON_RECT_HEIGHT, fHeight) ) {
		CDrawingObjectParams params(DO_RECTANGLE);
		params.SetValue( DOP_HEIGHT, fHeight);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonRectangleCenter()
{
	CPointf ptCenter;

	if (_getRibbonEditCoordinate2D(	ID_RIBBON_RECT_CENTER, ptCenter.x, ptCenter.y) ) {
		CDrawingObjectParams params(DO_RECTANGLE);
		params.SetValue( DOP_CENTER, ptCenter);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonRectangleRotation()
{
	float fRotation;

	if (_getRibbonEditValue(ID_RIBBON_RECT_ROTATION, fRotation)) {
		CDrawingObjectParams params(DO_RECTANGLE);
		params.SetValue( DOP_ROTATION, fRotation);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonTriangleColor()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_TRIANGLE_COLOR, &color)) {	
		CDrawingObjectParams params(DO_TRIANGLE);
		params.SetValue(DOP_COLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonTriangleThickness()
{
	float fThickness;

	if (_getRibbonEditValue(ID_RIBBON_TRIANGLE_THICKNESS, fThickness)) {
		CDrawingObjectParams params(DO_TRIANGLE);
		params.SetValue(DOP_THICKNESS, fThickness);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonTriangleSolid()
{
	m_bTriangleSolid = !m_bTriangleSolid;

	CDrawingObjectParams params(DO_TRIANGLE);
	params.SetValue(DOP_ISSOLID, m_bTriangleSolid?true:false);

	_objectPropertyChanged(&params);
}

void CMainFrame::OnRibbonTriangleHatched()
{
	m_bTriangleHatched= !m_bTriangleHatched;

	CDrawingObjectParams params(DO_TRIANGLE);
	params.SetValue(DOP_HATCHED, m_bTriangleHatched?true:false);

	_objectPropertyChanged(&params);
}


void CMainFrame::OnRibbonTrianglePoint1()
{
	float fWidth;

	if (_getRibbonEditValue( ID_RIBBON_TRIANGLE_POINT1, fWidth)) {
		CDrawingObjectParams params(DO_TRIANGLE);
		params.SetValue(DOP_WIDTH, fWidth);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonTrianglePoint2()
{
	float fHeight;

	if (_getRibbonEditValue( ID_RIBBON_TRIANGLE_POINT2, fHeight)) {
		CDrawingObjectParams params(DO_TRIANGLE);
		params.SetValue(DOP_HEIGHT,fHeight);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonTrianglePoint3()
{
	CPointf center;

	if (_getRibbonEditCoordinate2D(	ID_RIBBON_TRIANGLE_POINT3, center.x, center.y)) {
		CDrawingObjectParams params(DO_TRIANGLE);
		params.SetValue(DOP_CENTER, center);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonTriangleRotation()
{
	float fRotation;

	if (_getRibbonEditValue(ID_RIBBON_TRIANGLE_ROTATION, fRotation)){
		CDrawingObjectParams params(DO_TRIANGLE);
		params.SetValue(DOP_ROTATION, fRotation);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonEllipseColor()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_ELLIPSE_COLOR, &color)) {
		CDrawingObjectParams params(DO_ELLIPSE);
		params.SetValue(DOP_COLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonEllipseThickness()
{
	float fThickness;

	if (_getRibbonEditValue(ID_RIBBON_ELLIPSE_THICKNESS, fThickness)) {
		CDrawingObjectParams params(DO_ELLIPSE);
		params.SetValue(DOP_THICKNESS, fThickness);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonEllipseSolid()
{
	m_bEllipseSolid = !m_bEllipseSolid;
	CDrawingObjectParams params(DO_ELLIPSE);
	params.SetValue(DOP_ISSOLID, m_bEllipseSolid? true : false);
	_objectPropertyChanged(&params);
}

void CMainFrame::OnRibbonEllipseHatched()
{
	m_bEllipseHatched = !m_bEllipseHatched;
	CDrawingObjectParams params(DO_ELLIPSE);
	params.SetValue(DOP_HATCHED, m_bEllipseHatched? true : false);
	_objectPropertyChanged(&params);
}

void CMainFrame::OnRibbonEllipseCenter()
{
	CPointf center;

	if (_getRibbonEditCoordinate2D(	ID_RIBBON_ELLIPSE_CENTER, center.x, center.y)) {
		CDrawingObjectParams params(DO_ELLIPSE);
		params.SetValue(DOP_CENTER, center);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonEllipseRadius1()
{
	float fRadius;

	if (_getRibbonEditValue(ID_RIBBON_ELLIPSE_RADIUS1, fRadius)) {
		CDrawingObjectParams params(DO_ELLIPSE);
		params.SetValue(DOP_RADIUS1, fRadius);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonEllipseRadius2()
{
	float fRadius;

	if (_getRibbonEditValue(ID_RIBBON_ELLIPSE_RADIUS2, fRadius)) {
		CDrawingObjectParams params(DO_ELLIPSE);
		params.SetValue(DOP_RADIUS2, fRadius);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonEllipseRotation()
{
	float fRotation;

	if (_getRibbonEditValue(ID_RIBBON_ELLIPSE_ROTATION, fRotation)) {
		CDrawingObjectParams params(DO_ELLIPSE);
		params.SetValue(DOP_ROTATION, fRotation);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreamLineColor()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_STREAMLINE_COLOR, &color)) {
		CDrawingObjectParams params(DO_STREAMLINE);
		params.SetValue(DOP_COLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreamlineHalocolor()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_STREAMLINE_HALOCOLOR, &color)) {
		CDrawingObjectParams params(DO_STREAMLINE);
		params.SetValue(DOP_HALOCOLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreamlineArrowColor()
{
		floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_STREAMLINE_ARROW_COLOR, &color)) {
		CDrawingObjectParams params(DO_STREAMLINE);
		params.SetValue(DOP_ARROWCOLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreamLineThickness()
{
	float fThickness;

	if (_getRibbonEditValue(ID_RIBBON_STREAMLINE_THICKNESS, fThickness)) {
		CDrawingObjectParams params(DO_STREAMLINE);
		params.SetValue(DOP_THICKNESS, fThickness);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreamLineNumArrows()
{
	int numArrows;

	if (_getRibbonEditValue(ID_RIBBON_STREAMLINE_NUM_ARROWS, numArrows)) {
		CDrawingObjectParams params(DO_STREAMLINE);
		params.SetValue(DOP_NUMARROWS, numArrows);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreamlineArrows()
{
	m_bArrowsStreamline = !m_bArrowsStreamline;

	CDrawingObjectParams params(DO_STREAMLINE);
	params.SetValue(DOP_SHOWARROWS, (m_bArrowsStreamline)? true : false);
	_objectPropertyChanged(&params);
}

void CMainFrame::OnRibbonStreamLineOrigin()
{
	CPointf origin;

	if (_getRibbonEditCoordinate2D(	ID_RIBBON_STREAMLINE_START, origin.x, origin.y)) {
		CDrawingObjectParams params(DO_STREAMLINE);
		params.SetValue(DOP_ORIGIN, origin);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreamLineNumSteps()
{
	int numSteps;

	if (_getRibbonEditValue(ID_RIBBON_STREAMLINE_NUM_STEPS, numSteps)) {
		CDrawingObjectParams params(DO_STREAMLINE);
		params.SetValue(DOP_INTEGRATIONSTEPS, numSteps);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreamLineStepLen()
{
	float fStepLen;

	if (_getRibbonEditValue(ID_RIBBON_STREAMLINE_STEP_LEN, fStepLen)) {
		CDrawingObjectParams params(DO_STREAMLINE);
		params.SetValue(DOP_STEPLENGTH, fStepLen);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreamLineArrowSize()
{
	float fArrowSize;

	if (_getRibbonEditValue(ID_RIBBON_STREAMLINE_ARROW_SIZE, fArrowSize)) {
		CDrawingObjectParams params(DO_STREAMLINE);
		params.SetValue(DOP_ARROWSIZE, fArrowSize);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreamLineSmoothness()
{
	int nSmoothness;

	if (_getRibbonEditValue(ID_RIBBON_STREAMLINE_SMOOTHNESS, nSmoothness)) {
		CDrawingObjectParams params(DO_STREAMLINE);
		params.SetValue(DOP_SMOOTHNESS, nSmoothness);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreamlineNumstreamlets()
{
	int nStreamlets;

	if (_getRibbonEditValue(ID_RIBBON_STREAMLINE_NUMSTREAMLETS, nStreamlets)) {
		CDrawingObjectParams params(DO_STREAMLINE);
		params.SetValue(DOP_NUMDROPLETS, nStreamlets);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonStreamlineIsstreamlet()
{
	m_bIsStreamlet = !m_bIsStreamlet;

	CDrawingObjectParams params(DO_STREAMLINE);
	params.SetValue(DOP_DRAWASDROPLETS, (m_bIsStreamlet)? true : false);
	_objectPropertyChanged(&params);
}

void CMainFrame::OnRibbonPathLineColor()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_PATHLINE_COLOR, &color)) {
		CDrawingObjectParams params(DO_PATHLINE);
		params.SetValue(DOP_COLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonPathlineHalocolor()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_PATHLINE_HALOCOLOR, &color)) {
		CDrawingObjectParams params(DO_PATHLINE);
		params.SetValue(DOP_HALOCOLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonPathlineArrowColor()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_PATHLINE_ARROW_COLOR, &color)) {
		CDrawingObjectParams params(DO_PATHLINE);
		params.SetValue(DOP_ARROWCOLOR, color);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonPathLineThickness()
{
	float fThickness;

	if (_getRibbonEditValue(ID_RIBBON_PATHLINE_THICKNESS, fThickness)) {
		CDrawingObjectParams params(DO_PATHLINE);
		params.SetValue(DOP_THICKNESS, fThickness);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonPathLineNumArrows()
{
	int numArrows;

	if (_getRibbonEditValue(ID_RIBBON_PATHLINE_NUM_ARROWS, numArrows)) {
		CDrawingObjectParams params(DO_PATHLINE);
		params.SetValue(DOP_NUMARROWS, numArrows);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonPathlineArrows()
{
	m_bArrowsPathline = !m_bArrowsPathline;

	CDrawingObjectParams params(DO_PATHLINE);
	params.SetValue(DOP_SHOWARROWS, (m_bArrowsPathline)? true : false);

	_objectPropertyChanged(&params);
}

void CMainFrame::OnRibbonPathLineOrigin()
{
	CPointf origin;

	if (_getRibbonEditCoordinate2D(	ID_RIBBON_PATHLINE_START, origin.x, origin.y)) {
		CDrawingObjectParams params(DO_PATHLINE);
		params.SetValue(DOP_ORIGIN, origin);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonPathLineNumSteps()
{
	int numSteps;

	if (_getRibbonEditValue(ID_RIBBON_PATHLINE_NUM_STEPS, numSteps)) {
		CDrawingObjectParams params(DO_PATHLINE);
		params.SetValue(DOP_INTEGRATIONSTEPS, numSteps);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonPathLineStepLen()
{
	float fStepLen;

	if (_getRibbonEditValue(ID_RIBBON_PATHLINE_STEP_LEN, fStepLen)) {
		CDrawingObjectParams params(DO_PATHLINE);
		params.SetValue(DOP_STEPLENGTH, fStepLen);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonPathLineArrowSize()
{
	float fArrowSize;

	if (_getRibbonEditValue(ID_RIBBON_PATHLINE_ARROW_SIZE, fArrowSize)) {
		CDrawingObjectParams params(DO_PATHLINE);
		params.SetValue(DOP_ARROWSIZE, fArrowSize);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonPathLineSmoothness()
{
	int nSmoothness;

	if (_getRibbonEditValue(ID_RIBBON_PATHLINE_SMOOTHNESS, nSmoothness)) {
		CDrawingObjectParams params(DO_PATHLINE);
		params.SetValue(DOP_SMOOTHNESS, nSmoothness);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonPathlineNumpathlets()
{
	int nPathlets;

	if (_getRibbonEditValue(ID_RIBBON_PATHLINE_NUMPATHLETS, nPathlets)) {
		CDrawingObjectParams params(DO_PATHLINE);
		params.SetValue(DOP_NUMDROPLETS, nPathlets);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonPathlineIspathlet()
{
	m_bIsPathlet = !m_bIsPathlet;

	CDrawingObjectParams params(DO_PATHLINE);
	params.SetValue(DOP_DRAWASDROPLETS, (m_bIsPathlet)? true : false);
	_objectPropertyChanged(&params);
}

void CMainFrame::OnRibbonPathlineStartframe()
{
	int nStartFrame;

	if (_getRibbonEditValue(ID_RIBBON_PATHLINE_STARTFRAME, nStartFrame)) {
		CDrawingObjectParams params(DO_PATHLINE);
		params.SetValue(DOP_STARTFRAME, nStartFrame);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonPathlineUseStartframe()
{
	m_bPathlineUseStartframe = !m_bPathlineUseStartframe;

	CDrawingObjectParams params(DO_PATHLINE);
	params.SetValue(DOP_USE_STARTFRAME, m_bPathlineUseStartframe? true:false);
	_objectPropertyChanged(&params);
}


void CMainFrame::OnUpdateRibbonPathlineUseStartframe(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bPathlineUseStartframe);
}

void CMainFrame::OnRibbonStreakLineColor()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_STREAKLINE_COLOR, &color)) {
		CDrawingObjectParams params(DO_STREAKLINE);
		params.SetValue(DOP_COLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreakLineThickness()
{
	float fThickness;

	if (_getRibbonEditValue(ID_RIBBON_STREAKLINE_THICKNESS, fThickness)) {
		CDrawingObjectParams params(DO_STREAKLINE);
		params.SetValue(DOP_THICKNESS, fThickness);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreakLineNumArrows()
{
	int numArrows;

	if (_getRibbonEditValue(ID_RIBBON_STREAKLINE_NUM_ARROWS, numArrows)) {
		CDrawingObjectParams params(DO_STREAKLINE);
		params.SetValue(DOP_NUMARROWS, numArrows);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreaklineArrows()
{
	m_bArrowsStreakline = !m_bArrowsStreakline;

	CDrawingObjectParams params(DO_STREAKLINE);
	params.SetValue(DOP_SHOWARROWS, (m_bArrowsStreakline)? true : false);
	_objectPropertyChanged(&params);
}

void CMainFrame::OnRibbonStreakLineOrigin()
{
	CPointf origin;

	if (_getRibbonEditCoordinate2D(	ID_RIBBON_STREAKLINE_ORIGIN, origin.x, origin.y)) {
		CDrawingObjectParams params(DO_STREAKLINE);
		params.SetValue(DOP_ORIGIN, origin);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreakLineNumSteps()
{
	int numSteps;

	if (_getRibbonEditValue(ID_RIBBON_STREAKLINE_NUM_STEPS, numSteps)) {
		CDrawingObjectParams params(DO_STREAKLINE);
		params.SetValue(DOP_INTEGRATIONSTEPS, numSteps);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreakLineStepLen()
{
	float fStepLength;

	if (_getRibbonEditValue(ID_RIBBON_STREAKLINE_STEP_LEN, fStepLength)) {
		CDrawingObjectParams params(DO_STREAKLINE);
		params.SetValue(DOP_STEPLENGTH, fStepLength);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreakLineArrowSize()
{
	float fArrowSize;

	if (_getRibbonEditValue(ID_RIBBON_STREAKLINE_ARROW_SIZE, fArrowSize)) {
		CDrawingObjectParams params(DO_STREAKLINE);
		params.SetValue(DOP_ARROWSIZE, fArrowSize);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreakLineSmoothness()
{
	int nSmoothness;

	if (_getRibbonEditValue(ID_RIBBON_STREAKLINE_SMOOTHNESS, nSmoothness)) {
		CDrawingObjectParams params(DO_STREAKLINE);
		params.SetValue(DOP_SMOOTHNESS, nSmoothness);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnStreaklineAsParticles()
{
	m_bStreakLineAsParticles = !m_bStreakLineAsParticles;

	CDrawingObjectParams params(DO_STREAKLINE);
	params.SetValue(DOP_RENDER_AS_PARTICLES, (m_bStreakLineAsParticles)? true:false);
	_objectPropertyChanged(&params);
}

void CMainFrame::OnRibbonStreaklineStartframe()
{
	int nStartFrame;

	if (_getRibbonEditValue(ID_RIBBON_STREAKLINE_STARTFRAME, nStartFrame)) {
		CDrawingObjectParams params(DO_STREAKLINE);
		params.SetValue(DOP_STARTFRAME, nStartFrame);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreaklineUseStartframe()
{
	m_bStreaklineUseStartframe = !m_bStreaklineUseStartframe;

	CDrawingObjectParams params(DO_STREAKLINE);
	params.SetValue(DOP_USE_STARTFRAME, m_bStreaklineUseStartframe? true:false);
	_objectPropertyChanged(&params);
}


void CMainFrame::OnUpdateRibbonStreaklineUseStartframe(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bStreaklineUseStartframe);
}

void CMainFrame::OnRibbonStreaklineHalocolor()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_STREAKLINE_HALOCOLOR, &color)) {
		CDrawingObjectParams params(DO_STREAKLINE);
		params.SetValue(DOP_HALOCOLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreaklineArrowcolor()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_STREAKLINE_ARROWCOLOR, &color)) {
		CDrawingObjectParams params(DO_STREAKLINE);
		params.SetValue(DOP_ARROWCOLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonStreaklineIsstreaklet()
{
	m_bIsStreaklet = !m_bIsStreaklet;

	CDrawingObjectParams params(DO_STREAKLINE);
	params.SetValue(DOP_DRAWASDROPLETS, m_bIsStreaklet? true:false);
	_objectPropertyChanged(&params);
}


void CMainFrame::OnUpdateRibbonStreaklineIsstreaklet(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bIsStreaklet);
}


void CMainFrame::OnRibbonStreaklineNumstreaklets()
{
	int numStreaklets;

	CDrawingObjectParams params(DO_STREAKLINE);
	if (_getRibbonEditValue(ID_RIBBON_STREAKLINE_NUMSTREAKLETS, numStreaklets) ) {
		params.SetValue(DOP_NUMDROPLETS, numStreaklets);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonTimelineColor()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_TIMELINE_COLOR, &color)) {
		CDrawingObjectParams params(DO_TIMELINE);
		params.SetValue(DOP_COLOR, color);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonTimelineHalo()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_TIMELINE_HALOCOLOR, &color)) {
		CDrawingObjectParams params(DO_TIMELINE);
		params.SetValue(DOP_HALOCOLOR, color);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonTimelineThickness()
{
	float fThickness;

	if (_getRibbonEditValue(ID_RIBBON_TIMELINE_THICKNESS, fThickness)) {
		CDrawingObjectParams params(DO_TIMELINE);
		params.SetValue(DOP_THICKNESS, fThickness);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonTimelineSmoothness()
{
	int nSmoothness;

	if (_getRibbonEditValue(ID_RIBBON_TIMELINE_SMOOTHNESS, nSmoothness)) {
		CDrawingObjectParams params(DO_TIMELINE);
		params.SetValue(DOP_SMOOTHNESS, nSmoothness);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonTimelineIntegrationsteps()
{
	int nSteps;

	if (_getRibbonEditValue(ID_RIBBON_TIMELINE_INTEGRATIONSTEPS, nSteps)) {
		CDrawingObjectParams params(DO_TIMELINE);
		params.SetValue(DOP_INTEGRATIONSTEPS, nSteps);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonTimelineSeedStart()
{
	CPointf point;

	if (_getRibbonEditCoordinate2D(	ID_RIBBON_TIMELINE_SEED_START, point.x, point.y)) {
		CDrawingObjectParams params(DO_TIMELINE);
		params.SetValue(DOP_ORIGIN, point);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonTimelineSeedEnd()
{
	CPointf point;

	if (_getRibbonEditCoordinate2D(	ID_RIBBON_TIMELINE_SEED_END, point.x, point.y)) {
		CDrawingObjectParams params(DO_TIMELINE);
		params.SetValue(DOP_PTEND, point);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonTimelineSeedlineSamples()
{
	int nSamples;

	if (_getRibbonEditValue(ID_RIBBON_TIMELINE_SEEDLINE_SAMPLES, nSamples)) {
		CDrawingObjectParams params(DO_TIMELINE);
		params.SetValue(DOP_NUM_SAMPLES, nSamples);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonTimelineStepSize()
{
	float fStepSize;

	if (_getRibbonEditValue(ID_RIBBON_TIMELINE_STEP_SIZE, fStepSize)) {
		CDrawingObjectParams params(DO_TIMELINE);
		params.SetValue(DOP_STEPLENGTH, fStepSize);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonTimelineStartFrame()
{
	int nStartFrame;

	if (_getRibbonEditValue(ID_RIBBON_TIMELINE_START_FRAME, nStartFrame)) {
		CDrawingObjectParams params(DO_TIMELINE);
		params.SetValue(DOP_STARTFRAME, nStartFrame);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonTimelineFixedStartFrame()
{
	m_bTimelineUseStartFrame = !m_bTimelineUseStartFrame;

	CDrawingObjectParams params(DO_TIMELINE);
	params.SetValue(DOP_USE_STARTFRAME, m_bTimelineUseStartFrame?true:false);
	_objectPropertyChanged(&params);
}

void CMainFrame::OnRibbonTimelineShowSeedingline()
{
	m_bTimelineShowSeedingline = !m_bTimelineShowSeedingline;
	CDrawingObjectParams params(DO_TIMELINE);
	params.SetValue(DOP_SHOW_SEEDINGLINE, m_bTimelineShowSeedingline?true:false);
	_objectPropertyChanged(&params);
}

void CMainFrame::OnUpdateRibbonTimelineShowSeedingline(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bTimelineShowSeedingline);
}


void CMainFrame::OnUpdateRibbonTimelineFixedStartFrame(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bTimelineUseStartFrame);
}

void CMainFrame::OnRibbonTimelineShowTrajectory()
{
	m_bTimeLineShowTrajectory = !m_bTimeLineShowTrajectory;

	CDrawingObjectParams params(DO_TIMELINE);
	params.SetValue(DOP_SHOWTRAJECTORY, m_bTimeLineShowTrajectory?true:false);
	_objectPropertyChanged(&params);
}

void CMainFrame::OnUpdateRibbonTimelineShowTrajectory(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bTimeLineShowTrajectory);
}

void CMainFrame::OnRibbonSpeedlineColor()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_SPEEDLINE_COLOR, &color)) {
		CDrawingObjectParams params(DO_SPEEDLINE);
		params.SetValue(DOP_COLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonSpeedlineHalo()
{
	floatColor color;

	if (_getRibbonColorValue(ID_RIBBON_SPEEDLINE_HALO, &color)) {
		CDrawingObjectParams params(DO_SPEEDLINE);
		params.SetValue(DOP_HALOCOLOR, color);
		_objectPropertyChanged(&params);
	}
}

void CMainFrame::OnRibbonSpeedlineTransparencymode()
{
	DWORD dwData1, dwData2;
	DWORD dwData (0);
	if (_getRibbonDropdownCurSel(ID_RIBBON_SPEEDLINE_THICKNESSMODE, dwData1) &&
		_getRibbonDropdownCurSel(ID_RIBBON_SPEEDLINE_TRANSPARENCYMODE, dwData2))
	{
		dwData = dwData1 | dwData2;
		CDrawingObjectParams params(DO_SPEEDLINE);
		params.SetValue(DOP_SPEEDLINE_STYLE, static_cast<int>(dwData));
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonSpeedlineAlphaMax()
{
	float fVal;
	if (_getRibbonEditValue(ID_RIBBON_SPEEDLINE_ALPHA_MAX, fVal))
	{
		CDrawingObjectParams params(DO_SPEEDLINE);
		params.SetValue(DOP_ALPHA, fVal);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonSpeedlineAlphaMin()
{
	float fVal;
	if (_getRibbonEditValue(ID_RIBBON_SPEEDLINE_ALPHA_MIN, fVal))
	{
		CDrawingObjectParams params(DO_SPEEDLINE);
		params.SetValue(DOP_ALPHA_MIN, fVal);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonSpeedlineThicknessmode()
{
	OnRibbonSpeedlineTransparencymode();
}


void CMainFrame::OnRibbonSpeedlineThicknessMax()
{
	float fVal;
	if (_getRibbonEditValue(ID_RIBBON_SPEEDLINE_THICKNESS_MAX, fVal))
	{
		CDrawingObjectParams params(DO_SPEEDLINE);
		params.SetValue(DOP_THICKNESS, fVal);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonSpeedlineThicknessMin()
{
	float fVal;
	if (_getRibbonEditValue(ID_RIBBON_SPEEDLINE_THICKNESS_MIN, fVal))
	{
		CDrawingObjectParams params(DO_SPEEDLINE);
		params.SetValue(DOP_THICKNESS_MIN, fVal);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonSpeedlineTimesteps()
{
	int iVal;
	if (_getRibbonEditValue(ID_RIBBON_SPEEDLINE_TIMESTEPS, iVal))
	{
		CDrawingObjectParams params(DO_SPEEDLINE);
		params.SetValue(DOP_TRAJECTORYSTEPS, iVal);
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonSpeedlineSmooth()
{
	int iVal;
	if (_getRibbonEditValue(ID_RIBBON_SPEEDLINE_SMOOTH, iVal))
	{
		CDrawingObjectParams params(DO_SPEEDLINE);
		params.SetValue(DOP_SMOOTHNESS, iVal);
		_objectPropertyChanged(&params);
	}
}


void  CMainFrame::_objectPropertyChanged(CDrawingObjectParams *pParams)
{
	CView *pView = GetActiveView();
	if(pView)
	{
		pView->SendMessage(WM_VIEW_UPDATE_SELECTED, WPARAM(pParams) );
	}
}

void CMainFrame::ActivateSpeedLineContextCategory(CDrawingObjectParams *pParams)
{
	if (pParams->GetValueInt(DOP_TYPE) != DO_SPEEDLINE) return;

	m_wndRibbonBar.ShowContextCategories(ID_CONTEXT_SPEEDLINE);
	m_wndRibbonBar.ActivateContextCategory(ID_CONTEXT_SPEEDLINE);

	_setRibbonColorValue(ID_RIBBON_SPEEDLINE_COLOR, pParams->GetValueFloatColor(DOP_COLOR) );
	_setRibbonColorValue(ID_RIBBON_SPEEDLINE_HALO, pParams->GetValueFloatColor(DOP_HALOCOLOR) );
	_setRibbonDropdownCurSel(ID_RIBBON_SPEEDLINE_TRANSPARENCYMODE, pParams->GetValueInt(DOP_SPEEDLINE_STYLE) & 0x0F );
	_setRibbonDropdownCurSel(ID_RIBBON_SPEEDLINE_THICKNESSMODE, pParams->GetValueInt(DOP_SPEEDLINE_STYLE) & 0xF0 );

	_setRibbonEditValue(ID_RIBBON_SPEEDLINE_ALPHA_MAX, pParams->GetValueFloat(DOP_ALPHA));
	_setRibbonEditValue(ID_RIBBON_SPEEDLINE_ALPHA_MIN, pParams->GetValueFloat(DOP_ALPHA_MIN));
	_setRibbonEditValue(ID_RIBBON_SPEEDLINE_THICKNESS_MAX, pParams->GetValueFloat(DOP_THICKNESS));
	_setRibbonEditValue(ID_RIBBON_SPEEDLINE_THICKNESS_MIN, pParams->GetValueFloat(DOP_THICKNESS_MIN));

	_setRibbonEditValue(ID_RIBBON_SPEEDLINE_TIMESTEPS, pParams->GetValueInt(DOP_TRAJECTORYSTEPS));
	_setRibbonEditValue(ID_RIBBON_SPEEDLINE_SMOOTH, pParams->GetValueInt(DOP_SMOOTHNESS));

	m_wndRibbonBar.RedrawWindow();
}

void CMainFrame::RibbonVortexPropertyInit(CDrawingObjectParams *pParams)
{
	_setRibbonDropdownCurSel(ID_RIBBON_VORTEX_STYLE, pParams->GetValueInt(DOP_VORTEXSTYLE));
	_setRibbonColorValue(ID_RIBBON_VORTEX_COLOR, pParams->GetValueFloatColor(DOP_COLOR) );
	_setRibbonColorValue(ID_RIBBON_VORTEX_HALOCOLOR, pParams->GetValueFloatColor(DOP_HALOCOLOR)); 
	_setRibbonColorValue(ID_RIBBON_VORTEX_ARROW_COLOR, pParams->GetValueFloatColor(DOP_ARROWCOLOR));
	_setRibbonEditCoordinate2D(ID_RIBBON_VORTEX_CENTER, pParams->GetValuePointf(DOP_CENTER));
	_setRibbonEditValue(ID_RIBBON_VORTEX_RADIUS1, pParams->GetValueFloat(DOP_RADIUS1));
	_setRibbonEditValue(ID_RIBBON_VORTEX_RADIUS2, pParams->GetValueFloat(DOP_RADIUS2));
	_setRibbonEditValue(ID_RIBBON_VORTEX_ROTATION, pParams->GetValueFloat(DOP_ROTATION));
	_setRibbonEditValue(ID_RIBBON_VORTEX_THICKNESS, pParams->GetValueFloat(DOP_THICKNESS));
	_setRibbonEditValue(ID_RIBBON_VORTEX_REVOLUTIONS, pParams->GetValueFloat(DOP_REVOLUTIONS));
	_setRibbonEditValue(ID_RIBBON_VORTEX_ARROW_SIZE, pParams->GetValueFloat(DOP_ARROWSIZE));
	_setRibbonEditValue(ID_RIBBON_VORTEX_NUM_ARROWS, pParams->GetValueInt(DOP_NUMARROWS));
	_setRibbonDropdownCurSel(ID_RIBBON_VORTEX_ARROW_DIR, pParams->GetValueInt(DOP_ARROW_DIR));
	_setRibbonEditValue(ID_RIBBON_VORTEX_TRAJECTORY_TIMESTEPS, pParams->GetValueInt(DOP_TRAJECTORYSTEPS));
	_setRibbonEditValue(ID_RIBBON_VORTEX_TRAJECTORY_SMOOTH, pParams->GetValueInt(DOP_SMOOTHNESS));
	_setRibbonDropdownCurSel(ID_RIBBON_VORTEX_APPEARANCE, pParams->GetValueInt(DOP_APPEARANCE));
	
	CString dummy;
	dummy.Format(_T("%d%%"),  static_cast<int>(pParams->GetValueFloat(DOP_THRESHOLD) *100.0f));
	_setRibbonEditValue(ID_RIBBON_VORTEX_THRESHOLD, dummy);

	m_bVortexAutoSize = (pParams->GetValueBool(DOP_AUTOADJUSTSIZE))? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_VORTEX_AUTO_ADJUST_SIZE, m_bVortexAutoSize);

	m_bShowTrajectory = (pParams->GetValueBool(DOP_SHOWTRAJECTORY))? TRUE : FALSE;
	_setRibbonCheckValue(ID_RIBBON_VORTEX_TRAJECTORY_SHOW, m_bShowTrajectory);

	if ( pParams->childParams && m_bShowTrajectory) {
		ActivateSpeedLineContextCategory(pParams->GetChildParams());
	}

	m_wndRibbonBar.RedrawWindow();
}

void CMainFrame::RibbonRectPropertyInit(CDrawingObjectParams *pParams)
{
	_setRibbonColorValue( ID_RIBBON_RECT_COLOR, pParams->GetValueFloatColor(DOP_COLOR) );
	_setRibbonEditCoordinate2D( ID_RIBBON_RECT_CENTER, pParams->GetValuePointf(DOP_CENTER) );
	_setRibbonEditValue(ID_RIBBON_RECT_WIDTH, pParams->GetValueFloat(DOP_WIDTH) );
	_setRibbonEditValue(ID_RIBBON_RECT_HEIGHT, pParams->GetValueFloat(DOP_HEIGHT) );
	_setRibbonEditValue(ID_RIBBON_RECT_ROTATION, pParams->GetValueFloat(DOP_ROTATION) );
	_setRibbonEditValue(ID_RIBBON_RECT_THICKNESS, pParams->GetValueFloat(DOP_THICKNESS) );

	m_bRectSolid = pParams->GetValueBool(DOP_ISSOLID) ? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_RECT_SOLID, m_bRectSolid);

	m_bRectHatched = pParams->GetValueBool(DOP_HATCHED) ? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_RECT_HATCHED, m_bRectHatched);
}

void CMainFrame::RibbonEllipsePropertyInit(CDrawingObjectParams *pParams)
{
	_setRibbonColorValue(ID_RIBBON_ELLIPSE_COLOR, pParams->GetValueFloatColor(DOP_COLOR));
	_setRibbonEditCoordinate2D(ID_RIBBON_ELLIPSE_CENTER, pParams->GetValuePointf(DOP_CENTER));
	_setRibbonEditValue(ID_RIBBON_ELLIPSE_RADIUS1, pParams->GetValueFloat(DOP_RADIUS1));
	_setRibbonEditValue(ID_RIBBON_ELLIPSE_RADIUS2, pParams->GetValueFloat(DOP_RADIUS2));
	_setRibbonEditValue(ID_RIBBON_ELLIPSE_ROTATION, pParams->GetValueFloat(DOP_ROTATION));
	_setRibbonEditValue(ID_RIBBON_ELLIPSE_THICKNESS, pParams->GetValueFloat(DOP_THICKNESS));

	m_bEllipseSolid = pParams->GetValueBool(DOP_ISSOLID) ? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_ELLIPSE_SOLID, m_bEllipseSolid);

	m_bEllipseHatched = pParams->GetValueBool(DOP_HATCHED) ? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_ELLIPSE_HATCHED, m_bEllipseHatched);

	m_wndRibbonBar.RedrawWindow();
}

void CMainFrame::RibbonTrianglePropertyInit(CDrawingObjectParams *pParams)
{
	_setRibbonColorValue(ID_RIBBON_TRIANGLE_COLOR, pParams->GetValueFloatColor(DOP_COLOR));
	_setRibbonEditValue(ID_RIBBON_TRIANGLE_ROTATION, pParams->GetValueFloat(DOP_ROTATION));
	_setRibbonEditValue(ID_RIBBON_TRIANGLE_THICKNESS, pParams->GetValueFloat(DOP_THICKNESS));
	_setRibbonEditValue(ID_RIBBON_TRIANGLE_POINT1, pParams->GetValueFloat(DOP_WIDTH));
	_setRibbonEditValue(ID_RIBBON_TRIANGLE_POINT2, pParams->GetValueFloat(DOP_HEIGHT));
	_setRibbonEditCoordinate2D(ID_RIBBON_TRIANGLE_POINT3, pParams->GetValuePointf(DOP_CENTER));

	m_bTriangleSolid = pParams->GetValueBool(DOP_ISSOLID)? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_TRIANGLE_SOLID, m_bTriangleSolid);

	m_bTriangleHatched = pParams->GetValueBool(DOP_HATCHED) ? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_TRIANGLE_HATCHED, m_bTriangleHatched);	

	m_wndRibbonBar.RedrawWindow();
}

void CMainFrame::RibbonStreamlinePropertyInit(CDrawingObjectParams *pParams)
{
	_setRibbonColorValue(ID_RIBBON_STREAMLINE_COLOR, pParams->GetValueFloatColor(DOP_COLOR));
	_setRibbonColorValue(ID_RIBBON_STREAMLINE_HALOCOLOR, pParams->GetValueFloatColor(DOP_HALOCOLOR));
	_setRibbonColorValue(ID_RIBBON_STREAMLINE_ARROW_COLOR, pParams->GetValueFloatColor(DOP_ARROWCOLOR));
	_setRibbonEditValue(ID_RIBBON_STREAMLINE_THICKNESS, pParams->GetValueFloat(DOP_THICKNESS));
	_setRibbonEditValue(ID_RIBBON_STREAMLINE_NUM_ARROWS, pParams->GetValueInt(DOP_NUMARROWS));
	_setRibbonEditValue(ID_RIBBON_STREAMLINE_ARROW_SIZE, pParams->GetValueFloat(DOP_ARROWSIZE));
	_setRibbonEditCoordinate2D(ID_RIBBON_STREAMLINE_START, pParams->GetValuePointf(DOP_ORIGIN));
	_setRibbonEditValue(ID_RIBBON_STREAMLINE_NUM_STEPS, pParams->GetValueInt(DOP_INTEGRATIONSTEPS));
	_setRibbonEditValue(ID_RIBBON_STREAMLINE_STEP_LEN, pParams->GetValueFloat(DOP_STEPLENGTH));
	_setRibbonEditValue(ID_RIBBON_STREAMLINE_SMOOTHNESS, pParams->GetValueInt(DOP_SMOOTHNESS));
	_setRibbonEditValue(ID_RIBBON_STREAMLINE_NUMSTREAMLETS, pParams->GetValueInt(DOP_NUMDROPLETS));

	m_bArrowsStreamline = pParams->GetValueBool(DOP_SHOWARROWS)? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_STREAMLINE_SHOW_ARROWS, m_bArrowsStreamline);

	m_bIsStreamlet = pParams->GetValueBool(DOP_DRAWASDROPLETS)? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_STREAMLINE_ISSTREAMLET, m_bIsStreamlet);

	m_wndRibbonBar.RedrawWindow();
}

void CMainFrame::RibbonPathlinePropertyInit(CDrawingObjectParams *pParams)
{
	_setRibbonColorValue(ID_RIBBON_PATHLINE_COLOR, pParams->GetValueFloatColor(DOP_COLOR));
	_setRibbonColorValue(ID_RIBBON_PATHLINE_HALOCOLOR, pParams->GetValueFloatColor(DOP_HALOCOLOR));
	_setRibbonColorValue(ID_RIBBON_PATHLINE_ARROW_COLOR, pParams->GetValueFloatColor(DOP_ARROWCOLOR));
	_setRibbonEditValue(ID_RIBBON_PATHLINE_THICKNESS, pParams->GetValueFloat(DOP_THICKNESS));
	_setRibbonEditValue(ID_RIBBON_PATHLINE_NUM_ARROWS, pParams->GetValueInt(DOP_NUMARROWS));
	_setRibbonEditValue(ID_RIBBON_PATHLINE_ARROW_SIZE, pParams->GetValueFloat(DOP_ARROWSIZE));
	_setRibbonEditValue(ID_RIBBON_PATHLINE_STARTFRAME, pParams->GetValueInt(DOP_STARTFRAME));
	_setRibbonEditCoordinate2D(ID_RIBBON_PATHLINE_START, pParams->GetValuePointf(DOP_ORIGIN));
	_setRibbonEditValue(ID_RIBBON_PATHLINE_NUM_STEPS, pParams->GetValueInt(DOP_INTEGRATIONSTEPS));
	_setRibbonEditValue(ID_RIBBON_PATHLINE_STEP_LEN, pParams->GetValueFloat(DOP_STEPLENGTH));
	_setRibbonEditValue(ID_RIBBON_PATHLINE_SMOOTHNESS, pParams->GetValueInt(DOP_SMOOTHNESS));
	_setRibbonEditValue(ID_RIBBON_PATHLINE_NUMPATHLETS, pParams->GetValueInt(DOP_NUMDROPLETS));
	

	m_bArrowsPathline = pParams->GetValueBool(DOP_SHOWARROWS)? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_PATHLINE_SHOW_ARROWS, m_bArrowsPathline);

	m_bPathlineUseStartframe = pParams->GetValueBool(DOP_USE_STARTFRAME)? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_PATHLINE_USE_STARTFRAME, m_bPathlineUseStartframe);

	m_bIsPathlet = pParams->GetValueBool(DOP_DRAWASDROPLETS)? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_PATHLINE_ISPATHLET, m_bIsPathlet);

	m_wndRibbonBar.RedrawWindow();
}

void CMainFrame::RibbonStreaklinePropertyInit(CDrawingObjectParams *pParams)
{
	_setRibbonColorValue(ID_RIBBON_STREAKLINE_COLOR, pParams->GetValueFloatColor(DOP_COLOR));
	_setRibbonColorValue(ID_RIBBON_STREAKLINE_HALOCOLOR, pParams->GetValueFloatColor(DOP_HALOCOLOR));
	_setRibbonColorValue(ID_RIBBON_STREAKLINE_ARROWCOLOR, pParams->GetValueFloatColor(DOP_ARROWCOLOR));
	_setRibbonEditValue(ID_RIBBON_STREAKLINE_THICKNESS, pParams->GetValueFloat(DOP_THICKNESS));
	_setRibbonEditValue(ID_RIBBON_STREAKLINE_NUM_ARROWS, pParams->GetValueInt(DOP_NUMARROWS));
	_setRibbonEditValue(ID_RIBBON_STREAKLINE_ARROW_SIZE, pParams->GetValueFloat(DOP_ARROWSIZE));
	_setRibbonEditValue(ID_RIBBON_STREAKLINE_STARTFRAME, pParams->GetValueInt(DOP_STARTFRAME));
	_setRibbonEditCoordinate2D(ID_RIBBON_STREAKLINE_ORIGIN, pParams->GetValuePointf(DOP_ORIGIN));
	_setRibbonEditValue(ID_RIBBON_STREAKLINE_NUM_STEPS, pParams->GetValueInt(DOP_INTEGRATIONSTEPS));
	_setRibbonEditValue(ID_RIBBON_STREAKLINE_STEP_LEN, pParams->GetValueFloat(DOP_STEPLENGTH));
	_setRibbonEditValue(ID_RIBBON_STREAKLINE_SMOOTHNESS, pParams->GetValueInt(DOP_SMOOTHNESS));

	m_bArrowsStreakline = pParams->GetValueBool(DOP_SHOWARROWS)? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_STREAKLINE_SHOW_ARROWS, m_bArrowsStreakline);

	m_bStreakLineAsParticles = pParams->GetValueBool(DOP_RENDER_AS_PARTICLES)? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_STREAKLINE_AS_PARTICLES, m_bStreakLineAsParticles);

	m_bStreaklineUseStartframe = pParams->GetValueBool(DOP_USE_STARTFRAME)? TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_STREAKLINE_USE_STARTFRAME, m_bStreaklineUseStartframe);

	m_wndRibbonBar.RedrawWindow();
}

void CMainFrame::RibbonTimelinePropertyInit(CDrawingObjectParams *pParams)
{
	_setRibbonColorValue(ID_RIBBON_TIMELINE_COLOR, pParams->GetValueFloatColor(DOP_COLOR));
	_setRibbonColorValue(ID_RIBBON_TIMELINE_HALOCOLOR, pParams->GetValueFloatColor(DOP_HALOCOLOR));

	_setRibbonEditValue(ID_RIBBON_TIMELINE_THICKNESS, pParams->GetValueFloat(DOP_THICKNESS));
	_setRibbonEditValue(ID_RIBBON_TIMELINE_SMOOTHNESS, pParams->GetValueInt(DOP_SMOOTHNESS));

	_setRibbonEditCoordinate2D(ID_RIBBON_TIMELINE_SEED_START, pParams->GetValuePointf(DOP_ORIGIN));
	_setRibbonEditCoordinate2D(ID_RIBBON_TIMELINE_SEED_END, pParams->GetValuePointf(DOP_PTEND));
	_setRibbonEditValue(ID_RIBBON_TIMELINE_SEEDLINE_SAMPLES, pParams->GetValueInt(DOP_NUM_SAMPLES));

	_setRibbonEditValue(ID_RIBBON_TIMELINE_STEP_SIZE, pParams->GetValueFloat(DOP_STEPLENGTH));
	_setRibbonEditValue(ID_RIBBON_TIMELINE_START_FRAME, pParams->GetValueInt(DOP_STARTFRAME));

	m_bTimelineUseStartFrame = pParams->GetValueBool(DOP_USE_STARTFRAME)?TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_TIMELINE_FIXED_START_FRAME, m_bTimelineUseStartFrame);

	_setRibbonEditValue(ID_RIBBON_TIMELINE_INTEGRATIONSTEPS, pParams->GetValueInt(DOP_INTEGRATIONSTEPS));

	m_bTimelineShowSeedingline = pParams->GetValueBool(DOP_SHOW_SEEDINGLINE)?TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_TIMELINE_SHOW_SEEDINGLINE, m_bTimelineShowSeedingline);

	m_bTimeLineShowTrajectory = pParams->GetValueBool(DOP_SHOWTRAJECTORY)?TRUE:FALSE;
	_setRibbonCheckValue(ID_RIBBON_TIMELINE_SHOW_TRAJECTORY, m_bTimeLineShowTrajectory);

	if ( pParams->childParams && m_bTimeLineShowTrajectory) {
		ActivateSpeedLineContextCategory(pParams->GetChildParams());
	}

	m_wndRibbonBar.RedrawWindow();
}

void CMainFrame::OnSliderIsoVal()
{
	CMFCRibbonSlider *pSlider = reinterpret_cast<CMFCRibbonSlider*>(m_wndRibbonBar.FindByID(ID_SLIDER_ISO_VAL, FALSE));
	if (pSlider)
	{
		CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
		if (pView)
		{
			float fPos = pSlider->GetPos()/10.0f;

			CString str;
			str.Format(_T("%.3f"), fPos);
			pSlider->SetToolTipText(str);
			pView->SetIsoValue(fPos);
		}
	}
}

void CMainFrame::_setRibbonEditValue(DWORD dwCtrlID, float fVal)
{
	CString strDummy;
	strDummy.Format(_T("%.3f"), fVal);
	_setRibbonEditValue(dwCtrlID, strDummy);
}

void CMainFrame::_setRibbonEditValue(DWORD dwCtrlID, int nVal)
{
	CString strDummy;
	strDummy.Format(_T("%d"), nVal);
	_setRibbonEditValue(dwCtrlID, strDummy);
}

void  CMainFrame::_setRibbonEditValue(DWORD dwCtrlID, const CString& str)
{
	CMFCRibbonEdit* pEditCtrl = reinterpret_cast<CMFCRibbonEdit*>(m_wndRibbonBar.FindByID(dwCtrlID, FALSE));
	if (pEditCtrl)
		pEditCtrl->SetEditText(str);
}

void CMainFrame::_setRibbonEditCoordinate2D(DWORD dwCtrlID, const CPointf &point)
{
	_setRibbonEditCoordinate2D(dwCtrlID, point.x, point.y);
}

void CMainFrame::_setRibbonEditCoordinate2D(DWORD dwCtrlID, const CVector2D &vec)
{
	_setRibbonEditCoordinate2D(dwCtrlID, vec.x, vec.y);
}

void CMainFrame::_setRibbonEditCoordinate2D(DWORD dwCtrlID, float fX, float fY)
{
	CMFCRibbonEdit* pEditCtrl = reinterpret_cast<CMFCRibbonEdit*>(m_wndRibbonBar.FindByID(dwCtrlID, FALSE));
	if (pEditCtrl) {
		CString strDummy;
		strDummy.Format(_T("%.3f,%.3f"), fX, fY);
		pEditCtrl->SetEditText(strDummy);
	}
}

void CMainFrame::_setRibbonColorValue(DWORD dwCtrlID, const floatColor &color)
{
	CMFCRibbonColorButton* pColorCtrl = reinterpret_cast<CMFCRibbonColorButton*>(m_wndRibbonBar.FindByID(dwCtrlID, FALSE));

	if (pColorCtrl) {
		COLORREF col = RGB( static_cast<BYTE>(color.red * 255), 
							static_cast<BYTE>(color.green * 255),
							static_cast<BYTE>(color.blue * 255));
	
		pColorCtrl->SetColor(col);
		pColorCtrl->UpdateColor(col);
	}
}

void CMainFrame::_setRibbonDropdownCurSel(DWORD dwCtrlID, DWORD_PTR dwItemData)
{
	CMFCRibbonComboBox* pCmboCtrl = reinterpret_cast<CMFCRibbonComboBox*>(m_wndRibbonBar.FindByID(dwCtrlID, FALSE));
	pCmboCtrl->SelectItem( dwItemData );
}

#include "MFCRibbonCheckBoxStub.h"

void CMainFrame::_setRibbonCheckValue(DWORD dwCtrlID, BOOL bCheck)
{
	CMFCRibbonCheckBox* pCheckCtrl = reinterpret_cast<CMFCRibbonCheckBox*>(m_wndRibbonBar.FindByID(dwCtrlID, FALSE));
	if (pCheckCtrl)
		CMFCRibbonCheckBoxStub::SetCheck(pCheckCtrl, bCheck);
}

BOOL CMainFrame::_getRibbonEditValue(DWORD dwCtrlID, float &fVal, float fDefault)
{
	CString str;
	BOOL bResult = _getRibbonEditValue(dwCtrlID, str);
	if (bResult) {
		fVal = (str.IsEmpty())? fDefault : static_cast<float>(_tstof(str));
	}
	return bResult;
}

BOOL CMainFrame::_getRibbonEditValue(DWORD dwCtrlID, int &nVal, int nDefault)
{
	CString str;
	BOOL bResult =_getRibbonEditValue(dwCtrlID, str);
	if (bResult) {
		nVal = (str.IsEmpty())? nDefault : static_cast<int>(_tstol(str));
	}
	return bResult;
}

BOOL CMainFrame::_getRibbonEditValue(DWORD dwCtrlID, CString &str)
{
	CMFCRibbonEdit * pEditCtrl = reinterpret_cast<CMFCRibbonEdit *>(m_wndRibbonBar.FindByID(dwCtrlID, FALSE));
	if (pEditCtrl) {
		str = pEditCtrl->GetEditText();
		return TRUE;
	}

	return FALSE;
}

BOOL CMainFrame::_getRibbonEditCoordinate2D(DWORD dwCtrlID, float &fX, float &fY)
{
	CMFCRibbonEdit * pEditCtrl = reinterpret_cast<CMFCRibbonEdit *>(m_wndRibbonBar.FindByID(dwCtrlID, FALSE));

	if (pEditCtrl) {
		CString center(pEditCtrl->GetEditText());
		CString x, y;

		AfxExtractSubString(x, center, 0, ',');
		AfxExtractSubString(y, center, 1, ',');

		fX = static_cast<float>(atof(CStringA(x)));
		fY = static_cast<float>(atof(CStringA(y)));
		return TRUE;
	}

	return FALSE;
}

BOOL CMainFrame::_getRibbonColorValue(DWORD dwCtrlID, LPFLOATCOLOR pColor)
{
	CMFCRibbonColorButton* pColorCtrl = reinterpret_cast<CMFCRibbonColorButton*>(m_wndRibbonBar.FindByID(dwCtrlID, FALSE));
	
	if (pColorCtrl) {
		COLORREF col = pColorCtrl->GetColor();

		pColor->red		= GetRValue(col) / 255.0f;
		pColor->green	= GetGValue(col) / 255.0f;
		pColor->blue	= GetBValue(col) / 255.0f;
		pColor->alpha	= 1.0f;
		return TRUE;
	}

	return FALSE;
}

BOOL CMainFrame::_getRibbonDropdownCurSel(DWORD dwCtrlID, DWORD &nIndex)
{
	CMFCRibbonComboBox* pCmboCtrl = reinterpret_cast<CMFCRibbonComboBox*>(m_wndRibbonBar.FindByID(dwCtrlID, FALSE));
	if (pCmboCtrl) {
		nIndex = static_cast<int>(pCmboCtrl->GetItemData(pCmboCtrl->GetCurSel()));
		return TRUE;
	}

	return FALSE;
}

void CMainFrame::_getRibbonCheckValue(DWORD dwCtrlID, bool &bCheck)
{
	CMFCRibbonCheckBox* pCheckCtrl = reinterpret_cast<CMFCRibbonCheckBox*>(m_wndRibbonBar.FindByID(dwCtrlID, FALSE));
	bCheck = CMFCRibbonCheckBoxStub::GetCheck( pCheckCtrl )? true:false ;
}

void CMainFrame::OnUpdateRibbonRectSolid(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bRectSolid);
}

void CMainFrame::OnUpdateRibbonEllipseSolid(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bEllipseSolid);
}

void CMainFrame::OnUpdateRibbonTriangleSolid(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bTriangleSolid);
}

void CMainFrame::OnUpdateRibbonVortexAutoAdjustSize(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bVortexAutoSize);
}

void CMainFrame::OnRibbonGotoFrame()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		CMFCRibbonEdit *pEdit = reinterpret_cast<CMFCRibbonEdit*>(m_wndRibbonBar.FindByID(ID_RIBBON_GOTO_FRAME, FALSE));
		if (pEdit)
		{
			CString str(pEdit->GetEditText());
			pView->JumpToFrame(_tstol(str));
		}
	}
}

void CMainFrame::SetCurrentFrame(int nFrame)
{
	CMFCRibbonEdit *pEdit = reinterpret_cast<CMFCRibbonEdit*>(m_wndRibbonBar.FindByID(ID_RIBBON_GOTO_FRAME, FALSE));
	if (pEdit)
	{
		CString strDummy;
		strDummy.Format(_T("%d"), nFrame);

		pEdit->SetEditText(strDummy);
	}
}

void CMainFrame::OnStreamlineIntegrationSteps()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		int numSteps = 0;
		if (_getRibbonEditValue(ID_STREAMLINE_INTEGRATION_STEPS, numSteps)) {
			pView->SetStreamlineLength(numSteps);
		}
	}
}

void CMainFrame::OnStreamlineStepLenght()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		float stepSize = 0.0f;
		if (_getRibbonEditValue(ID_STREAMLINE_STEP_LENGHT, stepSize)) {
			pView->SetStreamlineStepSize(stepSize);
		}
	}
	
}

void CMainFrame::OnVortexThreshold()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		float vortexThreshold = 0.0f;
		CString str;

		if (_getRibbonEditValue(ID_VORTEX_THRESHOLD, str)) {
			AfxExtractSubString(str, str, 0, _T('%'));
			vortexThreshold = static_cast<float>(_tstof(str) /100.0f);
			pView->SetVortexThreshold(vortexThreshold);
		}
	}
}

void CMainFrame::UpdateMainRibbonCategory(const CView *pView)
{
	const CFlowIllustratorView *pV = reinterpret_cast<const CFlowIllustratorView*>(pView);

	_setRibbonEditValue(ID_STREAMLINE_INTEGRATION_STEPS, pV->GetStreamlineLength());
	_setRibbonEditValue(ID_STREAMLINE_STEP_LENGHT,pV->GetStreamlineStepSize() );

	_setRibbonEditValue(ID_RIBBON_ARROW_SIZE,pV->GetArrowLength() );

	CString str;
	str.Format(_T("%d%%"), static_cast<int>(pV->GetVortexThreshold() * 100) );
	_setRibbonEditValue(ID_VORTEX_THRESHOLD, str);


	//LIC
	_setRibbonEditValue(ID_RIBBON_LIC_WIDTH, pV->GetLICWidth());
	_setRibbonEditValue(ID_RIBBON_LIC_HEIGHT, pV->GetLICHeight());
	_setRibbonEditValue(ID_RIBBON_LIC_K_HALFSIZE, pV->GetLICHalfSize());
	_setRibbonEditValue(ID_RIBBON_LIC_SEED, pV->GetLICSeed());
}

void CMainFrame::OnUpdateRibbonStreamlineShowArrows(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bArrowsStreamline);
}


void CMainFrame::OnRibbonArrowSize()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		float fVal;
		if (_getRibbonEditValue(ID_RIBBON_ARROW_SIZE, fVal)) {
			pView->SetArrowLength(fVal);
		}
	}
}

void CMainFrame::OnRibbonVortexSnapToCore()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		pView->MoveSelectexVortexToCore();
	}
}

void CMainFrame::OnRibbonResetViewport()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		pView->ResetViewport();
	}
}

void CMainFrame::OnRibbonToBackground()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		pView->MoveSelected(-2);
	}
}

void CMainFrame::OnRibbinBackward()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		pView->MoveSelected(-1);
	}
}

void CMainFrame::OnRibbonToFront()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		pView->MoveSelected(2);
	}
}

void CMainFrame::OnRibbinForward()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		pView->MoveSelected(1);
	}
}

void CMainFrame::OnRibbonLicWidth()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		int nVal;
		if (_getRibbonEditValue(ID_RIBBON_LIC_WIDTH, nVal)) {
			pView->SetLICWidth(nVal);
		}
	}
}

void CMainFrame::OnRibbonLicHeight()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		int nVal;
		if (_getRibbonEditValue(ID_RIBBON_LIC_HEIGHT, nVal)) {
			pView->SetLICHeight(nVal);
		}
	}
}

void CMainFrame::OnRibbonLicKHalfsize()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		int nVal;
		if (_getRibbonEditValue(ID_RIBBON_LIC_K_HALFSIZE, nVal)) {
			pView->SetLICHalfSize(nVal);
		}
	}
}

void CMainFrame::OnRibbonLicSeed()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		int nVal;
		if (_getRibbonEditValue(ID_RIBBON_LIC_SEED, nVal)) {
			pView->SetLICSeed(nVal);
		}
	}
}

void CMainFrame::OnRibbonLicRecalc()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		pView->Dirty();
	}
}

void  CMainFrame::SetMinMaxIsoValue(float fMin, float fMax)
{
	CMFCRibbonSlider *pSldr = reinterpret_cast<CMFCRibbonSlider*>(m_wndRibbonBar.FindByID(ID_SLIDER_ISO_VAL, FALSE));

	if (pSldr)
	{
		pSldr->SetRange( static_cast<int>(fMin*10), static_cast<int>(fMax*10) );
	}
}

void CMainFrame::OnUpdateRibbonVortexTrajectoryShow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowTrajectory);
}

void CMainFrame::OnUpdateRibbonPathlineShowArrows(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bArrowsPathline);
}

void CMainFrame::OnRibbonAutoupdateVortexTrajectory()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		pView->ToggleAutoUpdateVortexTrajectory();
	}
}

void CMainFrame::OnUpdateRibbonAutoupdateVortexTrajectory(CCmdUI *pCmdUI)
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		pCmdUI->SetCheck(pView->GetAutoUpdateVortexTrajectory()) ;
	}
}

void CMainFrame::OnUpdateRibbonStreaklineShowArrows(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bArrowsStreakline);
}

void CMainFrame::OnUpdateRibbonStreaklineAsParticles(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bStreakLineAsParticles);
}

void CMainFrame::OnRibbonStreaklinesAsParticles()
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		pView->SetRenderStreaklineAsParticles( !pView->GetRenderStreaklineAsParticles());
	}
}

void CMainFrame::OnUpdateRibbonStreaklinesAsParticles(CCmdUI *pCmdUI)
{
	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		pCmdUI->SetCheck(pView->GetRenderStreaklineAsParticles());
	}
}

void CMainFrame::OnUpdateRibbonStreamlineIsstreamlet(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bIsStreamlet);
}

void CMainFrame::OnUpdateRibbonPathlineIspathlet(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bIsPathlet);
}

void CMainFrame::_notifyCild(UINT nMsgId, WPARAM wParam, LPARAM lParam)
{
	CView *pView(GetActiveView());
	if (pView) {
		pView->SendMessage(nMsgId, wParam, lParam);
	}
}

void CMainFrame::OnRibbonShowObjectsWindow()
{
	_notifyCild(FIV_SHOW_OBJECT_BROWSER);
}

void CMainFrame::OnUpdateRibbonEllipseHatched(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bEllipseHatched);
}

void CMainFrame::OnUpdateRibbonTriangleHatched(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bTriangleHatched);
}

void CMainFrame::OnRibbonStreamlineStyle()
{
	CMFCRibbonGallery *pWnd = reinterpret_cast<CMFCRibbonGallery*>(m_wndRibbonBar.FindByID(ID_RIBBON_STREAMLINE_STYLE, FALSE));
	if ( pWnd )
	{
		DWORD dwStyle = LineStyle[ pWnd->GetSelectedItem() ];

		CDrawingObjectParams params (DO_STREAMLINE);
		params.SetValue( DOP_SPEEDLINE_STYLE, static_cast<UINT>(dwStyle));
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonStreaklineStyle()
{
	CMFCRibbonGallery *pWnd = reinterpret_cast<CMFCRibbonGallery*>(m_wndRibbonBar.FindByID(ID_RIBBON_STREAKLINE_STYLE, FALSE));
	if ( pWnd )
	{
		DWORD dwStyle = LineStyle[ pWnd->GetSelectedItem() ];

		CDrawingObjectParams params (DO_STREAKLINE);
		params.SetValue( DOP_SPEEDLINE_STYLE, static_cast<UINT>(dwStyle));
		_objectPropertyChanged(&params);
	}
}


void CMainFrame::OnRibbonPathlineStyle()
{
	CMFCRibbonGallery *pWnd = reinterpret_cast<CMFCRibbonGallery*>(m_wndRibbonBar.FindByID(ID_RIBBON_PATHLINE_STYLE, FALSE));
	if ( pWnd )
	{
		DWORD dwStyle = LineStyle[ pWnd->GetSelectedItem() ];

		CDrawingObjectParams params (DO_PATHLINE);
		params.SetValue( DOP_SPEEDLINE_STYLE, static_cast<UINT>(dwStyle));
		_objectPropertyChanged(&params);
	}
}
