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

// FlowIllustratorView.cpp : implementation of the CFlowIllustratorView class
//

/*
	OpenGL setup and initialization is based on MSDN OpenGl Cube sample
	http://msdn.microsoft.com/en-us/library/7zx6z2e4(v=vs.90).aspx
*/

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "FlowIllustrator.h"
#endif

#include "FlowIllustratorDoc.h"
#include "FlowIllustratorView.h"

#include "amirareader.h"

#include "MainFrm.h"
#include <cmath>

#include "VortexObj.h"
#include "Line.h"
#include "Rectangle.h"
#include "Triangle.h"
#include "PathLine.h"


const float fDefaultArrowLength = 12.0f;
const float fMinObjectCreateSize = 15.0f;
static floatColor colSelectBox(0.0f, 0.0f, 0.0f);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define INITIAL_INTEGRATION_LEN 200
#define ALPHA_FADE_MAX .4f
#define ALPHA_FADE_MAX_MAIN .6f

// CFlowIllustratorView

IMPLEMENT_DYNCREATE(CFlowIllustratorView, CView)

BEGIN_MESSAGE_MAP(CFlowIllustratorView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
	ON_REGISTERED_MESSAGE(WM_TRACKRECT_CHANGED, &CFlowIllustratorView::OnTrackerRectChanged)
	ON_REGISTERED_MESSAGE(WM_TRACKRECT_ADJUST_MOUSE_POS, &CFlowIllustratorView::OnTrackerRectAdjustMousePos)
	ON_REGISTERED_MESSAGE(WM_TRACKRECT_END_TRACK, &CFlowIllustratorView::OnTrackerRectEndTrack)
	ON_REGISTERED_MESSAGE(WM_SVG_LOADED, &CFlowIllustratorView::OnSVGLoaded)
	ON_REGISTERED_MESSAGE(WM_DOC_FRAME_CHANGED, &CFlowIllustratorView::OnDocFrameChanged)
	ON_REGISTERED_MESSAGE(WM_VIEW_UPDATE_SELECTED, &CFlowIllustratorView::OnViewUpdateSelected)
	ON_REGISTERED_MESSAGE(DRAWINGOBJS_SELECT_ITEM, &CFlowIllustratorView::OnSelectDrawingObject)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
ON_COMMAND(ID_EDIT_COPY, &CFlowIllustratorView::OnEditCopy)
ON_COMMAND(ID_EDIT_CUT, &CFlowIllustratorView::OnEditCut)
ON_COMMAND(ID_EDIT_PASTE, &CFlowIllustratorView::OnEditPaste)
ON_COMMAND(ID_EDIT_SELECTALL, &CFlowIllustratorView::OnEditSelectall)
END_MESSAGE_MAP()

// CFlowIllustratorView construction/destruction

CFlowIllustratorView::CFlowIllustratorView()
{
	m_pDetectorFunc				= &CFlowIllustratorView::_isVortVorticityMagnitude;
	m_fStreamLineStep			= 0.1f;
	m_fVortexThreshold			= 0.3f;
	m_fArrowLength				= fDefaultArrowLength;
	m_nEditMode					= EM_VORTEX;
	m_dwPlay					= 0;
	m_ptMouseMove				= CPointf(-1.0f, -1.0f);
	m_ptLClick					= CPointf(-1.0f, -1.0f);
	m_bCreateDroplets			= FALSE;
	m_bAutoUpdateTrajectories	= FALSE;
	m_bRenderStreaklineAsParticles = FALSE;

	m_bPasteHere = FALSE;

	SetStreamlineLength(INITIAL_INTEGRATION_LEN);
	SetEditMode(EM_VORTEX);

	m_nDetectorFuncID	= 0;

	m_bLoadExample = TRUE;

	m_DetectorFunctions.push_back(	std::pair<CString, bool (CFlowIllustratorView::*)(const CAmiraVectorField2D*, const CPointf&)>
									(_T("Vorticity magnitude"), &CFlowIllustratorView::_isVortVorticityMagnitude) );
}

CFlowIllustratorView::~CFlowIllustratorView()
{
}

void CFlowIllustratorView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CFlowIllustratorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	m_ptContextMenu = point;
	ScreenToClient(&m_ptContextMenu);

	m_bPasteHere = TRUE;

	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CFlowIllustratorView diagnostics

#ifdef _DEBUG
void CFlowIllustratorView::AssertValid() const
{
	CView::AssertValid();
}

void CFlowIllustratorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CFlowIllustratorView message handlers

int CFlowIllustratorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFlowIllustratorRenderView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_DropTarget.Register(this);

	StartTimer();

	return 0;
}


void CFlowIllustratorView::OnSize(UINT nType, int cx, int cy)
{
	CFlowIllustratorRenderView::OnSize(nType, cx, cy);
}

void CFlowIllustratorView::OnDestroy()
{
	CFlowIllustratorRenderView::OnDestroy();

	StopTimer();

	m_CopyBuffer.clear();
}

void CFlowIllustratorView::OnDrawScene()
{
	if(m_pCreateDummy)
	{
		if ( IsPointInsideDomain(m_ptMouseMove) )
		{
			m_pCreateDummy->Draw();
		}
	}

	DrawSelection();
}

void CFlowIllustratorView::DrawSelection()
{
	for ( auto iter = m_Selected.begin() ; iter < m_Selected.end(); iter++)
	{
		(*iter)->Draw();
	}
}

void CFlowIllustratorView::OnTimer(UINT_PTR nIDEvent)
{
	CFlowIllustratorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc) {
		return;
	}

	if (IsPlaying()) {
		if (m_dwPlay == 1) {
			JumpToNextFrame();
		} else {
			JumpToPreviousFrame();
		}
	}
	
	RedrawWindow();

	CView::OnTimer(nIDEvent);

	// Eat spurious WM_TIMER messages
	MSG msg;
	while(::PeekMessage(&msg, m_hWnd, WM_TIMER, WM_TIMER, PM_REMOVE));
}


void CFlowIllustratorView::OnMouseMove(UINT nFlags, CPoint point)
{
	m_ptMouseMove = ScreenToDomainf(ScreenToGL(point));

	CRect rcView;
	GetClientRect(&rcView);

	for ( auto iter = m_Selected.begin(); iter < m_Selected.end(); iter++)
	{
		(*iter)->Highlight((*iter)->HitTest(m_ptMouseMove));
		(*iter)->Draw();
	}
	
	if (m_pCreateDummy)
	{
		if ( IsPointInsideDomain(m_ptMouseMove) )
		{
			switch (m_nEditMode)
			{
				case EM_STREAMLINE:
				case EM_PATHLINE:
				case EM_STREAKLINE:
					calcCharacteristicLine(m_pCreateDummy.get(), m_ptMouseMove);
					break;
				case EM_VORTEX:
					m_pCreateDummy->SetCenter(m_ptMouseMove);
					break;
				case EM_RECTANGLE:
				case EM_TRIANGLE:
				case EM_ELLIPSE:
					if (nFlags & MK_LBUTTON)
						m_pCreateDummy->SetRect(CRectF(m_ptLClick.x, m_ptLClick.y, m_ptMouseMove.x, m_ptMouseMove.y));
					break;
				case EM_SELECT:
				{
					if (m_pCreateDummy) {
						CRectF rect( m_ptLClick.x, m_ptLClick.y, m_ptMouseMove.x, m_ptMouseMove.y);
						rect.Normalize();
						m_pCreateDummy->SetRect(rect);
					}
				}
			}

			if (m_nEditMode & CHARACTERISTIC_LINE)
			{
				CStreamLine *pSL( reinterpret_cast<CStreamLine*>(m_pCreateDummy.get()) );
				pSL->SetArrowCount(5);
				pSL->ShowArrows(true);
				pSL->SetOrigin(m_ptMouseMove);
			}
		}
	}

	UpdateStatusBar();

	CView::OnMouseMove(nFlags, point);
}

void CFlowIllustratorView::JumpToFirstFrame()
{
	CFlowIllustratorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) {
		return;
	}

	m_bVorticityValid = FALSE;
	m_bVectorMagnitudeValid = FALSE;

	pDoc->FirstFrame();
}

void CFlowIllustratorView::JumpToPreviousFrame()
{
	CFlowIllustratorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) {
		return;
	}

	m_bVorticityValid = FALSE;
	m_bVectorMagnitudeValid = FALSE;

	pDoc->PreviousFrame();
}

void CFlowIllustratorView::TogglePlayback(DWORD dwDir)
{
	if (m_dwPlay == dwDir) {
		m_dwPlay = 0;
	} else {
		m_dwPlay = dwDir;
	}
}

void CFlowIllustratorView::JumpToNextFrame()
{
	CFlowIllustratorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) {
		return;
	}

	m_bVorticityValid = FALSE;
	m_bVectorMagnitudeValid = FALSE;

	pDoc->NextFrame();
}

void CFlowIllustratorView::JumpToLastFrame()
{
	CFlowIllustratorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) {
		return;
	}

	m_bVorticityValid = FALSE;
	m_bVectorMagnitudeValid = FALSE;

	pDoc->LastFrame();
}

void CFlowIllustratorView::JumpToFrame(int nFrame)
{
	CFlowIllustratorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) {
		return;
	}

	m_bVorticityValid = FALSE;
	m_bVectorMagnitudeValid = FALSE;

	pDoc->GotoFrame(nFrame);
}

void CFlowIllustratorView::StartTimer()
{
	SetTimer(1, 40, NULL);
}

void CFlowIllustratorView::StopTimer()
{
	KillTimer(1);
}

BOOL CFlowIllustratorView::IsPlaying()
{
	return (m_dwPlay != 0);
}

BOOL CFlowIllustratorView::IsPlayingFwd()
{
	return (m_dwPlay == 1);
}

