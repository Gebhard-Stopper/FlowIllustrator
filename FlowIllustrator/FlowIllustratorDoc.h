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
	CDrawingObjectMngr	 m_DrawObjMngr;	//Keep track of all objects added by the user
	CAmiraVectorField2D *m_pVectorField;
	BOOL	m_bDirty;
	BOOL	m_IsLoadingSVG;

// Operations
public:
	void PreviousFrame();
	void NextFrame();
	void LastFrame();
	void FirstFrame();
	void GotoFrame(unsigned int timeStep);
	BOOL isDirty() const { return m_bDirty; }
	void isDirty(BOOL bDirty) { m_bDirty = bDirty; }
	void ResetDirtyBit() { m_bDirty = FALSE; }
	
	__inline BOOL IsLoadingSVG() const { return m_IsLoadingSVG;	}
	__inline void IsLoadingSVG(BOOL bIsLoading) { m_IsLoadingSVG = bIsLoading; }
	__inline CDrawingObjectMngr* GetDrawingObjectMngr() { return &m_DrawObjMngr; }
	__inline const CAmiraVectorField2D* GetVectorfield() const { return m_pVectorField; }
	__inline const CAmiraVectorField2D* GetCurrentFrame() const { return m_pVectorField; }
	__inline const int GetCurrentFrameNo() const { return m_pVectorField->GetCurrentTimeStep(); }

public:
	void SaveSVG(LPCTSTR lpszPathName);
	void SavePNG(LPCTSTR lpszPathName);
	void SaveAmiraMesh(LPCTSTR lpszPathName, int nStartFrame, int nEndFrame, const CRectF &rcDomain);

	BOOL LoadSVG(const CString &strFileName);
	CView* GetActiveView() const;

protected:
	void Destroy();
	void _gotoTimeStep(unsigned int timeStep);
	CString GetSVGString();

	void parseStyleString(__in const CString& strSource, __out LPFLOATCOLOR pColor, __out BOOL &bSolid, __out float &fLineWidth);
	void parseTransformString(__in const CString& strSource, __out float &rotation);

	void _notifyParent(UINT nMsg, WPARAM wParam = NULL, LPARAM lParam = NULL);
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
