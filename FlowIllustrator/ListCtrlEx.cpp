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

// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "FlowIllustrator.h"
#include "ListCtrlEx.h"


// CListCtrlEx

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

CListCtrlEx::CListCtrlEx()
{

}

CListCtrlEx::~CListCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()



// CListCtrlEx message handlers


BOOL CListCtrlEx::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle |= LVS_OWNERDRAWFIXED;

	return CListCtrl::Create(dwStyle, rect, pParentWnd, nID);
}

void CListCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	CRect ItemRect (lpDrawItemStruct->rcItem);
	ItemRect.left += 1;
	ItemRect.top += 1;
	ItemRect.bottom -= 1;

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		CBrush brush( RGB(0, 148, 255) );
		CBrush *pOldBrush = dc.SelectObject(&brush);
		dc.Rectangle(ItemRect);
		dc.SelectObject(pOldBrush);
	}

	CRect r( ItemRect );
	r.right = r.left+r.Height() -1;

	CBrush brush( static_cast<COLORREF>(lpDrawItemStruct->itemData) );
	CBrush *pOldBrush = dc.SelectObject(&brush);

	dc.Rectangle(r);

	dc.SelectObject(pOldBrush);

	ItemRect.left += r.Width() + 4;
	dc.DrawText(GetItemText(lpDrawItemStruct->itemID,0), &ItemRect, DT_LEFT | DT_WORD_ELLIPSIS | DT_VCENTER | DT_SINGLELINE);

	dc.Detach();
}