BOOL CFlowIllustratorView::IsPlayingBckwd()
{
	return (m_dwPlay == 2);
}

BOOL CFlowIllustratorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (nFlags == MK_CONTROL)
	{
		if (zDelta > 0)
			gZoomFactor += 0.5f;
		else if (zDelta < 0)
			gZoomFactor -= 0.5f;

		if (gZoomFactor < 1.0f)
			gZoomFactor = 1.0f;

		m_bLICNoiseTexValid = FALSE;
		m_ptViewportCenter = m_ptMouseMove;

		CFlowIllustratorDoc *pDoc = GetDocument();
		if (pDoc)
		{
			const CAmiraVectorField2D *pVecField = pDoc->GetVectorfield();
			if (pVecField)
			{
				m_rcViewPort = m_rcDomain;

				CalcExtents();
				AdjustViewport(TRUE);
		
				CRect r;
				GetClientRect(&r);
				//Activate scroll bars and set their scroll range
				if (gZoomFactor > 1.0f) {

					CPoint ptCenter(r.Width()/2, r.Height()/2);	//center of screen
					CPoint ptDiff (pt.x - ptCenter.x, pt.y - ptCenter.y); //difference of mouse pos to center

					CRectF rcVP( m_rcViewPort );
					CRectF rcD( m_rcDomain );

					rcVP.Scale( m_fPixelUnitRatioX, m_fPixelUnitRatioY );
					rcD.Scale( m_fPixelUnitRatioX, m_fPixelUnitRatioY );

					if (m_bDrawCoordinateAxes) {
						rcD.InflateRect(COORDINATE_AXIS_WIDTH, COORDINATE_AXIS_HEIGHT);
					}

					if ( rcD.getWidth() > r.Width() ) {
						ShowScrollBar(SB_HORZ);
						int minSp =  static_cast<int>( abs(rcVP.m_Min.x - rcD.m_Min.x) );
						int maxSp = static_cast<int>( abs(rcD.m_Max.x - rcVP.m_Max.x) );
						SetScrollRange(SB_HORZ, 0, minSp+maxSp);
						SetScrollPos(SB_HORZ, minSp );
					}

					if ( rcD.getHeight() > r.Height() ) {
						ShowScrollBar(SB_VERT);
						int minSp =  static_cast<int>( abs(rcVP.m_Min.y - rcD.m_Min.y) );
						int maxSp = static_cast<int>( abs(rcD.m_Max.y - rcVP.m_Max.y) );
						SetScrollRange(SB_VERT, 0, minSp + maxSp);
						SetScrollPos(SB_VERT, minSp );
					}
				}
				else {
					ShowScrollBar(SB_BOTH, FALSE);
				}
			}

		} else {
			ShowScrollBar(SB_BOTH, FALSE);
		}
	}
	else
	{
		int streamlineStep = static_cast<int>(1.0f / m_fStreamLineStep);

		switch(m_nEditMode)
		{
			case EM_STREAMLINE:
			case EM_PATHLINE:
			case EM_STREAKLINE:
			{
				if (zDelta > 0)
					m_nStreamLineLen += streamlineStep;
				else if (zDelta < 0)
					m_nStreamLineLen -= streamlineStep;
		
				if ( m_nStreamLineLen < streamlineStep)
					m_nStreamLineLen = streamlineStep;

				SetStreamlineLength(m_nStreamLineLen);

				calcCharacteristicLine(m_pCreateDummy.get(), m_ptMouseMove);
				break;
			}
			case EM_VORTEX:
			{
				//set threshold between 0 and 100% in 5% steps
				if (zDelta > 0)
					m_fVortexThreshold += .05f;
				else if (zDelta < 0)
					m_fVortexThreshold -= .05f;

				if (m_fVortexThreshold > 1.0f) m_fVortexThreshold = 1.0f;
				if (m_fVortexThreshold < 0.0f) m_fVortexThreshold = 0.0f;
			
				break;
			}
		}
	}
	
	UpdateStatusBar();
	UpdateMainRibbonCategory();

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CFlowIllustratorView::DocumentLoaded(void)
{
	CFlowIllustratorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) {
		return;
	}

	const CAmiraVectorField2D *pVecField = pDoc->GetVectorfield();
	m_rcDomain = pVecField->GetDomainRect();

	ResetViewport();
	m_Selected.clear();

	m_pDrawObjMngr = pDoc->GetDrawingObjectMngr();

	SetEditMode(m_nEditMode);

	m_bVorticityValid	= FALSE;
	m_bVectorMagnitudeValid = FALSE;
	m_bLICNoiseTexValid	= FALSE;

	m_nLicWidth		= pVecField->GetExtentX();
	m_nLicHeight	= pVecField->GetExtentY();

	Dirty();

	UpdateStatusBar();
	UpdateMainRibbonCategory();
	OnDocFrameChanged(NULL, NULL);
	NotifyDrawingObjsChanged();
}

void CFlowIllustratorView::NewDocument(void)
{
	NotifyDrawingObjsChanged();

	m_Selected.clear();
	ResetViewport();
	UpdateStatusBar();
}

void CFlowIllustratorView::SetStreamlineColor(COLORREF nCol)
{
	m_NewObjectColor.red	= GetRValue(nCol) / 255.0f;
	m_NewObjectColor.green	= GetGValue(nCol) / 255.0f;
	m_NewObjectColor.blue	= GetBValue(nCol) / 255.0f;
	m_NewObjectColor.alpha	= 1.0f;

	if (m_pCreateDummy) {
		m_pCreateDummy->SetColor(m_NewObjectColor);
	}
}

void CFlowIllustratorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_ptLClick = ScreenToDomainf(ScreenToGL(point));

	if (m_pDrawObjMngr)
	{
		if (IsPointInsideDomain( m_ptLClick ) )
		{
			int nSelDummyIDx = m_pDrawObjMngr->Find(m_ptLClick);
			BOOL bContinue = TRUE;

			for (size_t i = 0; i < m_Selected.size(); i++)
			{
				UINT hitTest = m_Selected[i]->HitTest(m_ptLClick);
				if (hitTest != -1)  {
					bContinue = FALSE;

					if (i < m_Selected.size()) {
						bContinue = !m_Selected[i]->Track(this, m_ptLClick, nFlags);
					}
			
				}
			}
	
			if (bContinue)
			{
				switch(m_nEditMode)
				{
					case EM_STREAKLINE:
						AddNewStreakLine(m_ptLClick);
						break;
					case EM_PATHLINE:
						AddNewPathLine(m_ptLClick);
						break;
					case EM_STREAMLINE:
						AddNewStreamLine(m_ptLClick);
						break;
					case EM_VORTEX:
						AddNewVortex(m_ptLClick);
						break;
					case EM_SELECT:
						Select(nSelDummyIDx, nFlags & MK_CONTROL );

						if (nSelDummyIDx == -1)
						{
							CRectangle *pRect(new CRectangle(CRectF(m_ptLClick.x, m_ptLClick.y, m_ptLClick.x, m_ptLClick.y), colSelectBox));
							pRect->DrawStippled(true);
							m_pCreateDummy = shared_ptr<CDrawingObject>(pRect);
						}

						break;
				}
			}
		} 
		else
		{
			Select(-1, false);
		}
	}
	CView::OnLButtonDown(nFlags, point);
}

void CFlowIllustratorView::Select(int nSelIDx, BOOL bAddToSelection)
{
	//If it is not added to the curret selection, the current selection has to be cleared
	if (!bAddToSelection || nSelIDx < 0) {
		OnSelectDrawingObject(nullptr, false);
	} 
	
	//If we have a valid index, (de)select it
	if (nSelIDx > -1)
	{
		//Get the object with the specified index
		auto pObj = m_pDrawObjMngr->GetAt(nSelIDx);	
		
		//if it is selected, select it, done.
		if(!pObj->isSelected()) {
			m_Selected.push_back( shared_ptr<CSelection> (new CSelection(this, pObj)) );
		} 
		else 
		{	
			//remove the object from the current selection
			for (auto iter = m_Selected.begin(); iter < m_Selected.end(); ++iter) {
				if ( (*iter)->m_pSelected.get() == pObj.get() ) {
					m_Selected.erase(iter);
					break;
				}
			}
		}

		if (m_Selected.size() > 0) {
			OnSelectDrawingObject( pObj.get() , bAddToSelection);
		} else {
			OnSelectDrawingObject(nullptr, false);
		}
	}
}

void CFlowIllustratorView::calcStreamLine(const CPointf& point, CStreamLine *pStreamLine, bool bForward)
{
	CFlowIllustratorDoc *pDoc = GetDocument();

	if (pDoc)
	{
		const CAmiraVectorField2D *pVecField = pDoc->GetVectorfield();

		if (pVecField)
		{
			vector<CPointf>* pData = pStreamLine->GetDataPoints();
			pData->clear();
			CVector3D vec(point.x, point.y, 0);
			pVecField->integrateRK4( vec  , static_cast<int>(pStreamLine->GetMaxIntegrationLen()), pStreamLine->GetStepSize(), bForward,  pData, m_rcViewPort);
			pStreamLine->SetOrigin( point );
		}
	}
}

void CFlowIllustratorView::calcPathLine(const CPointf& point, CPathLine *pPathLine) const
{
	CFlowIllustratorDoc *pDoc = GetDocument();

	if (pDoc)
	{
		CAmiraVectorField2D *pVecField = const_cast<CAmiraVectorField2D*>(pDoc->GetVectorfield());

		if (pVecField)
		{
			vector<CPointf> *pData = pPathLine->GetDataPoints();
			pData->clear();
			CVector3D vec(point.x, point.y, 1.0f);

			int nFrame(0);
			if (pPathLine->UseFixedStartFrame()) {
				if (!pPathLine->NeedRecalc()) return;
				nFrame = pVecField->GetCurrentTimeStep();
				pVecField->GotoTimeStep(pPathLine->GetStartFrame());
			}

			pVecField->integrateRK4( vec, static_cast<int>(pPathLine->GetMaxIntegrationLen()), pPathLine->GetStepSize(), true,  pData, m_rcViewPort);
			pPathLine->SetOrigin( point );

			if (pPathLine->UseFixedStartFrame()) {
				pPathLine->NeedRecalc(false);
				pVecField->GotoTimeStep(nFrame);
			}
		}
	}
}

