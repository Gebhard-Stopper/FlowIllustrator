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

// FlowIllustratorDoc.cpp : implementation of the CFlowIllustratorDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "FlowIllustrator.h"
#endif

#include "FlowIllustratorDoc.h"
#include "FlowIllustratorView.h"
#include "amirareader.h"
#include "SVGConverter.h"
#include "SaveScreenshotSeriesDlg.h"
#include "StatusDlg.h"
#include "SaveAmiraMeshDlg.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFlowIllustratorDoc

IMPLEMENT_DYNCREATE(CFlowIllustratorDoc, CDocument)

BEGIN_MESSAGE_MAP(CFlowIllustratorDoc, CDocument)
	ON_COMMAND(ID_FILE_SAVE_SCREENSHOT, &CFlowIllustratorDoc::OnFileSaveScreenshot)
	ON_COMMAND(ID_FILE_SAVE, &CFlowIllustratorDoc::OnFileSave)
	ON_COMMAND(ID_FILE_OPEN_SVG, &CFlowIllustratorDoc::OnFileOpenSVG)
	ON_COMMAND(ID_FILE_SAVE_SERIES, &CFlowIllustratorDoc::OnFileSaveScreenshotSeries)
END_MESSAGE_MAP()


// CFlowIllustratorDoc construction/destruction

CFlowIllustratorDoc::CFlowIllustratorDoc()
{
	m_pVectorField  = nullptr;
	m_bDirty = FALSE;
	m_IsLoadingSVG = FALSE;
}

CFlowIllustratorDoc::~CFlowIllustratorDoc()
{
	Destroy();
}

BOOL CFlowIllustratorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	m_DrawObjMngr.RemoveAll();

	CFlowIllustratorView* pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView)
	{
		pView->NewDocument();
	}   

	return TRUE;
}

void CFlowIllustratorDoc::Destroy()
{
	m_DrawObjMngr.RemoveAll();

	if (m_pVectorField)
	{
		delete m_pVectorField;
		m_pVectorField = nullptr;
	}
}


// CFlowIllustratorDoc serialization

void CFlowIllustratorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CFlowIllustratorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CFlowIllustratorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CFlowIllustratorDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CFlowIllustratorDoc diagnostics

#ifdef _DEBUG
void CFlowIllustratorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFlowIllustratorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CFlowIllustratorDoc commands

BOOL CFlowIllustratorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	Destroy();

	CStringA str (lpszPathName);
	m_pVectorField = new CAmiraVectorField2D();
	bool bSuccess = m_pVectorField->LoadAmiraFile(LPCSTR(str));

	if (bSuccess)
	{
		CFlowIllustratorView* pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
		if (pView)
		{
			pView->DocumentLoaded();
		}   

		m_bDirty = TRUE;
	}

	return bSuccess;
}


void CFlowIllustratorDoc::NextFrame()
{
	if (m_pVectorField)
	{
		unsigned int currTimeStep = m_pVectorField->GetCurrentTimeStep() + 1;
		if (currTimeStep >= m_pVectorField->GetNumTimeSteps() )
		{
			currTimeStep = m_pVectorField->GetNumTimeSteps()-1;
		}

		_gotoTimeStep(currTimeStep);
	}
}

void CFlowIllustratorDoc::PreviousFrame()
{
	if (m_pVectorField)
	{
		unsigned int currTimeStep = m_pVectorField->GetCurrentTimeStep() - 1;
		if (currTimeStep < 0 )
		{
			currTimeStep = 0;
		}

		_gotoTimeStep(currTimeStep);
	}
}

void CFlowIllustratorDoc::LastFrame()
{
	if (m_pVectorField)
	{
		unsigned int currTimeStep = m_pVectorField->GetNumTimeSteps() - 1;

		_gotoTimeStep(currTimeStep);
	}
}

void CFlowIllustratorDoc::FirstFrame()
{
	if (m_pVectorField)
	{
		_gotoTimeStep(0);
	}
}

void CFlowIllustratorDoc::GotoFrame(unsigned int timeStep)
{
	if (m_pVectorField)
	{
		if (timeStep >= 0 && timeStep < m_pVectorField->GetNumTimeSteps())
		{	
			_gotoTimeStep(timeStep);
		}
	}
}

void CFlowIllustratorDoc::_gotoTimeStep(unsigned int timeStep)
{
	//Ensure that the part of the file is mapped to memory here
	//m_AmiraReader.requireTimestep(timeStep, &m_pVectorField);

	m_bDirty = TRUE;
	m_pVectorField->GotoTimeStep(timeStep);

	_notifyParent(WM_DOC_FRAME_CHANGED);
}

