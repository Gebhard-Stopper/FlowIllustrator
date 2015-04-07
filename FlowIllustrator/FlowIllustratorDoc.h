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

// FlowIllustratorDoc.h : interface of the CFlowIllustratorDoc class
//


#pragma once

#include "AmiraVectorField2D.h"
#include "Markup.h"
#include "DrawingObjectMngr.h"

using namespace FICore;

const UINT WM_SAVE_SCREENSHOT = RegisterWindowMessage(_T("SAVE_SCREENSHOT"));
const UINT WM_SVG_LOADED = RegisterWindowMessage(_T("WM_SVG_LOADED"));
const UINT WM_DOC_FRAME_CHANGED = RegisterWindowMessage(_T("WM_DOC_FRAME_CHANGED"));

class CFlowIllustratorDoc : public CDocument
{
protected: // create from serialization only
	CFlowIllustratorDoc();
	DECLARE_DYNCREATE(CFlowIllustratorDoc)

// Attributes
private:
	CDrawingObjectMngr	 m_DrawObjMngr;		/**< The drawing object manager keeps track of all CDrawingObject derived classes. The order of objects stored in the drawing object manager corresponds to the drawing order.*/
	CAmiraVectorField2D *m_pVectorField;	/**< Pointer to the currently opened vector field.*/
	BOOL	m_bDirty;						/**< The dirty-flag indicates that something in the opened vector field has changed, and it needs to be rendered. */
	BOOL	m_IsLoadingSVG;					/**< Indicates that currently an SVG file is being loaded. */

// Operations
public:
	/**
	 *	Moves the internal frame pointer of m_pVectorField to the beginning of the previous frame (time-step).
	 *	If the current time step is already the first time step available in the current vector field, nothing happens.
	 */
	void PreviousFrame();

	/**
	 *	Moves the internal frame pointer of m_pVectorField to the beginning of the next frame (time-step).
	 *	If the current time step is already the last time step available in the current vector field, nothing happens.
	 */
	void NextFrame();

	/**
	 *	Moves the internal frame pointer of m_pVectorField to the beginning of the last frame (time-step).
	 *	If the current time step is already the last time step available in the current vector field, nothing happens.
	 */
	void LastFrame();

	/**
	 *	Moves the internal frame pointer of m_pVectorField to the beginning of the first frame (time-step).
	 *	If the current time step is already the first time step available in the current vector field, nothing happens.
	 */
	void FirstFrame();

	/**
	 *	Moves the internal frame pointer of m_pVectorField to the beginning of the specified frame (time-step).
	 *	If the specified time step is < 0 or > MAX_FRAME, nothing happens.
	 *
	 *	@param timeStep The time-step to bve made current.
	 */
	void GotoFrame(unsigned int timeStep);

	/**
	 *	Retrieve, if the current vector field is makred dirty.
	 *
	 *	@return The current status of the dirty-flag.
	 *
	 *	@remarks IF this function returns TRUE, the vector field ha changed and needs to be rendered.
	 */
	BOOL isDirty() const { return m_bDirty; }

	/**
	 *	Modify the dirty-flag.
	 *
	 *	@param bDirty The new status of the dirty-flag.
	 */
	void isDirty(BOOL bDirty) { m_bDirty = bDirty; }

	/**
	 *	Reset the dirty-flag to FALSE.
	 */
	void ResetDirtyBit() { m_bDirty = FALSE; }
	
	/**
	 *	Retrieve the status, if currently an SVG file is being loaded.
	 *
	 *	@reutrn Returns TRUE, if curretnly an SVG file is being loaded, otherwise FALSE.
	 */
	__inline BOOL IsLoadingSVG() const { return m_IsLoadingSVG;	}

	/**
	 *	Set the status, if currently an SVG file is being loaded.
	 *
	 *	@param bIsLoading CSVGConverter sets this flag to TRUE, if curretnly an SVG file is being loaded, otherwise FALSE.
	 */
	__inline void IsLoadingSVG(BOOL bIsLoading) { m_IsLoadingSVG = bIsLoading; }