void CFlowIllustratorView::calcStreakLine(const CPointf& point, CStreakLine *pStreakLine)
{
	CFlowIllustratorDoc *pDoc = GetDocument();

	if (pDoc)
	{
		CAmiraVectorField2D *pVecField = const_cast<CAmiraVectorField2D*>(pDoc->GetVectorfield());

		if (pVecField)
		{
			vector<CPointf> *pData = pStreakLine->GetDataPoints();
			pData->clear();
			CVector3D vec(point.x, point.y, 1.0f);

			int nFrame(0);
			if (pStreakLine->UseFixedStartFrame()) {
				if (!pStreakLine->NeedRecalc()) return;
				nFrame = pVecField->GetCurrentTimeStep();
				pVecField->GotoTimeStep(pStreakLine->GetStartFrame());
			}

			pVecField->integrateStreakLine(vec  , static_cast<int>(pStreakLine->GetMaxIntegrationLen()), pStreakLine->GetStepSize(), true,  pData, m_rcViewPort);

			pStreakLine->SetOrigin( point );

			if (pStreakLine->UseFixedStartFrame()) {
				pStreakLine->NeedRecalc(false);
				pVecField->GotoTimeStep(nFrame);
			} 
		}
	}
}

void CFlowIllustratorView::calcTimeLine( CTimeLine *pTimeLine)
{
	CFlowIllustratorDoc *pDoc = GetDocument();

	if (pDoc)
	{
		CAmiraVectorField2D *pVecField = const_cast<CAmiraVectorField2D*>(pDoc->GetVectorfield());

		int nFrame(0);
		if (pTimeLine->UseFixedStartFrame()) {
			if (!pTimeLine->NeedRecalc() ) return;
			nFrame = pVecField->GetCurrentTimeStep();
			pVecField->GotoTimeStep(pTimeLine->GetStartFrame());
		}

		if (pVecField)
		{
			vector<CPointf> *pData = pTimeLine->GetDataPoints();

			pVecField->integrateTimeLine(	pTimeLine->GetOrigin(), pTimeLine->GetSeedLineEnd(), 
											pTimeLine->GetNumSamples(), pTimeLine->GetMaxIntegrationLen(), 
											pTimeLine->GetStepSize(), true, pData, m_rcDomain);
		}

		if (pTimeLine->UseFixedStartFrame()) {
			pTimeLine->NeedRecalc(false);
			pVecField->GotoTimeStep(nFrame);
		} 
	}
}

void CFlowIllustratorView::calcVortexTrajectory(CVortexObj *pVortex) const
{
	CFlowIllustratorDoc *pDoc = GetDocument();
	if (!pDoc) 
		return;

	const CAmiraVectorField2D *pVecField = pDoc->GetVectorfield();

	floatColor c(1,0,0);

	CSpeedLine *pTrajectory = new CSpeedLine(pVortex->GetColor(), pVortex->GetThickness(), 1.0f);
	vector<CPointf> *pPoints = pTrajectory->GetDataPoints();

	int time = pVecField->GetCurrentTimeStep();

	CPointf pt = pVortex->GetCenter();
	pPoints->push_back(pt);

	CRectF rect;

	if (m_rcDomain.getHeight() < m_rcDomain.getWidth())
		 rect = CRectF(0, 0, m_rcDomain.getHeight()/3, m_rcDomain.getHeight()/3); 
	else 
		rect = CRectF(0, 0, m_rcDomain.getWidth()/3, m_rcDomain.getWidth()/3);

	for (int i=time, j = 0; j < pVortex->GetTrajectoryLength() && i > 0; i--, j++) {
		rect.SetCenter(CPointf::toVector2D(pt));

		auto pVort(pVecField->GetVorticityField(rect, true, static_cast<float>(i)));

		if (pVort->GradientAscent(pt, pt))
			pPoints->push_back(pt);
	}

	pVortex->SetTrajectory(pTrajectory);
}

void CFlowIllustratorView::calcTimeLineTrajectory(CTimeLine *pTimeLine) const
{
	auto *pDoc = GetDocument();
	if (!pDoc) 
		return;

	auto *pVecField = pDoc->GetVectorfield();

	CPointf ptOrigin(pTimeLine->GetOrigin());
	CPointf ptEnd(pTimeLine->GetSeedLineEnd());

	CVector2D step ((ptEnd - ptOrigin) / 6.0f );
	CVector2D pos (ptOrigin.x, ptOrigin.y);
	pos += step;

	floatColor color(pTimeLine->GetColor());
	color.alpha = 1.0f;

	pTimeLine->DeleteAllChildren();

	int StartFrame = pTimeLine->UseFixedStartFrame()? pTimeLine->GetStartFrame() : pVecField->GetCurrentTimeStep();

	for (int i=0; i<3; i++) {
		CPathLine pl( CPointf::fromVector2D(pos), pTimeLine->GetMaxIntegrationLen(), StartFrame, 0, color, pTimeLine->GetStepSize() );
		calcPathLine( CPointf::fromVector2D(pos), &pl );

		CSpeedLine *pSpeedLine = new CSpeedLine(color, pTimeLine->GetThickness(), 1.0f);
		pSpeedLine->SetStyle( SL_TRANSPARENCY_INCREASE | SL_THICKNESS_CONST );

		vector<CPointf> *pPoints(pSpeedLine->GetDataPoints());
		vector<CPointf> *pPointsSrc(pl.GetDataPoints());

		pPoints->insert(pPoints->begin(), pPointsSrc->begin(), pPointsSrc->end());

		pTimeLine->AddTrajectory( pSpeedLine );

		pos += (step*2.0f);
	}
	
}

int CFlowIllustratorView::GetDetectorFunctionID() const
{
	return m_nDetectorFuncID;
}

void CFlowIllustratorView::GetDetectorFunctionNames(CStringArray &arrNames) const
{
	arrNames.RemoveAll();

	for (size_t i = 0; i < m_DetectorFunctions.size(); i++)
	{
		arrNames.Add(m_DetectorFunctions[i].first);
	}
}

void CFlowIllustratorView::UpdateStatusBar()
{
	CMainFrame *pMainFrm = reinterpret_cast<CMainFrame*>(GetParent());
	if (pMainFrm)
	{
		CFlowIllustratorDoc *pDoc = GetDocument();
		if (pDoc)
		{
			const CAmiraVectorField2D *pVecField = pDoc->GetVectorfield();
			if (pVecField)
			{
				CRect rcView;
				GetClientRect(&rcView);

				CString str;
				CString strPos;
				str.Format(_T("DimX: %d  DimY:%d  DimZ:%d  Streamline length: %d"),		pVecField->GetExtentX(), 
																						pVecField->GetExtentY(), 
																						pVecField->GetNumTimeSteps(), 
																						m_nStreamLineLen);

				strPos.Format(_T("X:%f  Y:%f"), m_ptMouseMove.x, m_ptMouseMove.y);
				pMainFrm->UpdateStatusBar(str, strPos);
			}
		}
	}
}

void CFlowIllustratorView::SetDetectorFunction(int nID)
{
	if(nID >= 0 && nID < static_cast<int>(m_DetectorFunctions.size()) )
	{
		m_nDetectorFuncID = nID;
		m_pDetectorFunc = m_DetectorFunctions[nID].second;
	}
}

bool CFlowIllustratorView::_isVortVorticityMagnitude(const CAmiraVectorField2D* /*pVecField*/, const CPointf& pt)
{
	if (AcquireVorticityField())
	{
		return (m_pVortFieldAbs->GetValue(pt.x, pt.y) >= m_pVortFieldAbs->GetMaxValue()*m_fVortexThreshold);
	}

	return false;
}

void CFlowIllustratorView::OnInitialUpdate()
{
	UpdateMainRibbonCategory();
	UpdateRibbonDropdowns();

	if (m_bLoadExample) {
		m_bLoadExample = FALSE;
		CFlowIllustratorDoc *pDoc = reinterpret_cast<CFlowIllustratorDoc*>(GetDocument());
		if (pDoc) {
			if (pDoc->OnOpenDocument(_T("examples/rectangle/rectangle.am"))) {
				CStringArray examples;
				examples.Add(_T("examples/rectangle/pathlines.svg"));
				examples.Add(_T("examples/rectangle/streamlines_vortices.svg"));
				examples.Add(_T("examples/rectangle/timelines.svg"));
				examples.Add(_T("examples/rectangle/vortexmotion.svg"));

				srand( (unsigned) time(NULL) );
				pDoc->LoadSVG(examples.GetAt(rand()%examples.GetSize()));
			}
		}
	}

	CView::OnInitialUpdate();
}

void CFlowIllustratorView::UpdateRibbonDropdowns()
{
	CMainFrame *pMainFrm = reinterpret_cast<CMainFrame*>(GetParent());
	if (pMainFrm)
	{
		CStringArray arrNames;
		GetDisplayFunctionNames(arrNames);
		pMainFrm->InitDisplayFunctionsDropdown(arrNames);
		pMainFrm->SetCurrDisplayFunction(GetDisplayFunctionID());

		GetDetectorFunctionNames(arrNames);
		pMainFrm->InitDetectorFunctionsDropdown(arrNames);
		pMainFrm->SetCurrDetectorFunction(GetDetectorFunctionID());

		pMainFrm->InitSpeedLineDropdowns();
	}
}

void CFlowIllustratorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool bCtrlPressed = (GetKeyState(VK_CONTROL) != 0);
	bool bAltPressed = (GetKeyState(VK_SHIFT) != 0); //alt key

	switch (nChar)
	{
		case VK_DELETE:
			deleteSelected();
			break;
		case VK_ESCAPE:
			Select(-1, false);
			break;
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			if (bAltPressed)
			{
				RotateSelected(nChar, bCtrlPressed);
			}
			else
			{
				TranslateSelected(nChar, bCtrlPressed);
			}
			break;
		case VK_SPACE:
			m_bCreateDroplets = !m_bCreateDroplets;
			if (m_nEditMode & EM_STREAMLINE || m_nEditMode & EM_PATHLINE || m_nEditMode & EM_STREAKLINE) {
				if (m_pCreateDummy) {
					reinterpret_cast<CStreamLine*>(m_pCreateDummy.get())->DrawAsDroplet(m_bCreateDroplets?true:false);
				}
			}
			break;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CFlowIllustratorView::deleteSelected()
{
	for (auto iter = m_Selected.begin(); iter < m_Selected.end(); iter++)
	{
		m_pDrawObjMngr->Delete((*iter)->m_pSelected.get());
	}

	OnSelectDrawingObject(nullptr, false);
}

void CFlowIllustratorView::OnSelectDrawingObject(CDrawingObject* pObj, BOOL bAddToSelection)
{
	if (!pObj) {
		m_Selected.clear();
	}

	NotifyDrawingObjsChanged();
	UpdateRibbon(pObj, bAddToSelection);
}

void CFlowIllustratorView::UpdateRibbon(CDrawingObject* pObj, BOOL bAddToSelection, BOOL bRefreshData)
{
	CMainFrame *pMainFrm = reinterpret_cast<CMainFrame*>(GetParentFrame());

	if (pMainFrm)
	{
		if(pObj)
		{
			CDrawingObjectParams params(pObj->GetParams());

			if ( bRefreshData ) {
				pMainFrm->UpdateRibbon( &params );
			} else {
				pMainFrm->ActivateContextCategory(pObj->GetType(), &params, bAddToSelection);
			}
		}
		else
		{
			pMainFrm->ActivateContextCategory(-1, nullptr, false );
		}
	}
}

void CFlowIllustratorView::UpdateSelected(CDrawingObjectParams &params)
{
	if (!m_Selected.empty())
	{
		for (size_t i=0;i < m_Selected.size(); i++)
		{
			auto pObj = m_Selected[i]->m_pSelected;

			bool bResult (pObj->SetParams(params));

			switch (pObj->GetType())
			{
				case DO_STREAMLINE:
				case DO_PATHLINE:
				case DO_STREAKLINE:
				case DO_TIMELINE:
				{
					CStreamLine *pStreamLine = reinterpret_cast<CStreamLine*>(pObj.get());

					//Only recalc if necessary
					if ( params.HasValue(DOP_ORIGIN) || params.HasValue(DOP_STEPLENGTH) || params.HasValue(DOP_INTEGRATIONSTEPS)
						|| params.HasValue(DOP_STARTFRAME) || params.HasValue(DOP_USE_STARTFRAME) || pObj->GetType() == DO_TIMELINE)
					{
						calcCharacteristicLine(pStreamLine, pStreamLine->GetOrigin());
					}

					if (params.HasValue(DOP_SMOOTHNESS)) {
						pStreamLine->Smooth(pStreamLine->GetSmoothness());
					}

					CalcStreamlineBoundingBox(pStreamLine);

					if (pObj->GetType() == DO_TIMELINE) {
						CTimeLine *pTimeLine = reinterpret_cast<CTimeLine*>(pObj.get());

						if (pTimeLine->ShowTrajectory()) {
							if( params.HasValue(DOP_SHOWTRAJECTORY) || params.HasValue(DOP_TRAJECTORYSTEPS) || 
							params.HasValue(DOP_TRANSPARENTTRAJECTORY)) 
							{
								calcTimeLineTrajectory(pTimeLine);
							}
						}
					}
					break;
				}
				case DO_VORTEX:
				{
					CVortexObj *pVort = reinterpret_cast<CVortexObj*>(pObj.get());
					if (pVort->ShowTrajectory())
					{
						if( params.HasValue(DOP_SHOWTRAJECTORY) || params.HasValue(DOP_TRAJECTORYSTEPS) || 
							params.HasValue(DOP_TRANSPARENTTRAJECTORY)) 
						{
							calcVortexTrajectory(pVort);
						}
					}
					break;
				}
			}

			if ( (pObj->GetType() == DO_VORTEX || pObj->GetType() == DO_TIMELINE) && bResult )
				if (params.GetValueBool(DOP_SHOWTRAJECTORY)) {
					OnSelectDrawingObject(pObj.get(), true);
			}

			//Update corresponding tracker rect
			if (i < m_Selected.size()) {
				m_Selected[i]->SetRect(pObj->GetRect());
				m_Selected[i]->SetRotation(pObj->GetRotation());
			}
		}

		NotifyDrawingObjsChanged();
	}
}

void CFlowIllustratorView::TranslateSelected(UINT direction, bool bMoveFast)
{
	if (!m_Selected.empty())
	{
		float offset = (bMoveFast)? 15.0f : 2.0f;
		CPointf pt( offset/m_xExtent, offset/m_yExtent);

		for (auto iter = m_Selected.begin(); iter < m_Selected.end(); iter++)
		{
			auto pObj = (*iter)->m_pSelected;

			switch(direction)
			{
				case VK_UP:
					pObj->Translate(0, pt.y);
					break;
				case VK_DOWN:
					pObj->Translate(0, -pt.y);
					break;
				case VK_LEFT:
					pObj->Translate(-pt.x, 0);
					break;
				case VK_RIGHT:
					pObj->Translate(pt.x, 0);
					break;
			}

			UpdateRibbon(pObj.get(), TRUE, TRUE);
		}
	}
}

void CFlowIllustratorView::RotateSelected(UINT direction, bool bMoveFast)
{
	if (!m_Selected.empty())
	{
		for ( auto iter = m_Selected.begin(); iter < m_Selected.end(); iter++)
		{
			auto pObj = (*iter)->m_pSelected;

			int offset = (bMoveFast)? 10 : 1;

			float rotation = pObj->GetRotation();

			switch(direction)
			{
				case VK_UP:
				case VK_LEFT:
					pObj->SetRotation(rotation+offset);
					break;
				case VK_DOWN:
				case VK_RIGHT:
					pObj->SetRotation(rotation-offset);
					break;
			}

			UpdateRibbon(pObj.get(), TRUE, TRUE);
		}
	}
}

void CFlowIllustratorView::AddNewVortex(const CPointf& point)
{
	CFlowIllustratorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) {
		return;
	}

	CVortexObj::VORTEX_STYLE nVortType(CVortexObj::ELLIPTIC);
	CVortexObj::VORTEX_ROTATION_DIR nVortDir(CVortexObj::CLOCK_WISE);
	float r1(0.0f), r2(0.0f), angle(0.0f);
	CPointf pt;
	const CAmiraVectorField2D *pVecField = pDoc->GetVectorfield();
	BOOL bResult = FALSE;

	//Only measure if point is actually inside a vortex.
	//Otherwise behaviour is undeterminded and it might hit the fan...
	if (m_pDetectorFunc && (this->*m_pDetectorFunc)(pVecField, point))
	{
		pt = GetVortexCore(point);

		if (measureVortex(m_pVortFieldAbs, pt, r1, r2, angle, m_fVortexThreshold))
		{
			//Is vortex source or sink?
			//Adjust appearence

			CRITICAL_POINT_TYPE nType = CENTER;
			if (pVecField)
				nType = pVecField->GetCriticalPointType(pt);

			float fCoreVal = m_pVortField->GetValue(pt.x, pt.y);
			nVortDir = (fCoreVal < 0)? CVortexObj::CLOCK_WISE : CVortexObj::COUNTER_CLOCKWISE;

			switch(nType)
			{
				case REPELLING_FOCUS:
					if (nVortDir == CVortexObj::CLOCK_WISE) {
						nVortType = CVortexObj::SPIRAL_CLOCKWISE;
					} else {
						nVortType = CVortexObj::SPIRAL_COUNTERCLOCKWISE;
					}
					break;
				case ATTRACTING_FOCUS:
					if (nVortDir == CVortexObj::CLOCK_WISE) {
						nVortType = CVortexObj::SPIRAL_COUNTERCLOCKWISE;
					} else {
						nVortType = CVortexObj::SPIRAL_CLOCKWISE;
					}
					break;
				default:
					nVortType = CVortexObj::ELLIPTIC;
			}
			bResult = TRUE;
		}
	}
	
	//If measuring the vortex failed for some reason, set default values
	if(!bResult)
	{
		pt			= point;
		r1			= r2 = (15.0f*gPixelPerUnit);
		angle		= 0;
		nVortType	= CVortexObj::ELLIPTIC;
		nVortDir	= CVortexObj::CLOCK_WISE;
	}

	float vorticitty = m_pVortFieldAbs->GetValue(pt);
	float revolutions = (vorticitty > 3)? vorticitty/2.0f : 1.5F;
	if (revolutions > 5) revolutions = 5;

	float linethickness = revolutions;

	CVortexObj *e = new CVortexObj(pt.x, pt.y, r1, r2, m_fArrowLength*gPixelPerUnit, m_fVortexThreshold, m_NewObjectColor, revolutions, nVortType, nVortDir);
	if (nVortType != CVortexObj::ELLIPTIC) {
		e->SetAppearance(CVortexObj::DOUBLE_SPIRAL);
		e->SetNumArrows(static_cast<int>(revolutions));
	} else {
		e->SetNumArrows(static_cast<int>(revolutions)*2);
	}
	e->SetRotation(angle);
	e->SetThickness(linethickness);

	m_pDrawObjMngr->Add(e);
}

