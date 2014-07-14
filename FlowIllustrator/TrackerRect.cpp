#include "StdAfx.h"
#include "TrackerRect.h"


static floatColor Color(0.0f,0.0f,0.0f,1.0f);
#define HANDLE_SIZE 8.0f


CTrackerRect::CTrackerRect(CWnd* pOwner, const CRectF& rect)
	: m_rect(rect), m_pOwner(pOwner)
{
	//m_fHandleSize = 8 *gPixelPerUnit;
	SetRotation(0);
	m_nHighlightId = -1;
}

CTrackerRect::~CTrackerRect(void)
{
}

CRectF CTrackerRect::GetRect() const
{
	return m_rect;
}

void CTrackerRect::OnChangedRect (const CRectF& rectOld, const CPointf &ptMousePos, const CPointf& ptMousePosOld, const float oldRotation, bool bRotate)
{
	trackRectUpdate tu;
	tu.angle			= m_fRotation;
	tu.oldAngle			= oldRotation;
	tu.rect				= m_rect;
	tu.oldRect			= rectOld;
	tu.ptMouseMove		= CPointf(ptMousePos.x - ptMousePosOld.x, ptMousePos.y - ptMousePosOld.y);
	tu.bRotate			= bRotate;

	_notifyParent(WM_TRACKRECT_CHANGED, reinterpret_cast<WPARAM>(&tu));
}

void CTrackerRect::adjustMousePos(CPointf &point)
{
	_notifyParent(WM_TRACKRECT_ADJUST_MOUSE_POS, reinterpret_cast<WPARAM>(&point));
}

void CTrackerRect::OnEndTrack()
{
	trackRectUpdate tu;
	tu.angle			= m_fRotation;
	tu.rect				= m_rect;

	_notifyParent(WM_TRACKRECT_END_TRACK, reinterpret_cast<WPARAM>(&tu));

	if ( !tu.rect.EqualRect(m_rect)) {
		m_rect = tu.rect;
	}
}

BOOL CTrackerRect::_notifyParent(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pOwner) 
	{
		m_pOwner->SendMessage(nMsg, wParam, lParam);
		return TRUE;
	}
	return FALSE;
}

void CTrackerRect::Draw() const
{
	CRectF rect(m_rect);
	rect.Normalize();

	CVector2D center = rect.GetCenter();

	glLineWidth(1);
	glColor3f(0.0f,0.0f,0.0f);

	Color.red = 0;

	glLineStipple(1, 0xAAAA); 
	glEnable(GL_LINE_STIPPLE);
	DrawRect(rect, FALSE);
	glDisable(GL_LINE_STIPPLE);


	// draw resize handles
	UINT mask = GetHandleMask();
	for (int i = 0; i < 8; ++i)
	{
		if (i==m_nHighlightId) {
			Color.red = 1;
		} else {
			Color.red = 0;
		} 
		if (mask & (1<<i))
		{
			GetHandleRect(i, &rect);
			DrawHandleRect(rect, TRUE);
		}
	}
}

UINT CTrackerRect::GetHandleMask() const
{
	UINT mask = 0x0F;   // always have 4 corner handles
	float fHandleSize = HANDLE_SIZE*(gPixelPerUnit/gZoomFactor);

	int size = static_cast<int>(fHandleSize*3.0f);
	if (abs(m_rect.getWidth()) - size > (4.0f*size))
		mask |= 0x50;
	if (abs(m_rect.getWidth()) - size > (4.0f*size))
		mask |= 0xA0;
	return mask;
}