CView* CFlowIllustratorDoc::GetActiveView() const
{
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	if (pFrame) {
		return pFrame->GetActiveView();
	}

	return nullptr;
}

void CFlowIllustratorDoc::SaveSVG(LPCTSTR lpszPathName)
{
	CStdioFile file(lpszPathName, CFile::modeWrite | CFile::modeCreate);
	file.WriteString(GetSVGString());
	file.Flush();
	file.Close();
}

void CFlowIllustratorDoc::SaveAmiraMesh(LPCTSTR lpszPathName, int nStartFrame, int nEndFrame, const CRectF &rcDomain)
{
	CStringA strFileName(lpszPathName), strHead, strDummy;
	FILE *pFile;
	fopen_s(&pFile, strFileName, "wb");
	if (pFile)
	{
		int nNumTimeSteps = nEndFrame - nStartFrame;
		CPointf ptMin = m_pVectorField->GetClosestSamplePos( CPointf::fromVector2D(rcDomain.m_Min) );
		CPointf ptMax = m_pVectorField->GetClosestSamplePos( CPointf::fromVector2D(rcDomain.m_Max) );

		CRectF realDomain;

		realDomain.m_Min = CPointf::toVector2D( m_pVectorField->GetDomainCoordinates(ptMin.x, ptMin.y) );
		realDomain.m_Max = CPointf::toVector2D( m_pVectorField->GetDomainCoordinates(ptMax.x, ptMax.y) );

		int nWidth =  static_cast<int>( ptMax.x - ptMin.x );
		int nHeight =  static_cast<int>( ptMax.y - ptMin.y );

		strHead = "# AmiraMesh BINARY-LITTLE-ENDIAN 2.1\n\n";
		strDummy.Format(	"define Lattice %d %d %d\n\n", 
							nWidth, 
							nHeight, 
							nNumTimeSteps);
		strHead += strDummy;

		strDummy.Format(	"Parameters {\n\tBoundingBox %f %f %f %f %f %f\n\tCoordType \"uniform\"\n}\n\n",
							realDomain.m_Min.x, 
							realDomain.m_Max.x, 
							realDomain.m_Min.y, 
							realDomain.m_Max.y, 
							nStartFrame / m_pVectorField->GetSamplesPerSecond(), 
							nEndFrame / m_pVectorField->GetSamplesPerSecond());
		strHead += strDummy;

		strHead += "Lattice { float[2] Data } @1\n\n";
		strHead += "# Data section follows\n";
		strHead += "@1\n";

		fwrite(strHead, sizeof(char), strHead.GetLength(), pFile);

		for (int i=nStartFrame; i < nEndFrame; ++i)
		{
			CVector2D *pFrame = m_pVectorField->GetFrame(i);

			pFrame += static_cast<int>(ptMin.y) * m_pVectorField->GetExtentX();	//go to correct row
			pFrame += static_cast<int>(ptMin.x); //goto correct column

			for (int j=0; j<nHeight; ++j)
			{
				fwrite( pFrame, sizeof(CVector2D), nWidth, pFile);
				pFrame += m_pVectorField->GetExtentX();	//go to next row
			}
		}

		fclose(pFile);
	}
}

void CFlowIllustratorDoc::SavePNG(LPCTSTR lpszPathName)
{
	if (!m_pVectorField) return;

	CImage img;

	CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
	if (pView) {
		pView->RedrawWindow();

		BYTE *pPixelBuf = nullptr;
		int nWidth = m_pVectorField->GetExtentX()*2;
		int nHeight = m_pVectorField->GetExtentY()*2;

		pView->CopyFrameBuffer(&pPixelBuf, nWidth, nHeight);

		CBitmap bmp;
		bmp.CreateBitmap(nWidth, nHeight, 1, 32, pPixelBuf);
		img.Attach(bmp, CImage::DIBOR_BOTTOMUP);
		
		img.Save(lpszPathName);

		delete [] pPixelBuf;
	}
}

void CFlowIllustratorDoc::OnFileSaveScreenshot()
{
	CString strFileName;

	BOOL bResult = DislpayFileDlg(	_T("Portable Network Graphics (*.png)|*.png||"),
									_T("png"),
									strFileName);

	if (bResult)
	{
		SavePNG(strFileName);
	}
}