	/**
	 *	Retrieve a pointer to the CDrawingObjectMngr.
	 *
	 *	@return A pointer to this CFlowIllustratorDoc's CDrawingObjectMngr.
	 */
	__inline CDrawingObjectMngr* GetDrawingObjectMngr() { return &m_DrawObjMngr; }

	/**
	 *	Retrieve a pointer to this CFlowIllustratorDoc's currently opened vector field.
	 *
	 *	@return A pointer to the currently opened CAmiraVectorField2D.
	 *
	 *	@remarks If the returned pointer is NULL, no vector field is currently opened.
	 */
	__inline const CAmiraVectorField2D* GetVectorfield() const { return m_pVectorField; }

	/**
	 *	Retrieve the current frame number of the opened vector field.
	 *
	 *	@return The frame number as int.
	 *
	 *	@remarks The frame number is zero-based, i.e. The first frame has frame number 0, and the last of n frames has the frame number n-1.
	 */
	__inline const int GetCurrentFrameNo() const { return m_pVectorField->GetCurrentTimeStep(); }

	/**
	 *	Saves all CDrawingObjects, currently managed by this document's CDrawingObjectManager to the SVG file at the specified location.
	 *
	 *	@param lpszPathName The file name of the SVG file to be saved.
	 */
	void SaveSVG(LPCTSTR lpszPathName);

	/**
	 *	Saves a screenshot of the currently rendered scene in PNG format at the specified location.
	 *
	 *	@param lpszPathName The file name of the PNG file to be saved.
	 *
	 *	@remarks Everything currently renderd in the workspace is saved to the PNG file. This also includes the currently selected flow visualization.
	 */
	void SavePNG(LPCTSTR lpszPathName);

	/**
	 *	Save a copy of the corrently opened amira mesh file.
	 *
	 *	@param lpszPathName The name of hte target file. Must be differnt form the currently opened file.
	 *	@param nStartFrame	The First frame of the currently opened file that is to be copied to the new amira mesh file.
	 *	@param nEndFrame	The last frame to be copied to the new amira mesh file. Must be >= nStartFrame.
	 *	@param rcDomain		Reference to a CRectF, specifying the (sub) domain to be copied to the output file.
	 *
	 *	@remarks	This function copies all frames >= nStartFrame and < nEndFrame to a new amira mesh file.
	 *				The rectangular region specified in rcDomain must not be larger then the original domain.
	 *				If the specified output domain is smaller than the original domain, is is ensured the actual output 
	 *				domain is at least of the specified size. The output domain might be slightly larger, though. This is because,
	 *				This function converts the given domain coordinates in to grid coordinates and then adjusts them to the nearest
	 *				sample locations that guarantee to include the specified domain.
	 *				
	 */
	void SaveAmiraMesh(LPCTSTR lpszPathName, int nStartFrame, int nEndFrame, const CRectF &rcDomain);

	/**
	 *	Load an SVG file and convert the contained data into their respective CDrawingObejct derived classes.
	 *
	 *	@param strFileName Name of the SVG fiel to be opened.
	 */
	BOOL LoadSVG(const CString &strFileName);

	/**
	 *	Retrieve a pointer to the currently active CView of this application.
	 *
	 *	@return A pointer to the active CView. This is typically an object of type CFlowIllustratorView.
	 */
	CView* GetActiveView() const;


// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CFlowIllustratorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL DislpayFileDlg(const CString &strFilter, const CString &strFormat, CString &strFileName, BOOL bOpen = FALSE) const;
	void Destroy();
	void _gotoTimeStep(unsigned int timeStep);
	CString GetSVGString();

	void parseStyleString(__in const CString& strSource, __out LPFLOATCOLOR pColor, __out BOOL &bSolid, __out float &fLineWidth);
	void parseTransformString(__in const CString& strSource, __out float &rotation);

	void _notifyParent(UINT nMsg, WPARAM wParam = NULL, LPARAM lParam = NULL);

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg void OnFileSaveScreenshot();
	afx_msg void OnFileSave();
	afx_msg void OnFileOpenSVG();
	afx_msg void OnFileSaveScreenshotSeries();
};
