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

#pragma once


// CSaveScreenshotSeriesDlg dialog

class CSaveScreenshotSeriesDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSaveScreenshotSeriesDlg)

public:
	CSaveScreenshotSeriesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSaveScreenshotSeriesDlg();

// Dialog Data
	enum { IDD = IDD_SAVE_SCREENSHOT_SERIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CString		m_strFileName;
	CString		m_strFolderName;
	int			m_nStartFrame;
	int			m_nEndFrame;
	BOOL		m_bGrowStreakLine;

protected:
	afx_msg void OnBtnSelectFolder();
	afx_msg void OnBnClickedOk();
public:
	int m_nNewParticles;
};
