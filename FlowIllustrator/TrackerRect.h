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
#include "afxext.h"
#include "Rectangle.h"

const UINT WM_TRACKRECT_CHANGED = RegisterWindowMessage(_T("TRACKRECT_CHANGED_RECT"));
const UINT WM_TRACKRECT_ADJUST_MOUSE_POS = RegisterWindowMessage(_T("TRACKRECT_ADJUST_MOUSE_POS"));
const UINT WM_TRACKRECT_END_TRACK = RegisterWindowMessage(_T("TRACKRECT_END_TRAC"));

typedef struct TRACK_RECT_UPDATE_S
{
	CRectF		rect;
	CRectF		oldRect;
	CPointf		ptMouseMove;
	float		angle;
	float		oldAngle;
	bool		bRotate;

}trackRectUpdate, *LPTRACKRECTUPDATE;

class CTrackerRect
{
public:
	enum TR_HandleID 
	{	NONE = -1, 
		BOTTOM_LEFT = 0, BOTTOM_RIGHT = 1, TOP_RIGHT = 2,  TOP_LEFT = 3, 
		BOTTOM = 4, RIGHT = 5, TOP = 6, LEFT = 7, CENTER = 8
	};

public:
	CTrackerRect( CWnd* pOwner, const CRectF& rect);
	virtual ~CTrackerRect(void);

protected:
	CRectF	m_rect;
	//float	m_fHandleSize;
	CWnd*	m_pOwner;
	float	m_fRotation;

	int		m_nHighlightId;

protected:
	void GetTrueRect(CRectF &rect) const;
	void GetHandleRect(int nHandleID, CRectF *pRect) const;
	UINT GetHandleMask() const;
	void DrawRect(const CRectF &rect, BOOL bSolid) const;
	void DrawHandleRect(const CRectF &rect, BOOL bSolid) const;
	int HandleHitTest(const CPointf& point) const;
	BOOL TrackHandle(int nHandle, CWnd* pWnd, CPointf point, UINT nFlags);
	void GetModifyPointers(int nHandle, float**ppx, float**ppy);
	CPointf _getRealPoint(const CPointf &point, bool bInvert = true) const;
	void adjustMousePos(CPointf &point);

public:
	int HitTest(const CPointf& point) const;
	CRectF GetRect() const;
	void Draw() const;
	BOOL Track(CWnd* pWnd, CPointf point, UINT nFlags);

	void Highlight(int nHandleID) {
		m_nHighlightId = nHandleID;
	}

	float GetRotation() const;
	void SetRotation(float nDegree);
	void SetRect(const CRectF &rect);

public:
	virtual void OnChangedRect (const CRectF& rectOld, const CPointf &ptMousePos,  const CPointf &ptMousePosOld,const float oldRotation, bool bRotate);
	void OnEndTrack();

private:
	BOOL _notifyParent(UINT nMsg, WPARAM wParam = NULL, LPARAM lParam = NULL);
};