CPointf CFlowIllustratorView::GetVortexCore(const CPointf& point)
{
	if (!AcquireVorticityField()) return CPointf(-1,-1);

	CPointf pt;

	m_pVortFieldAbs->GradientAscent(point, pt);

	return pt;
}

BOOL CFlowIllustratorView::measureVortex(CScalarField2D *pVortField, const CPointf &ptVortexCore, float &radius1, float &radius2, float &angle, float fThreshold)
{
	
	//Get gradient and co-gradient and follow them to find the extent of the vortex
	CVector2D gradient = pVortField->GetGradient(ptVortexCore.x, ptVortexCore.y);
	CVector2D coGrad(-gradient.y, gradient.x);

	if (gradient.abs() > 1e-5)
	{
		CVector2D dummyG(ptVortexCore.x - gradient.x, ptVortexCore.y - gradient.y);
		CVector2D dummyCG(ptVortexCore.x - coGrad.x, ptVortexCore.y - coGrad.y);

		float threshold = pVortField->GetValue(ptVortexCore.x, ptVortexCore.y) * fThreshold;

		while (pVortField->InsideDomain(dummyG) && pVortField->GetValue(dummyG.x, dummyG.y) >= threshold)
		{
			dummyG -= gradient;
		}

		while (pVortField->InsideDomain(dummyCG) && pVortField->GetValue(dummyCG.x, dummyCG.y) >= threshold)
		{
			dummyCG -= coGrad;
		}

		radius1 = sqrt(pow( float(ptVortexCore.x-dummyG.x), 2) + pow( float(ptVortexCore.y-dummyG.y), 2));
		radius2 = sqrt(pow( float(ptVortexCore.x-dummyCG.x), 2) + pow( float(ptVortexCore.y-dummyCG.y), 2));

		gradient.Normalize();
		coGrad.Normalize();

		CVector2D xAxis(1.0f, 0.0f);

		angle = RadToDeg(acos(gradient*xAxis));

		return TRUE;
	}
	else
	{
		radius1 = 0;
		radius2 = 0;
		angle	= 0;
	}

	return FALSE;
}

void CFlowIllustratorView::AddNewRectangle(const CPointf& ptMouseDown, const CPointf& ptMouseUp)
{
	float width = ptMouseUp.x - ptMouseDown.x;
	float height = ptMouseUp.y - ptMouseDown.y;

	if ((fabs(width)*m_fPixelUnitRatioX) < fMinObjectCreateSize) {
		if (width < 0) {
			width = -fMinObjectCreateSize/m_fPixelUnitRatioX;
		} else {
			width = fMinObjectCreateSize/m_fPixelUnitRatioX;
		}
	}

	if ((fabs(height)*m_fPixelUnitRatioY) < fMinObjectCreateSize) {
		if (height < 0) {
			height = -fMinObjectCreateSize/m_fPixelUnitRatioY;
		} else {
			height = fMinObjectCreateSize/m_fPixelUnitRatioY;
		}
	}

	m_pDrawObjMngr->Add(new CRectangle(CRectF(m_ptLClick.x, m_ptLClick.y, m_ptLClick.x + width, m_ptLClick.y + height), m_NewObjectColor, true));
	ResetCreateDummy();
}

void CFlowIllustratorView::AddNewEllipse(const CPointf& ptMouseDown, const CPointf& ptMouseUp)
{
	float width = ptMouseUp.x - m_ptLClick.x;
	float height = ptMouseUp.y - m_ptLClick.y;

	if ((fabs(width)*m_fPixelUnitRatioX) < fMinObjectCreateSize) {
		if (width < 0) {
			width = -fMinObjectCreateSize/m_fPixelUnitRatioX;
		} else {
			width = fMinObjectCreateSize/m_fPixelUnitRatioX;
		}
	}

	if ((fabs(height)*m_fPixelUnitRatioY) < fMinObjectCreateSize) {
		if (height < 0) {
			height = -fMinObjectCreateSize/m_fPixelUnitRatioY;
		} else {
			height = fMinObjectCreateSize/m_fPixelUnitRatioY;
		}
	}

	m_pDrawObjMngr->Add(new CEllipsoid(ptMouseDown.x + (width/2.0f), ptMouseDown.y + (height/2.0f), fabs(width/2.0f), fabs(height/2.0f), m_NewObjectColor, true));

	ResetCreateDummy();
}

void CFlowIllustratorView::ResetCreateDummy()
{
	if (m_pCreateDummy) {
		m_pCreateDummy->SetRect(CRectF(0,0,0,0));
	}
}

void CFlowIllustratorView::AddNewTriangle(const CPointf& ptMouseDown, const CPointf& ptMouseUp)
{
	float width = ptMouseUp.x - m_ptLClick.x;
	float height = ptMouseUp.y - m_ptLClick.y;

	if ((fabs(width)*m_fPixelUnitRatioX) < fMinObjectCreateSize) {
		if (width < 0) {
			width = -fMinObjectCreateSize/m_fPixelUnitRatioX;
		} else {
			width = fMinObjectCreateSize/m_fPixelUnitRatioX;
		}
	}

	if ((fabs(height)*m_fPixelUnitRatioY) < fMinObjectCreateSize) {
		if (height < 0) {
			height = -fMinObjectCreateSize/m_fPixelUnitRatioY;
		} else {
			height = fMinObjectCreateSize/m_fPixelUnitRatioY;
		}
	}

	m_pDrawObjMngr->Add(new CTriangle(CRectF(ptMouseDown.x, ptMouseDown.y, ptMouseDown.x + width, ptMouseDown.y + height), m_NewObjectColor));

	ResetCreateDummy();
}

void CFlowIllustratorView::AddNewLine(const CPointf& /*ptMouseDown*/, const CPointf& /*ptMouseUp*/)
{
	AfxMessageBox(_T("AddNewLine"));
}

void CFlowIllustratorView::AddNewStreamLine(const CPointf& /*point*/)
{
	CStreamLine* pStreamLine(reinterpret_cast<CStreamLine*>(m_pCreateDummy->Duplicate()));

	CalcStreamlineBoundingBox(pStreamLine);
	pStreamLine->SetArrowCount(5);
	pStreamLine->ShowArrows(true);
	m_pDrawObjMngr->Add(pStreamLine);
}

void CFlowIllustratorView::AddNewStreakLine(const CPointf& /*point*/)
{
	CStreakLine* pStreakLine(reinterpret_cast<CStreakLine*>(m_pCreateDummy->Duplicate()));

	CalcStreamlineBoundingBox(pStreakLine);
	pStreakLine->SetArrowCount(5);
	pStreakLine->ShowArrows(true);
	pStreakLine->RenderAsParticles(m_bRenderStreaklineAsParticles?true:false);
	m_pDrawObjMngr->Add(pStreakLine);
}

void CFlowIllustratorView::AddNewPathLine(const CPointf& point)
{
	AddNewStreamLine(point);
}

void CFlowIllustratorView::AddNewTimeLine(const CPointf& ptSeedLineStart, const CPointf& ptSeedLineEnd)
{
	CTimeLine *pTimeLine = new CTimeLine(ptSeedLineStart, ptSeedLineEnd, 100, m_nStreamLineLen, m_fStreamLineStep, 2.0f, m_NewObjectColor);

	calcCharacteristicLine(pTimeLine, ptSeedLineStart);

	m_pDrawObjMngr->Add(pTimeLine);
}

void CFlowIllustratorView::SetEditMode(EDIT_MODE nMode)
{
	if (!m_Selected.empty()) {
		m_Selected.clear();
	}

	if (m_pCreateDummy) {
		m_pCreateDummy = nullptr;
	}

	m_nEditMode = nMode;

	CDrawingObject *pNewObj(nullptr);

	switch (m_nEditMode)
	{
		case  EM_VORTEX:
			pNewObj = new CVortexObj( 
						m_ptMouseMove.x, 
						m_ptMouseMove.y, 
						.1f/gZoomFactor , 
						.1f/gZoomFactor, 
						(m_fArrowLength*gPixelPerUnit)/gZoomFactor, 
						m_fVortexThreshold, 
						m_NewObjectColor );
			break;
		case EM_PATHLINE:
		{
			CFlowIllustratorDoc *pDoc = GetDocument();
			if (pDoc) {
				pNewObj = new CPathLine(m_ptMouseMove, m_nStreamLineLen, pDoc->GetCurrentFrameNo(), m_fArrowLength*gPixelPerUnit, m_NewObjectColor, m_fStreamLineStep);
				reinterpret_cast<CStreamLine*>(pNewObj)->DrawAsDroplet(m_bCreateDroplets?true:false);	
			}
			break;
		}
		case EM_STREAKLINE:
		{
			CFlowIllustratorDoc *pDoc = GetDocument();
			if (pDoc) {
				pNewObj = new CStreakLine(m_ptMouseMove, m_nStreamLineLen, pDoc->GetCurrentFrameNo(), m_fArrowLength*gPixelPerUnit, m_NewObjectColor, m_fStreamLineStep);
				reinterpret_cast<CStreakLine*>(pNewObj)->RenderAsParticles(m_bRenderStreaklineAsParticles?true:false);
			}
			break;
		}
		case EM_STREAMLINE:
			pNewObj = new CStreamLine(m_ptMouseMove, m_nStreamLineLen, m_fArrowLength*gPixelPerUnit, m_NewObjectColor, m_fStreamLineStep);
			reinterpret_cast<CStreamLine*>(pNewObj)->DrawAsDroplet(m_bCreateDroplets?true:false);
			break;
		case EM_RECTANGLE:
			pNewObj = new CRectangle(CRectF(0,0,0,0), m_NewObjectColor);
			break;
		case EM_TRIANGLE:
			pNewObj = new CTriangle(CRectF(0,0,0,0), m_NewObjectColor);
			break;
		case EM_ELLIPSE:
			pNewObj = new CEllipsoid(CRectF(0,0,0,0), m_NewObjectColor, false);
			break;
		case EM_TIMELINE :
			pNewObj = new CTimeLine(m_ptMouseMove, m_ptMouseMove, 0,0,.1f,2, m_NewObjectColor);
			break;
	}

	if (pNewObj) {
		m_pCreateDummy = shared_ptr<CDrawingObject>(pNewObj);
	}
}

