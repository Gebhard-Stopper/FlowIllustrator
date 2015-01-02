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

// FlowIllustratorView.h : interface of the CFlowIllustratorView class
//

#pragma once


#include "Vector2D.h"
#include <vector>

#include "DrawingObject.h"
#include "BSpline.h"
#include "StreakLine.h"
#include "TimeLine.h"
#include "VortexObj.h"
#include "Selection.h"
#include "FlowIllustratorRenderView.h"

const UINT FIV_SHOW_OBJECT_BROWSER = RegisterWindowMessage(_T("FIV_SHOW_OBJECT_BROWSER"));
const UINT DRAWINGOBJS_CHANGED = RegisterWindowMessage(_T("DRAWINGOBJS_CHANGED"));

/**
 *	This class inherits the rendering capabilities of CFlowIllustratorRenderView and extents its original
 *	functionality by providing means for user interaction.
 */
class CFlowIllustratorView 
	: public CFlowIllustratorRenderView
{
/*
	0x01 indicates characteristic line
*/
static const DWORD CHARACTERISTIC_LINE = 0x01;

public: 
	enum EDIT_MODE {	EM_SELECT		= 0x0, 
						EM_VORTEX		= 0x2, 
						EM_STREAMLINE	= 0x5, 
						EM_STREAKLINE	= 0x9, 
						EM_PATHLINE		= 0x11,
						EM_TIMELINE		= 0x21,
						EM_RECTANGLE	= 0x40, 
						EM_TRIANGLE		= 0x80,  
						EM_ELLIPSE		= 0x100, 
						EM_LINE			= 0x200};

protected: // create from serialization only
	CFlowIllustratorView();
	DECLARE_DYNCREATE(CFlowIllustratorView)


// Attributes
protected:
	bool (CFlowIllustratorView::*m_pDetectorFunc)(const CAmiraVectorField2D*, const CPointf&);	//Pointer to current vortex detector function
	std::vector< std::pair<CString, bool (CFlowIllustratorView::*)(const CAmiraVectorField2D*, const CPointf&)> >m_DetectorFunctions;	//Vector containling all available vortex detector function

protected:
	CPointf				m_ptMouseMove;			/**< Current mouse position in domain coordinates. */
	CPointf				m_ptLClick;				/**< Mouse click position in domain coordinates. */

//Object creation
	shared_ptr<CDrawingObject>  m_pCreateDummy;					/**< Displayed as preview of new objects. */
	EDIT_MODE					m_nEditMode;					/**< Current edit mode (e.g. select, create vortex etc.) */
	int							m_nStreamLineLen;				/**< Number of steps used to integrate a new streamline */
	float						m_fStreamLineStep;				/**< Step width used to create a new streamline */
	float						m_fArrowLength;					/**< Length of a new arrow in pixel. */
	float						m_fVortexThreshold;				/**< Threshold used to measure a new vortex(relative to peak value at vortex core). */
	BOOL						m_bCreateDroplets;				/**< Indicates, if stream and path lines are created as droplets. */
	BOOL						m_bRenderStreaklineAsParticles; /**< Indicates, if new streak lines are displayed as particles. */
	
	int							m_nDetectorFuncID;				/**< ID/index of the currently selected vortex detector function. */

	vector<shared_ptr<CSelection>>	m_Selected;			/**< Vector of currently selected objects. */

	BOOL			m_bAutoUpdateTrajectories;			/**< Indicates, if vortex trajectories are updated each time the simulation time step is changed. */

	//File Drag and Drop
	COleDropTarget	m_DropTarget;	/**< Registers this CFlowIllustratorView as target for drag and drop operations. Used to open files. */
	DROPEFFECT		m_DropEffect;	/**< Symbol, displayed during a drag and drop operation. Indicates, if drag and drop is successfully possible. */

	//Copy & pase
	CPoint								m_ptContextMenu;	/**< Location in domain coordinates, where the user clicked the right mouse button to ober the context menu. */
	BOOL								m_bPasteHere;		/**< If TRUE, a single object is pasted at the m_ptContextMenu, if the user selected "Paste" from the context menu. */
	vector<shared_ptr<CDrawingObject>>	m_CopyBuffer;		/**< Buffer, that hold copies of all objects the user copied for a copy-and-pase operation. */

	BOOL			m_bLoadExample;

// Operations
protected:
	//Object slection

	/**
	 *	Adds the CDrawingObject, stored in m_pDrawObjMngr to the vector of selected objects.
	 *	
	 *	@param nSelIDx Index of the newly selected object.
	 *	@param bAddToSelection	If TRUE, the newly selected object is appended to the vector of currently selected objects. 
								Otherwise, the vector of selected objects is cleared, before the object, referenced by nSelIDx is added.
	 */
	void Select(int nSelIDx, BOOL bAddToSelection);

	/**
	 *	This function is called by Select() to finalize the selection process.
	 *	It causes the data displayed in the ribbon bar to be updated, and sends the DRAWINGOBJS_CHANGED message.
	 */
	void OnSelectDrawingObject(CDrawingObject* pObj, BOOL bAddToSelection);

	/**
	 *	Deletes all currently selected CDrawingObjects.
	 */
	void deleteSelected();

	/**
	 *	Causes the data, displayed in the status bar at the bottom of the application window to be updated.
	 */
	void UpdateStatusBar();

	/**
	 *	Causes the data, displayed in the "Home"-category of the ribbon to be updated.
	 */
	void UpdateMainRibbonCategory();

	/**
	 *	Fills causes the "Display function" and "Detector function" dropwodn menus in the "Home"-category of the ribbon to be updated.
	 */
	void UpdateRibbonDropdowns();

	/**
	 *	Causes the ribbon bar to be updated, according the the type and data of the supplied CDrawingObject.
	 *
	 *	@param pObj Pointer to a CDrawingObject.
	 *	@param bAddToSelection Flag, which indicates if pObj was added to the current selection.
	 *
	 *	@remarks	This function call may result in displaying or hiding context categories in the ribbon.
	 *				If bAddToSelection is TRUE, this may cause additional context categories to be displayed,
	 *				if the object, supplied with pObj is the first of its kind.
	 */
	void UpdateRibbon(CDrawingObject* pObj, BOOL bAddToSelection, BOOL bRefreshData = FALSE);

	/**
	 *	Callback, invoked by CFlowIllustratorRenderView, to allow derived classes to perform additional render operations.
	 *	This overloaded verion is used to render m_pCreateDummy, and calls DrawSelection().
	 */
	virtual void OnDrawScene();

	/**
	 * Draws CTrackerRect objects around all selected CDrawingObjects.
	 */
	void DrawSelection();

	/**
	 *	Notifies this CFlowIllustratorView's parent window, that the selection of a CDrawingObject has changed.
	 */
	void NotifyDrawingObjsChanged();

public:
	//Used by CFlowIllustratorDoc
	void DocumentLoaded(void);
	void NewDocument(void);

	//Interface used by CMainFrame
	float GetArrowLength() const;
	float GetStreamlineStepSize() const;
	float GetVortexThreshold() const;
	int GetStreamlineLength() const;
	
	int GetDetectorFunctionID() const;
	void SetDetectorFunction(int nID);
	void GetDetectorFunctionNames(CStringArray &arrNames) const;

	void SetEditMode(EDIT_MODE nMode);
	void SetStreamlineLength(int nNumSteps);
	void SetStreamlineStepSize(float fStepSize);
	void SetVortexThreshold(float fThreshold);
	void SetArrowLength(float fArrowLen);
	void SetRenderStreaklineAsParticles(BOOL bAsParticles);
	BOOL GetRenderStreaklineAsParticles() const;
	
	void UpdateSelected(CDrawingObjectParams &params);
	void UpdateParams(CDrawingObject *pObj, const CDrawingObjectParams& params);

	void TranslateSelected(UINT direction, bool bMoveFast);
	void RotateSelected(UINT direction, bool bMoveFast);

	void MoveSelectexVortexToCore();
	void MoveSelected(int nDir);

	void ToggleAutoUpdateVortexTrajectory();
	BOOL GetAutoUpdateVortexTrajectory();

protected:
	void AddNewVortex(const CPointf& point);
	void AddNewRectangle(const CPointf& ptMouseDown, const CPointf& ptMouseUp);
	void AddNewEllipse(const CPointf& ptMouseDown, const CPointf& ptMouseUp);
	void AddNewTriangle(const CPointf& ptMouseDown, const CPointf& ptMouseUp);
	void AddNewLine(const CPointf& ptMouseDown, const CPointf& ptMouseUp);
	void AddNewStreamLine(const CPointf& point);
	void AddNewStreakLine(const CPointf& point);
	void AddNewPathLine(const CPointf& point);
	void AddNewTimeLine(const CPointf& ptSeedLineStart, const CPointf& ptSeedLineEnd);

	void CalcStreamlineBoundingBox(CStreamLine *pStreamLine);

	void ResetCreateDummy();

	void AdvanceStreamObjects();

private:
	bool _isVortVorticityMagnitude(const CAmiraVectorField2D*, const CPointf&);


// Implementation
public:
	virtual ~CFlowIllustratorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DWORD	m_dwPlay;

protected:
	void StartTimer();
	void StopTimer();

protected:
	/**
	 *	Performs stream line integration, starting at the current time step, at specified origin.
	 *	
	 *	@param point	Origin, from where stream line integration is started.
	 *	@param pStreamLine	Pointer to a CStreamLine to receive the vertices obtained by the integration.
	 *	@param bForward	If true, forward integration is performed, otherwise backward integration.
	 */
	void calcStreamLine(const CPointf& point, CStreamLine *pStreamLine, bool bForward = true);
	void calcPathLine(const CPointf& point, CPathLine *pPathLine) const;
	void calcStreakLine(const CPointf& point, CStreakLine *pStreakLine);
	void calcTimeLine( CTimeLine *pTimeLine);
	void calcCharacteristicLine(CDrawingObject* pObj, const CPointf &pOrigin);
	void calcVortexTrajectory(CVortexObj *pVortex) const;

	/**
	 *	This function calculates three speed lines, representing the trajectory of the specified CTimeLine object,
	 *	and attaches the to it.
	 *
	 *	@param pTimeLine Pointer to a CTimeLine object, whose trajectoreis are to be calculated.
	 *
	 *	@remarks	This function chooses three particles, evenly spaced on the seeding line of the specified CTimeLine object,
	 *				and performes a path line integration for each particle.
	 */
	void calcTimeLineTrajectory(CTimeLine *pTimeLine) const;
	BOOL measureVortex(CScalarField2D *pVortField, const CPointf &ptVortexCore, float &radius1, float &radius2, float &angle, float fThreshold);
	CPointf GetVortexCore(const CPointf& point);

public:
	void JumpToFirstFrame();
	void JumpToPreviousFrame();
	void TogglePlayback(DWORD dwDir);
	void JumpToNextFrame();
	void JumpToLastFrame();

	void JumpToFrame(int nFrame);

	BOOL IsPlaying();
	BOOL IsPlayingFwd();
	BOOL IsPlayingBckwd();

	void SetStreamlineColor(COLORREF nCol);
	void GrowStreakLines(int nNumNewParticles);

	void CopyFrameBuffer(BYTE **pBuffer, int &nWidth, int &nHeight) const;

// Generated message map functions
protected:
	//{{AFX_MSG(CFlowIllustratorView)
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnInitialUpdate();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg LRESULT OnTrackerRectChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrackerRectAdjustMousePos(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrackerRectEndTrack(WPARAM wParam, LPARAM lParam);

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();

	afx_msg LRESULT OnDocFrameChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSVGLoaded(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnViewUpdateSelected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectDrawingObject(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnDeleteSelected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMoveUp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMoveDown(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditSelectall();
};