void CTrackerRect::GetHandleRect(int nHandleID, CRectF *pRect) const
{
	CRectF dummy;
	float fHandleSize = HANDLE_SIZE*(gPixelPerUnit/gZoomFactor);

	switch(nHandleID)
	{
		case 0: //Bottom left
			dummy.m_Max.y	= m_rect.m_Min.y;
			dummy.m_Min.y	= dummy.m_Max.y - fHandleSize;
			dummy.m_Max.x	= m_rect.m_Min.x;
			dummy.m_Min.x	= dummy.m_Max.x - fHandleSize;
			break;
		case 1: // Bottom right
			dummy.m_Max.y	= m_rect.m_Min.y;
			dummy.m_Min.y	= dummy.m_Max.y - fHandleSize;
			dummy.m_Min.x	= m_rect.m_Max.x;
			dummy.m_Max.x	= dummy.m_Min.x + fHandleSize;
			break;
		case 2: //Top right
			dummy.m_Min.y	= m_rect.m_Max.y;
			dummy.m_Max.y	= dummy.m_Min.y + fHandleSize;
			dummy.m_Min.x	= m_rect.m_Max.x;
			dummy.m_Max.x	= dummy.m_Min.x + fHandleSize;
			break;
		case 3: //Top left
			dummy.m_Min.y	= m_rect.m_Max.y;
			dummy.m_Max.y	= dummy.m_Min.y  + fHandleSize;
			dummy.m_Max.x	= m_rect.m_Min.x;
			dummy.m_Min.x	= dummy.m_Max.x - fHandleSize;
			break;
		case 4: //bottom
			dummy.m_Max.y	= m_rect.m_Min.y;
			dummy.m_Min.y	= dummy.m_Max.y  - fHandleSize;
			dummy.m_Min.x	= m_rect.m_Min.x + ( (m_rect.getWidth()/2.0) - fHandleSize/2.0);
			dummy.m_Max.x	= dummy.m_Min.x + fHandleSize;
			break;
		case 5://right
			dummy.m_Min.x	= m_rect.m_Max.x;
			dummy.m_Max.x	= dummy.m_Min.x + fHandleSize;
			dummy.m_Min.y	= m_rect.m_Min.y + ( (m_rect.getHeight()/2.0) - fHandleSize/2.0);
			dummy.m_Max.y	= dummy.m_Min.y + fHandleSize;
			dummy.Normalize();
			break;
		case 6: //top
			dummy.m_Min.y	= m_rect.m_Max.y;
			dummy.m_Max.y	= dummy.m_Min.y + fHandleSize;
			dummy.m_Min.x	= m_rect.m_Min.x + ( (m_rect.getWidth()/2.0) - fHandleSize/2.0);
			dummy.m_Max.x	= dummy.m_Min.x + fHandleSize;
			break;
		case 7: //left
			dummy.m_Max.x	= m_rect.m_Min.x;
			dummy.m_Min.x	= dummy.m_Max.x - fHandleSize;
			dummy.m_Min.y	= m_rect.m_Min.y + ( (m_rect.getHeight()/2.0) - fHandleSize/2.0);
			dummy.m_Max.y	= dummy.m_Min.y + fHandleSize;
			dummy.Normalize();
			break;
		case 8:
			dummy			= m_rect;
			break;
	}

	*pRect = dummy;
}

void CTrackerRect::DrawRect(const CRectF &rect, BOOL bSolid) const
{
	CRectangle dummy (rect, Color, bSolid);
	dummy.SetRotation(m_fRotation);
	dummy.Draw();
}

void CTrackerRect::DrawHandleRect(const CRectF &rect, BOOL bSolid) const
{
	CRectF r(m_rect);

	CRectangle dummy (rect, Color, bSolid?true:false);
	dummy.SetRotation(m_fRotation);
	dummy.SetRotationOrigin(r.GetCenter());
	dummy.Draw();
}

int CTrackerRect::HitTest(const CPointf& point) const
{
	CRectF trueRect;
	GetTrueRect(trueRect);

	CPointf pt = _getRealPoint(point);

	if (trueRect.PtInRect(pt)) {
		//Somewhere inside the true rect
		return HandleHitTest(pt);
	}

	return NONE; //Definetely outside
}

CPointf CTrackerRect::_getRealPoint(const CPointf &point, bool bInvert) const
{
	CVector2D v( CPointf::toVector2D(point) );
	CVector2D center( m_rect.GetCenter() );

	v -= center;	//Point on the tracker rect, relative to its center

	float rad = DegToRad(m_fRotation);

	if (bInvert)
		v.Rotate(-rad);
	else
		v.Rotate(rad);

	v += center;

	return CPointf( (v.x), (v.y) );
}