void CFlowIllustratorView::SetStreamlineLength(int nNumSteps)
{
	m_nStreamLineLen = nNumSteps;

	if (m_nEditMode & CHARACTERISTIC_LINE)
	{
		if (m_pCreateDummy) {
			reinterpret_cast<CStreamLine*>( m_pCreateDummy.get() )->SetMaxIntegrationLen(m_nStreamLineLen);
		}
	}
}

void CFlowIllustratorView::SetStreamlineStepSize(float fStepSize)
{
	m_fStreamLineStep = fStepSize;

	if (m_nEditMode & CHARACTERISTIC_LINE)
	{
		if (m_pCreateDummy) {
			reinterpret_cast<CStreamLine*>(m_pCreateDummy.get())->SetStepSize(m_fStreamLineStep);
		}
	}
}

void CFlowIllustratorView::SetVortexThreshold(float fThreshold)
{
	m_fVortexThreshold = fThreshold;
}

int CFlowIllustratorView::GetStreamlineLength() const
{
	return m_nStreamLineLen;
}

float CFlowIllustratorView::GetStreamlineStepSize() const
{
	return m_fStreamLineStep;
}

float CFlowIllustratorView::GetVortexThreshold() const
{
	return m_fVortexThreshold;
}

void CFlowIllustratorView::SetArrowLength(float fArrowLen)
{
	m_fArrowLength = fArrowLen;
	Dirty();

	if (m_pCreateDummy)
	{
		if (m_pCreateDummy->GetType() == DO_VORTEX)
		{
			reinterpret_cast<CVortexObj*>( m_pCreateDummy.get() )->SetArrowSize(m_fArrowLength*m_fPixelUnitRatioX);
		}
		else if (m_pCreateDummy->GetType() == DO_STREAMLINE)
		{
			reinterpret_cast<CStreamLine*>( m_pCreateDummy.get() )->SetArrowSize(m_fArrowLength*m_fPixelUnitRatioX);
		}
	}
}

float CFlowIllustratorView::GetArrowLength() const
{
	return m_fArrowLength;
}

void CFlowIllustratorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CPointf ptUp = ScreenToDomainf(ScreenToGL(point));

	switch(m_nEditMode)
	{
		case EM_RECTANGLE:
			AddNewRectangle(m_ptLClick, ptUp);
			break;
		case EM_ELLIPSE:
			AddNewEllipse(m_ptLClick, ptUp );
			break;
		case EM_TRIANGLE:
			AddNewTriangle(m_ptLClick, ptUp);
			break;
		case EM_LINE:
			AddNewLine(m_ptLClick, ptUp);
			break;
		case EM_TIMELINE:
			AddNewTimeLine(m_ptLClick, ptUp);
			break;
		case EM_SELECT:
		{
			if (m_pCreateDummy) 
			{
				CRectF rect = m_pCreateDummy->GetRect();
				rect.Normalize();

				for (int i=0; i < m_pDrawObjMngr->Size(); i++) {
					CRectF r = m_pDrawObjMngr->GetAt(i)->GetRect();
					r.Normalize();
					if (r.IntersectRet(rect)) {
						Select(i, true);
					}
				}

				m_pCreateDummy = nullptr;
			}
			break;
		}
	}

	NotifyDrawingObjsChanged();
	CView::OnLButtonUp(nFlags, point);
}

void CFlowIllustratorView::calcCharacteristicLine(CDrawingObject* pObj, const CPointf &pOrigin)
{
	if (pObj->GetType() & DO_CHARACTERISTIC_LINE) 
	{
		if (!reinterpret_cast<CStreamLine*>(pObj)->NeedRecalc()) {
			return;
		}

		switch(pObj->GetType())
		{
			case DO_STREAMLINE:
				calcStreamLine(pOrigin, reinterpret_cast<CStreamLine*>(pObj));
				break;
			case DO_PATHLINE:
				calcPathLine(pOrigin, reinterpret_cast<CPathLine*>(pObj));
				break;
			case DO_STREAKLINE:
				calcStreakLine(pOrigin, reinterpret_cast<CStreakLine*>(pObj));
				break;
			case DO_TIMELINE:
				calcTimeLine( reinterpret_cast<CTimeLine*>(pObj) );
				break;
		}

		CalcStreamlineBoundingBox(reinterpret_cast<CStreamLine*>(pObj));
	}
}

void CFlowIllustratorView::AdvanceStreamObjects()
{
	CFlowIllustratorDoc *pDoc = GetDocument();
	if (!pDoc) return;

	const CAmiraVectorField2D *pVecField = const_cast<CAmiraVectorField2D*>(pDoc->GetVectorfield());
	if(!pVecField) return;

	CDrawingObjectMngr *pDrawObjMngr = pDoc->GetDrawingObjectMngr();

	if (m_pCreateDummy)
	{
		if (m_pCreateDummy->GetType() != DO_TIMELINE) {
			calcCharacteristicLine(m_pCreateDummy.get() , m_ptMouseMove);
		}
	}

	for (size_t i=0; i < pDrawObjMngr->Size(); i++) {
		CDrawingObject *pObj = pDrawObjMngr->GetAt(i).get();

		switch(pObj->GetType())
		{
			case DO_VORTEX:
			{
				CVortexObj *pVort = reinterpret_cast<CVortexObj*>(pObj);
				CPointf pt = pVort->GetCenter();
				CRectF rect;

				if (m_rcDomain.getHeight() < m_rcDomain.getWidth())
					 rect = CRectF(0, 0, m_rcDomain.getHeight()/3.0f, m_rcDomain.getHeight()/3.0f); 
				else 
					rect = CRectF(0, 0, m_rcDomain.getWidth()/3.0f, m_rcDomain.getWidth()/3.0f);

				rect.SetCenter(CPointf::toVector2D(pt));

				CScalarField2D *pVortField = pVecField->GetVorticityField(rect, true, static_cast<float>(pVecField->GetCurrentTimeStep()) );
				//float val = pVortField->GetValue(pt);

				if (pVortField->GetValue(pt) > 1e-3)
				{
					if (pVortField->GradientAscent(pt, pt))
					{
						float r1, r2, angle;
						if (measureVortex(pVortField, pt, r1, r2, angle, pVort->GetThreshold()))
						{

							pVort->SetCenter(pt);

							if (pVort->AutoAdjustSize()) {
								pVort->SetRadius1(r1);
								pVort->SetRadius2(r2);
								pVort->SetRotation(angle);
							}
						}
					}
					else
					{
						pDrawObjMngr->DeleteAt(i);
						i--;
					}

					if (m_bAutoUpdateTrajectories)
						calcVortexTrajectory(pVort);
				}
			}

			case DO_STREAMLINE:
			case DO_PATHLINE:
			case DO_STREAKLINE:
			case DO_TIMELINE:
				calcCharacteristicLine(pObj, reinterpret_cast<CStreamLine*>(pObj)->GetOrigin());
				if (pObj->GetType() == DO_TIMELINE) {
					if (reinterpret_cast<CTimeLine*>(pObj)->ShowTrajectory()) {
						calcTimeLineTrajectory(reinterpret_cast<CTimeLine*>(pObj));
					}
				}
				break;	
		}
	}

	if (!m_Selected.empty()) {
		for(size_t i = 0;i < m_Selected.size(); i++)
		{
			auto pObj = m_Selected[i]->m_pSelected;

			UpdateRibbon(pObj.get(), TRUE, TRUE);

			//Update tracker rects
			if (i < m_Selected.size()) {
				m_Selected[i]->SetRect(pObj->GetRect());
				m_Selected[i]->SetRotation(pObj->GetRotation());
			}
		}
	}
}

LRESULT CFlowIllustratorView::OnTrackerRectChanged(WPARAM wParam, LPARAM /*lParam*/)
{
	LPTRACKRECTUPDATE pTu = reinterpret_cast<LPTRACKRECTUPDATE>(wParam);

	for(size_t i = 0; i < m_Selected.size(); i++)
	{
		auto pObj = m_Selected[i]->m_pSelected;
		auto nObjType(pObj->GetType());

		if ( nObjType & DO_CHARACTERISTIC_LINE ) {
			m_Selected[i]->SetRotation(0);	//Characteristic lines cannot be rotated

			CStreamLine *pSL = reinterpret_cast<CStreamLine*>(pObj.get());
			pSL->NeedRecalc(true);
			pObj->Translate(pTu->ptMouseMove.x, pTu->ptMouseMove.y);

			calcCharacteristicLine(pSL, pSL->GetOrigin());	

			if (m_bAutoUpdateTrajectories) {
				if ( pObj->GetType() == DO_TIMELINE) {
					calcTimeLineTrajectory( reinterpret_cast<CTimeLine*>(pSL) );
				}
			}
		} else {
			CRectF rcDiff = pTu->rect.DiffRect(pTu->oldRect);
			CRectF rect = pObj->GetRect();

			if (!pTu->bRotate) {
				rect.InfalteRect(rcDiff);
				pObj->SetRect(rect);
			} else {
				float newAngle = pObj->GetRotation() + (pTu->angle - pTu->oldAngle);
				pObj->SetRotation( newAngle );
			}

			if (m_bAutoUpdateTrajectories) {
				if ( nObjType == DO_VORTEX) {
					calcVortexTrajectory( reinterpret_cast<CVortexObj*>(pObj.get()) );
				}
			}
		}

		if (!pTu->bRotate) {
			CRectF r(pObj->GetRect());
			r.Normalize();
			m_Selected[i]->SetRect(r);
		} else {
			m_Selected[i]->SetRotation(pObj->GetRotation());
		}
		UpdateRibbon(pObj.get(), TRUE, TRUE);
	}

	RedrawWindow();

	return 0;
}

