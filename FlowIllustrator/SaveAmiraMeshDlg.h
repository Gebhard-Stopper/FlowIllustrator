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
#include "afxwin.h"
#include "RectF.h"

// CSaveAmiraMeshDlg dialog

class CSaveAmiraMeshDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSaveAmiraMeshDlg)

public:
	CSaveAmiraMeshDlg(const CRectF &rect, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSaveAmiraMeshDlg();

// Dialog Data
	enum { IDD = IDD_SAVE_AMIRA_MESH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	BOOL m_bUseEntireDomain;

protected:
	void ActivateControls();

public:
	UINT m_nStartFrame;
	UINT m_nEndFrame;
	float m_fXMin;
	float m_fYMin;
	float m_fXMax;
	float m_fYMax;


public:
	afx_msg void OnBnClickedCheckEntireDomain();

	virtual BOOL OnInitDialog();

	__inline CRectF GetDomainRect() const {
		return CRectF(m_fXMin, m_fYMin, m_fXMax, m_fYMax);
	}

	__inline BOOL UseEntireDomain() const {
		return m_bUseEntireDomain;
	}
};