int CTrackerRect::HandleHitTest(const CPointf& point) const
{
	UINT mask = GetHandleMask();
	CRectF rect;

	for (int i = 0; i < 8; i++) 
	{
		if (mask & (1<<i))
		{
			GetHandleRect(i, &rect);
			if (rect.PtInRect(point))
				return i;
		}
	}

	if (m_rect.PtInRect(point)) return CENTER;

	return NONE; //click between handles, but outside
}

void CTrackerRect::GetTrueRect(CRectF &rect) const
{
	rect = m_rect;
	float fHandleSize = HANDLE_SIZE*(gPixelPerUnit/gZoomFactor);

	rect.InflateRect(fHandleSize, fHandleSize);
}

BOOL CTrackerRect::Track(CWnd* pWnd, CPointf point, UINT nFlags)
{
	CPointf pt = _getRealPoint(point);

	int nHandle = HandleHitTest(pt);
	if (nHandle < 0)
	{
		// didn't hit a handle, so just return FALSE
		return FALSE;
	}

	// otherwise, call helper function to do the tracking
	return TrackHandle(nHandle, pWnd, pt, nFlags);
}

BOOL CTrackerRect::TrackHandle(int nHandle, CWnd* pWnd, CPointf point, UINT nFlags)
{
	ASSERT(nHandle >= 0);
	ASSERT(nHandle <= 8);   // handle 8 is inside the rect

	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return FALSE;

	AfxLockTempMaps();  // protect maps while looping

	BOOL bFinalErase = FALSE;

	ASSERT(!bFinalErase);

	// save original width & height in pixels
	float nWidth = m_rect.getWidth();
	float nHeight = m_rect.getHeight();

	// set capture to the window which received this message
	pWnd->SetCapture();
	ASSERT(pWnd == CWnd::GetCapture());
	pWnd->UpdateWindow();
	
	CRectF rectSave = m_rect;

	// find out what x/y coords we are supposed to modify
	float *px, *py;

	GetModifyPointers(nHandle, &px, &py);

	CVector2D diff;


	if (nHandle == CENTER)
	{
		point = _getRealPoint(point, false);
	}

	if (px)
		diff.x = point.x - (*px);
	else 
		diff.x = point.x;

	if (py)
		diff.y = point.y - (*py);
	else
		diff.y = 0;



	BOOL bMoved = FALSE;
	CVector2D center = m_rect.GetCenter();
	CPointf ptMove(point.x, point.y);
	CPointf ptMoveOld;
	BOOL bRotate = FALSE;
	float rotation = m_fRotation;
	CRectF rectOld;
	float fOldRotation;

	CPoint ptScreenClick;
	CPoint ptScreenMove;
	::GetCursorPos(&ptScreenClick);
	ScreenToClient(m_pOwner->m_hWnd, &ptScreenClick);
	
	CPointf	ptMoveOriginal;

	// get messages until capture lost or cancelled/accepted
	for (;;)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (CWnd::GetCapture() != pWnd)
			break;

		switch (msg.message)
		{
		// handle movement/accept messages
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			ptScreenMove.x		= GET_X_LPARAM(msg.lParam);
			ptScreenMove.y		= GET_Y_LPARAM(msg.lParam);
			ptMoveOriginal.x	= static_cast<float>(ptScreenMove.x);
			ptMoveOriginal.y	= static_cast<float>(ptScreenMove.y);
			rectOld				= m_rect;
			fOldRotation		= m_fRotation;
			
			//Allow the parent window to convert screen coordinates to its own coordinate system
			adjustMousePos(ptMoveOriginal);

			ptMoveOld = ptMove;
			ptMove = _getRealPoint(ptMoveOriginal);

			// handle resize cases (and part of move)
			if (nFlags & MK_CONTROL)
			{
				SetRotation(rotation + (ptScreenClick.y - ptScreenMove.y) );
				bRotate = TRUE;
			}
			else
			{
				bRotate = FALSE;

				if (px){
					*px = (ptMoveOriginal.x - diff.x);
				}

				if (py) {
					*py = (ptMoveOriginal.y - diff.y);
				}

				// handle move case
				if (nHandle == CENTER)
				{
					m_rect.m_Max.x = m_rect.m_Min.x + nWidth;
					m_rect.m_Max.y = m_rect.m_Min.y + nHeight;
				} 
				else
				{
					if (px){
						*px = (ptMove.x - diff.x);
					}

					if (py) {
						*py = (ptMove.y - diff.y);
					}

					if (nFlags & MK_SHIFT)
					{
						//keep the original center
						m_rect.SetCenter(center);
					}
					else
					{
						//Adjust the position with respect to the current rotation
						CVector2D newCenter = m_rect.GetCenter();

						CVector2D centerOffset = (newCenter-center);
						float rad = DegToRad(rotation);
						centerOffset.Rotate(rad);

						center += centerOffset;
						m_rect.SetCenter(center);
					}
				}
			}
			// allow caller to adjust the rectangle if necessary
			//AdjustRect(nHandle, &m_rect);

			// only redraw and callback if the rect actually changed!
			bFinalErase = (msg.message == WM_LBUTTONUP);
			if (!rectOld.EqualRect(m_rect) || bFinalErase || bRotate)
			{
				if (bMoved || bRotate)
				{
					Draw();
				}
				OnChangedRect(rectOld, ptMove, ptMoveOld, fOldRotation, bRotate);
				if (msg.message != WM_LBUTTONUP)
					bMoved = TRUE;
			}
			if (bFinalErase)
				goto ExitLoop;

			if (!rectOld.EqualRect(m_rect))
			{
				Draw();
			}
			break;

		// handle cancel messages
		case WM_KEYDOWN:
			if (msg.wParam != VK_ESCAPE)
				break;
		case WM_RBUTTONDOWN:
			if (bMoved)
			{
				bFinalErase = TRUE;
				Draw();
			}
			m_rect = rectSave;
			goto ExitLoop;

		// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

ExitLoop:
	//m_pRotationCenter = nullptr;
	ReleaseCapture();

	AfxUnlockTempMaps(FALSE);

	// restore rect in case bMoved is still FALSE
	if (!bMoved)
		m_rect = rectSave;

	// return TRUE only if rect has changed
	BOOL bResult = ( !rectSave.EqualRect(m_rect) || (m_fRotation != rotation) );

	if (bResult)
	{
		OnEndTrack();
	}

	return bResult;
}

void CTrackerRect::GetModifyPointers(int nHandle, float**ppx, float**ppy)
{
	*ppx = nullptr;
	*ppy = nullptr;

	switch(nHandle)
	{
		case CENTER :
		case BOTTOM_LEFT:
			*ppx = (&m_rect.m_Min.x);
			*ppy = (&m_rect.m_Min.y);
			break;
		case BOTTOM_RIGHT:
			*ppx = (&m_rect.m_Max.x);
			*ppy = (&m_rect.m_Min.y);
			break;
		case TOP_RIGHT :
			*ppx = (&m_rect.m_Max.x);
			*ppy = (&m_rect.m_Max.y);
			break;
		case TOP_LEFT :
			*ppx = (&m_rect.m_Min.x);
			*ppy = (&m_rect.m_Max.y);
			break;
		case BOTTOM :
			*ppy = (&m_rect.m_Min.y);
			break;
		case RIGHT:
			*ppx = (&m_rect.m_Max.x);
			break;
		case TOP :
			*ppy = (&m_rect.m_Max.y);
			break;
		case LEFT :
			*ppx = (&m_rect.m_Min.x);
			break;
	}
}

float CTrackerRect::GetRotation() const
{
	return m_fRotation;
}

void CTrackerRect::SetRotation(float nDegree)
{
	nDegree = fmod(nDegree, 360.0f );

	if (nDegree < 0.0f) {
		m_fRotation = 360.0f + nDegree;
	} else {
		m_fRotation = nDegree;
	}
}

void CTrackerRect::SetRect(const CRectF &rect)
{
	m_rect = rect;
}