LRESULT CFlowIllustratorView::OnTrackerRectAdjustMousePos(WPARAM wParam, LPARAM /*lParam*/)
{
	CPointf *pPoint = reinterpret_cast<CPointf*>(wParam);

	*pPoint = ScreenToDomainf(ScreenToGL(pPoint->ToCPoint()));

	return 0;
}

LRESULT CFlowIllustratorView::OnTrackerRectEndTrack(WPARAM wParam, LPARAM /*lParam*/)
{
	LPTRACKRECTUPDATE pTu = reinterpret_cast<LPTRACKRECTUPDATE>(wParam);

	CRectF rect(pTu->rect);
	rect.Normalize();

	for(size_t i = 0; i < m_Selected.size(); i++)
	{
		auto pObj = m_Selected[i]->m_pSelected;

		if (pObj->GetType() & DO_CHARACTERISTIC_LINE) {
			CStreamLine *pStreamLine = reinterpret_cast<CStreamLine*>(pObj.get());
			CalcStreamlineBoundingBox(pStreamLine);
			UpdateRibbon(pStreamLine, TRUE, TRUE);

			if (i < m_Selected.size()) {
				m_Selected[i]->SetRect(pStreamLine->GetRect());
			}
		}
	}

	RedrawWindow();

	return 0;
}

void CFlowIllustratorView::CopyFrameBuffer(BYTE **pBuffer, int &nWidth, int &nHeight) const
{
	if (*pBuffer) delete *pBuffer;

	nWidth = m_xExtent;
	nHeight = m_yExtent;

	*pBuffer = new BYTE[m_xExtent*m_yExtent*4];
	BYTE *dummy = new BYTE[m_xExtent*m_yExtent*4];

	glReadPixels(m_xMin, m_yMin, m_xExtent, m_yExtent, GL_BGRA, GL_UNSIGNED_BYTE, dummy);

	int nLineSize = m_xExtent*4;

	for (int j=0; j<m_yExtent; j++) {
		memcpy( *pBuffer+(j*nLineSize), dummy+( (m_yExtent-1-j)*nLineSize), nLineSize);
	}

	delete [] dummy;
}

void CFlowIllustratorView::UpdateMainRibbonCategory()
{
	CMainFrame *pMainFrm = reinterpret_cast<CMainFrame*>(GetParent());

	if (pMainFrm)
	{
		pMainFrm->UpdateMainRibbonCategory(this);
	}
}

void CFlowIllustratorView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	float UnitPerPxlX = (m_rcViewPort.getWidth() / m_xExtent);

	int pos = static_cast<int>(nPos);
	CString s;

	CFlowIllustratorDoc *pDoc = GetDocument();
	if (pDoc)
	{
		m_bLICNoiseTexValid = FALSE;

		const CAmiraVectorField2D *pVecField = pDoc->GetVectorfield();

		if (pVecField)
		{
			int nCurrPos = GetScrollPos(SB_HORZ);
			int nScrollFast = 10;

			CRectF rcVP( m_rcViewPort );
			CRectF rcD( m_rcDomain );

			rcVP.Scale( m_fPixelUnitRatioX, m_fPixelUnitRatioY );
			rcD.Scale( m_fPixelUnitRatioX, m_fPixelUnitRatioY );

			if (m_bDrawCoordinateAxes) {
				rcD.InflateRect(COORDINATE_AXIS_WIDTH, COORDINATE_AXIS_HEIGHT);
			}

			int minSp = static_cast<int>( abs(rcVP.m_Min.x - rcD.m_Min.x) );
			int maxSp = static_cast<int>( abs(rcD.m_Max.x - rcVP.m_Max.x) );

			int nMin(0), nMax(minSp+maxSp);

			int nScrollAmount = 0;

			switch(nSBCode)
			{
				case SB_LINELEFT:
					if (nCurrPos > nMin){
						nScrollAmount = -1;
					}
					break;
				case SB_PAGELEFT:
					if (nCurrPos > nMin){
						nScrollAmount = ((nCurrPos-nScrollFast) < nMin)? nMin-nCurrPos: -nScrollFast;
					}
					break;
				case SB_LINERIGHT:
					if (nCurrPos < nMax){
						nScrollAmount = 1;	
					}
					break;
				case SB_PAGERIGHT:
					if (nCurrPos < nMax){
						nScrollAmount = ((nCurrPos+nScrollFast) > nMax)? nMax-nCurrPos: nScrollFast;
					}
					break;
				case SB_THUMBTRACK:
					if (pos < 0)
					{
						if (nCurrPos > nMin){
							int diff = pos-nCurrPos;
							nScrollAmount = ((nCurrPos + diff) > nMin)? diff : nMin - nCurrPos+1;
						}

					} else if (pos > 0)
					{
						if (nCurrPos < nMax){
							int diff = pos-nCurrPos;
							nScrollAmount = ((nCurrPos + diff) < nMax)? diff : nMax - nCurrPos-1;
						}
					}
				
					break;
			}

			m_rcViewPort.Translate(nScrollAmount*UnitPerPxlX, 0);
			nCurrPos += nScrollAmount;

			SetScrollPos(SB_HORZ, nCurrPos);
			AdjustViewport();
		}
	}

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CFlowIllustratorView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	float UnitPerPxlY = (m_rcViewPort.getHeight() / m_yExtent);


	int pos = static_cast<int>(nPos);
	CString s;

	CFlowIllustratorDoc *pDoc = GetDocument();
	if (pDoc)
	{
		m_bLICNoiseTexValid = FALSE;

		const CAmiraVectorField2D *pVecField = pDoc->GetVectorfield();

		if (pVecField)
		{
			int nCurrPos = GetScrollPos(SB_VERT);
			int nScrollFast = 10;

			CRectF rcVP( m_rcViewPort );
			CRectF rcD( m_rcDomain );

			rcVP.Scale( m_fPixelUnitRatioX, m_fPixelUnitRatioY );
			rcD.Scale( m_fPixelUnitRatioX, m_fPixelUnitRatioY );

			if (m_bDrawCoordinateAxes) {
				rcD.InflateRect(COORDINATE_AXIS_WIDTH, COORDINATE_AXIS_HEIGHT);
			}

			int minSp = static_cast<int>( abs(rcVP.m_Min.y - rcD.m_Min.y) );
			int maxSp = static_cast<int>( abs(rcD.m_Max.y - rcVP.m_Max.y) );

			int nMin(0), nMax(minSp+maxSp);

			int nScrollAmount = 0;

			switch(nSBCode)
			{
				case SB_LINEDOWN:
					if (nCurrPos > nMin){
						nScrollAmount = 1;
					}
					break;
				case SB_PAGEDOWN:
					if (nCurrPos > nMin){
						nScrollAmount = ((nCurrPos-nScrollFast) < nMin)? nMin-nCurrPos: nScrollFast;
					}
					break;
				case SB_LINEUP:
					if (nCurrPos < nMax){
						nScrollAmount = -1;	
					}
					break;
				case SB_PAGEUP:
					if (nCurrPos < nMax){
						nScrollAmount = ((nCurrPos+nScrollFast) > nMax)? nMax-nCurrPos: -nScrollFast;
					}
					break;
				case SB_THUMBTRACK:
					if (pos < 0)
					{
						if (nCurrPos > nMin){
							int diff = pos-nCurrPos;
							nScrollAmount = ((nCurrPos + diff) > nMin)? diff : nMin - nCurrPos+1;
						}

					} 
					else if (pos > 0)
					{
						if (nCurrPos < nMax){
							int diff = pos-nCurrPos;
							nScrollAmount = ((nCurrPos + diff) < nMax)? diff : nMax - nCurrPos-1;
						}
					}
				
					break;
			}

			m_rcViewPort.Translate(0, -nScrollAmount*UnitPerPxlY);
			nCurrPos += nScrollAmount;

			SetScrollPos(SB_VERT, nCurrPos);
			AdjustViewport();
		}
	}

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

//Only moves to the center. Does not change size or orientation
void CFlowIllustratorView::MoveSelectexVortexToCore()
{
	for (size_t i = 0; i < m_Selected.size(); i++)
	{

		auto pSelected = m_Selected[i]->m_pSelected;

		if (pSelected && pSelected->GetType() == DO_VORTEX)
		{
			CVortexObj *pVort = reinterpret_cast<CVortexObj*>(pSelected.get());

			CPointf pt = GetVortexCore(pVort->GetCenter());

			pVort->SetCenter(pt);

			if (i < m_Selected.size()){
				 m_Selected[i]->SetRect(pVort->GetRect());
			}

			UpdateRibbon(pVort, TRUE, TRUE);
		}
	}
}