void CFlowIllustratorDoc::OnFileSave()
{
	CString strFileName;

	BOOL bResult = DislpayFileDlg(	_T("Scalable Vector Graphics (*.svg)|*.svg| Amira mesh (*.am)|*.am||"),
									_T("svg"),
									strFileName);

	if (bResult)
	{
		if (strFileName.Right(3).MakeLower() == _T(".am")) {
			if (!m_pVectorField) {
				AfxMessageBox(_T("Please open a mesh file first"));
				return;
			}

			CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
			ASSERT(pView);
			
			if (pView) {
				CSaveAmiraMeshDlg dlg( pView->GetViewportRect() );
				if ( IDOK == dlg.DoModal() ) {
					CRectF rcDomain = dlg.UseEntireDomain()? m_pVectorField->GetDomainRect() : dlg.GetDomainRect();
					SaveAmiraMesh( strFileName, dlg.m_nStartFrame, dlg.m_nEndFrame, rcDomain);
				}
			}

		} else {
			SaveSVG(strFileName);
		}
	}
}


void CFlowIllustratorDoc::OnFileOpenSVG()
{
	CString strFileName;

	BOOL bResult = DislpayFileDlg(	_T("Scalable Vector Graphics (*.svg)|*.svg||"),
									_T("svg"),
									strFileName,
									TRUE);

	if (bResult)
	{
		if (LoadSVG(strFileName)) {
			_notifyParent(WM_SVG_LOADED);
		}
	}
}

void CFlowIllustratorDoc::OnFileSaveScreenshotSeries()
{
	CSaveScreenshotSeriesDlg dlg;

	if (IDOK == dlg.DoModal())
	{
		CString strFileName, strFolderName;
		AfxExtractSubString(strFileName, dlg.m_strFileName, 0, _T('.'));

		strFolderName = dlg.m_strFolderName;
		if ( strFolderName.Right(1) != _T("\\") ) {
			strFolderName += _T("\\");
		}

		CFlowIllustratorView *pView = reinterpret_cast<CFlowIllustratorView*>(GetActiveView());
		if (pView) {
			int nCurrFrame = m_pVectorField->GetCurrentTimeStep();
			CString strOutName;

			CStatusDlg statusDlg;
			statusDlg.Create(IDD_DIALOG_STATUS, pView);
			statusDlg.ShowWindow(SW_SHOW);

			statusDlg.m_wndProgressBar.SetRange(dlg.m_nStartFrame, dlg.m_nEndFrame);

			for (int i= dlg.m_nStartFrame; i < dlg.m_nEndFrame; i++)
			{
				statusDlg.m_wndProgressBar.SetPos(i);
				strOutName.Format(_T("%s%s_%d.png"), strFolderName, strFileName, i);
				if (!dlg.m_bGrowStreakLine) {
					pView->JumpToFrame(i);
				} else {
					pView->GrowStreakLines(dlg.m_nNewParticles);
				}
				pView->DrawScene();
				SavePNG(strOutName);
			}

			//reset the amount of particles per streakline
			if (dlg.m_bGrowStreakLine) {
				pView->GrowStreakLines(dlg.m_nNewParticles * (dlg.m_nEndFrame-dlg.m_nStartFrame));
			}

			GotoFrame(nCurrFrame);
			statusDlg.DestroyWindow();
		}
	}
}

BOOL CFlowIllustratorDoc::DislpayFileDlg(const CString &strFilter, const CString &strFormat, CString &strFileName, BOOL bOpen) const
{
	CString strTitle (GetTitle());
	CString strDummy;

	AfxExtractSubString(strDummy, strTitle, 0, _T('.'));

	CFileDialog dlg(bOpen, strFormat, strDummy, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter, NULL, 0, TRUE);
	
	INT_PTR bResult = dlg.DoModal();

	if (bResult == IDOK)
	{
		strFileName = dlg.GetPathName();
	}

	return (bResult == IDOK);
}

BOOL CFlowIllustratorDoc::LoadSVG(const CString &strFileName)
{
	m_DrawObjMngr.RemoveAll();
	return CSVGConverter::LoadSVG(strFileName, *this);
}

CString CFlowIllustratorDoc::GetSVGString()
{
	return CSVGConverter::GetSVGString(*this);
}

void CFlowIllustratorDoc::_notifyParent(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CView *pView = GetActiveView();
	if (pView) {
		pView->SendMessage(nMsg, wParam, lParam);
	}
}