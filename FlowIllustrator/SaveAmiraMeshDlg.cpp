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

// SaveAmiraMeshDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FlowIllustrator.h"
#include "SaveAmiraMeshDlg.h"
#include "afxdialogex.h"


// CSaveAmiraMeshDlg dialog

IMPLEMENT_DYNAMIC(CSaveAmiraMeshDlg, CDialogEx)

CSaveAmiraMeshDlg::CSaveAmiraMeshDlg(const CRectF &rect, CWnd* pParent /*=NULL*/)
	: CDialogEx(CSaveAmiraMeshDlg::IDD, pParent)
	, m_nStartFrame(0)
	, m_nEndFrame(0)
	, m_bUseEntireDomain(FALSE)
	, m_fXMin(rect.m_Min.x), m_fYMin(rect.m_Min.y), m_fXMax(rect.m_Max.x), m_fYMax(rect.m_Max.y)
{

}

CSaveAmiraMeshDlg::~CSaveAmiraMeshDlg()
{
}

void CSaveAmiraMeshDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FRAME_FROM, m_nStartFrame);
	DDX_Text(pDX, IDC_EDIT_FRAME_TO, m_nEndFrame);
	DDX_Text(pDX, IDC_EDIT_XMIN, m_fXMin);
	DDX_Text(pDX, IDC_EDIT_YMIN, m_fYMin);
	DDX_Text(pDX, IDC_EDIT_XMAX, m_fXMax);
	DDX_Text(pDX, IDC_EDIT_YMAX, m_fYMax);
}


BEGIN_MESSAGE_MAP(CSaveAmiraMeshDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_ENTIRE_DOMAIN, &CSaveAmiraMeshDlg::OnBnClickedCheckEntireDomain)
END_MESSAGE_MAP()


// CSaveAmiraMeshDlg message handlers

void CSaveAmiraMeshDlg::OnBnClickedCheckEntireDomain()
{
	m_bUseEntireDomain = !m_bUseEntireDomain;

	ActivateControls();
}


BOOL CSaveAmiraMeshDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ActivateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSaveAmiraMeshDlg::ActivateControls()
{
	GetDlgItem(IDC_EDIT_XMIN)->EnableWindow(!m_bUseEntireDomain);
	GetDlgItem(IDC_EDIT_YMIN)->EnableWindow(!m_bUseEntireDomain);
	GetDlgItem(IDC_EDIT_XMAX)->EnableWindow(!m_bUseEntireDomain);
	GetDlgItem(IDC_EDIT_YMAX)->EnableWindow(!m_bUseEntireDomain);
}
