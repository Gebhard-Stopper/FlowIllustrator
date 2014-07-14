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

// SaveScreenshotSeriesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FlowIllustrator.h"
#include "SaveScreenshotSeriesDlg.h"
#include "afxdialogex.h"
#include "FolderDlg.h"

// CSaveScreenshotSeriesDlg dialog

IMPLEMENT_DYNAMIC(CSaveScreenshotSeriesDlg, CDialogEx)

CSaveScreenshotSeriesDlg::CSaveScreenshotSeriesDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSaveScreenshotSeriesDlg::IDD, pParent)
	, m_nNewParticles(0)
{
	m_strFileName = _T("");
	m_strFolderName = _T("");
	m_nStartFrame = 0;
	m_nEndFrame = 0;
}

CSaveScreenshotSeriesDlg::~CSaveScreenshotSeriesDlg()
{
}

void CSaveScreenshotSeriesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_EDIT_FOLDERNAME, m_strFolderName);
	DDX_Text(pDX, IDC_EDIT_STARTFRAME, m_nStartFrame);
	DDX_Text(pDX, IDC_EDIT_ENDFRAME, m_nEndFrame);
	DDX_Check(pDX, IDC_CHECK_GROW_STREAKLINE, m_bGrowStreakLine);
	DDX_Text(pDX, IDC_EDIT_NEWPARTICLES, m_nNewParticles);
	DDV_MinMaxInt(pDX, m_nNewParticles, 1, 10000);
}


BEGIN_MESSAGE_MAP(CSaveScreenshotSeriesDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SELECT_FOLDER, &CSaveScreenshotSeriesDlg::OnBtnSelectFolder)
	ON_BN_CLICKED(IDOK, &CSaveScreenshotSeriesDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSaveScreenshotSeriesDlg message handlers


void CSaveScreenshotSeriesDlg::OnBtnSelectFolder()
{
	CFolderDialog dlg(_T("Select base folder"));

	if (IDOK == dlg.DoModal())
	{
		m_strFolderName = dlg.GetFolderPath();
		UpdateData(FALSE);
	}
}


void CSaveScreenshotSeriesDlg::OnBnClickedOk()
{
	BOOL bError = FALSE;

	UpdateData();

	if (m_strFileName.IsEmpty()) {
		AfxMessageBox(_T("Please specify a file name."), MB_OK | MB_ICONEXCLAMATION);
		bError = TRUE;
	}

	if (m_strFolderName.IsEmpty()) {
		AfxMessageBox(_T("Please specify a folder name."), MB_OK | MB_ICONEXCLAMATION);
		bError = TRUE;
	}

	if (m_nEndFrame < m_nStartFrame) {
		AfxMessageBox(_T("End Frame must be greater or equal to Start Frame."), MB_OK | MB_ICONEXCLAMATION);
		bError = TRUE;
	}

	if (!bError) {
		CDialogEx::OnOK();
	}
}