void CFlowIllustratorView::MoveSelected(int nDir)
{
	if (m_pDrawObjMngr) 
	{
		for (size_t i=0; i < m_Selected.size(); i++)
		{
			int nSelIdx = m_pDrawObjMngr->Find(m_Selected[i]->m_pSelected.get());

			switch(nDir)
			{
				case -2:
					m_pDrawObjMngr->MoveToBack(nSelIdx);
					break;
				case -1:
					m_pDrawObjMngr->MoveBackward(nSelIdx);
					break;
				case 1:
					 m_pDrawObjMngr->MoveForward(nSelIdx);
					break;
				case 2:
					m_pDrawObjMngr->MoveToFront(nSelIdx);
					break;
			}
		}

		//Notify dialog that something has changed
		NotifyDrawingObjsChanged();
	}
}

BOOL CFlowIllustratorView::OnDrop(COleDataObject* pDataObject, DROPEFFECT /*dropEffect*/, CPoint /*point*/)
{
	HGLOBAL hg = pDataObject->GetGlobalData(CF_HDROP);
	BOOL bResult = FALSE;

	if (hg)
	{
		HDROP hdrop = (HDROP) GlobalLock ( hg );
		
		if (hdrop)
		{
			TCHAR strFileName[2048];

			if (DragQueryFile(hdrop, 0, strFileName, 2048))
			{
				CString strDummy;
				if (AfxExtractSubString(strDummy, strFileName, 1, _T('.')))
				{
					strDummy = strDummy.MakeLower();
					if (strDummy == _T("svg")) 
					{
						CFlowIllustratorDoc *pDoc = GetDocument();
						if (pDoc)
						{
							bResult = pDoc->LoadSVG(strFileName);
						}
					}
					else if (strDummy == _T("am"))
					{
						CFlowIllustratorDoc *pDoc = GetDocument();
						if (pDoc)
						{
							bResult = pDoc->OnOpenDocument(strFileName);
						}
					}
				}
			}
		}
		GlobalUnlock ( hg );
	}

	return bResult;
}


DROPEFFECT CFlowIllustratorView::OnDragEnter(COleDataObject* pDataObject, DWORD /*dwKeyState*/, CPoint /*point*/)
{
	HGLOBAL hg = pDataObject->GetGlobalData(CF_HDROP);
	m_DropEffect = DROPEFFECT_NONE;

	if (hg)
	{
		HDROP hdrop = (HDROP) GlobalLock ( hg );
		
		if (hdrop)
		{
			TCHAR strFileName[2048];

			if (DragQueryFile(hdrop, 0, strFileName, 2048))
			{
				CString strDummy;
				if (AfxExtractSubString(strDummy, strFileName, 1, _T('.')))
				{
					strDummy = strDummy.MakeLower();
					if (strDummy == _T("svg") || strDummy == _T("am"))
						m_DropEffect = DROPEFFECT_MOVE;
				}
			}
		}
		GlobalUnlock ( hg );
	}

	return m_DropEffect;
}


DROPEFFECT CFlowIllustratorView::OnDragOver(COleDataObject* /*pDataObject*/, DWORD /*dwKeyState*/, CPoint /*point*/)
{
	return m_DropEffect;
}


void CFlowIllustratorView::OnDragLeave()
{
	m_DropEffect = DROPEFFECT_NONE;

	CView::OnDragLeave();
}

void CFlowIllustratorView::CalcStreamlineBoundingBox(CStreamLine *pStreamLine)
{
	//Ensures a streamline bounding box has a minimum width and height height -> makes it easyer for very straight
	//streamlines to select
	pStreamLine->CalcBoundingRect();
	CRectF rect = pStreamLine->GetRect();

	float fMinWidthHeight = (10.0f*gPixelPerUnit);

	if (rect.getHeight() <fMinWidthHeight)
	{
		CVector2D center = rect.GetCenter();
		rect.m_Min.y = center.y - (fMinWidthHeight/2.0f);
		rect.m_Max.y = center.y + (fMinWidthHeight/2.0f);
	}

	if (rect.getWidth() <fMinWidthHeight)
	{
		CVector2D center = rect.GetCenter();
		rect.m_Min.x = center.x - (fMinWidthHeight/2.0f);
		rect.m_Max.x = center.x + (fMinWidthHeight/2.0f);
	}

	pStreamLine->SetRect(rect);
}

LRESULT CFlowIllustratorView::OnSVGLoaded(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_Selected.clear();

	if (!m_pDrawObjMngr) {
		CFlowIllustratorDoc *pDoc = GetDocument();
		if (pDoc) {
			m_pDrawObjMngr = pDoc->GetDrawingObjectMngr();
		}
	}

	return 0;
}

LRESULT CFlowIllustratorView::OnDocFrameChanged(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	CFlowIllustratorDoc *pDoc = GetDocument();
	if (!pDoc) return 1;

	CMainFrame *pMainFrm = reinterpret_cast<CMainFrame*>(GetParent());
	if (pMainFrm)
	{
		pMainFrm->SetCurrentFrame(pDoc->GetCurrentFrameNo());
	}

	auto pVecfield = pDoc->GetVectorfield();
	if (pVecfield)
	{		
		if (AcquireVectorMagnitudeField())
		{
			MAX_VELOCITY = m_pVectorMagnitudeField->GetMaxValue();
		}
	}

	if (!pDoc->IsLoadingSVG()) {
		AdvanceStreamObjects();
	}

	return 0;
}

void CFlowIllustratorView::ToggleAutoUpdateVortexTrajectory()
{
	m_bAutoUpdateTrajectories = ! m_bAutoUpdateTrajectories;
}

BOOL CFlowIllustratorView::GetAutoUpdateVortexTrajectory()
{
	return m_bAutoUpdateTrajectories;
}

void CFlowIllustratorView::SetRenderStreaklineAsParticles(BOOL bAsParticles)
{
	m_bRenderStreaklineAsParticles = bAsParticles;
	if (m_nEditMode == EM_STREAKLINE) {
		if (m_pCreateDummy) {
			reinterpret_cast<CStreakLine*>(m_pCreateDummy.get())->RenderAsParticles( m_bRenderStreaklineAsParticles?true:false );
		}
	}
}

BOOL CFlowIllustratorView::GetRenderStreaklineAsParticles() const
{
	return m_bRenderStreaklineAsParticles;
}

void CFlowIllustratorView::GrowStreakLines(int nNumNewParticles)
{
	if (m_pDrawObjMngr)
	{
		for (size_t i=0; i < m_pDrawObjMngr->Size(); i++) {
			CDrawingObject *pObj = m_pDrawObjMngr->GetAt(i).get();
			if (pObj->GetType() == DO_STREAKLINE) {
				reinterpret_cast<CStreakLine*>(pObj)->SetMaxIntegrationLen( reinterpret_cast<CStreakLine*>(pObj)->GetMaxIntegrationLen() + nNumNewParticles);
				calcStreakLine(reinterpret_cast<CStreakLine*>(pObj)->GetOrigin(), reinterpret_cast<CStreakLine*>(pObj));
			}
		}
	}
}

LRESULT CFlowIllustratorView::OnViewUpdateSelected(WPARAM wParam, LPARAM /*lParam*/)
{
	CDrawingObjectParams *pParams = reinterpret_cast<CDrawingObjectParams*>(wParam);

	UpdateSelected( *pParams );
	return 0;
}

void CFlowIllustratorView::NotifyDrawingObjsChanged()
{
	CWnd *pWnd = GetParent();
	if (pWnd) {
		pWnd->SendMessage(DRAWINGOBJS_CHANGED, WPARAM(m_pDrawObjMngr));
	}
}

LRESULT CFlowIllustratorView::OnSelectDrawingObject(WPARAM wParam, LPARAM lParam)
{
	int nIndx = static_cast<int>(wParam);
	BOOL bAddToSelection = static_cast<BOOL>(lParam);

	Select(nIndx, bAddToSelection);

	SetFocus();

	return 0;
}

LRESULT CFlowIllustratorView::OnDeleteSelected(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	deleteSelected();
	return 0;
}

LRESULT CFlowIllustratorView::OnMoveUp(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	MoveSelected(1);
	return 0;
}

LRESULT CFlowIllustratorView::OnMoveDown(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	MoveSelected(-1);
	return 0;
}

void CFlowIllustratorView::OnEditCopy()
{
	m_CopyBuffer.clear();

	if (m_Selected.empty())
	{
		int indx = m_pDrawObjMngr->Find( CPointf(ScreenToDomainf(ScreenToGL(m_ptContextMenu))) );
		Select(indx, FALSE);
	}

	for (auto iter = m_Selected.begin(); iter != m_Selected.end(); ++iter)
	{
		auto pOriginal = (*iter)->m_pSelected;
		m_CopyBuffer.push_back( shared_ptr<CDrawingObject>(pOriginal->Duplicate()) );
	}
}

void CFlowIllustratorView::OnEditCut()
{
	OnEditCopy();
	deleteSelected();
}

void CFlowIllustratorView::OnEditPaste()
{
	m_Selected.clear();

	for ( auto iter =  m_CopyBuffer.begin(); iter != m_CopyBuffer.end(); ++iter)
	{
		shared_ptr<CDrawingObject> pObj( (*iter)->Duplicate() );

		if (m_CopyBuffer.size() == 1 && m_bPasteHere)
		{
			CPointf pt(ScreenToDomainf(ScreenToGL(m_ptContextMenu)));

			if ( pObj->GetType() & DO_CHARACTERISTIC_LINE ) {
				calcCharacteristicLine(pObj.get(), pt);
			} else {
				pObj->SetCenter(pt);
			}
				
		}

		m_Selected.push_back( shared_ptr<CSelection>( new CSelection(this, pObj) ) );
		m_pDrawObjMngr->Add( pObj );
	}

	m_bPasteHere = FALSE;
	NotifyDrawingObjsChanged();
}

void CFlowIllustratorView::OnEditSelectall()
{
	m_Selected.clear();

	for (size_t i=0; i<m_pDrawObjMngr->Size(); i++) {
		Select(i, TRUE);
	}
}
