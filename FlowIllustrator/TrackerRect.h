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

/**
 *	WM_TRACKRECT_CHANGED is sent via _notifyParent() to the tracker rect's parent window, each time the size or position of the tracker rect has changed.
 *	@param WPARAM A pointer to a trackRectUpdate structure
 *	@param LPARAM NULL
 */
const UINT WM_TRACKRECT_CHANGED = RegisterWindowMessage(_T("TRACKRECT_CHANGED_RECT"));

/**
 *	WM_TRACKRECT_ADJUST_MOUSE_POS is sent via _notifyParent() to the tracker rect's parent window, each time tracker rect
 *	intercepts a WM_MOUSEMOVE or WM_LBUTTONUP message. It allows to convert the original screen coordinates into a user defined
 *	coordinate system.
 *
 *	@param WPARAM A pointer to a CPointf structure, containing the current mouse location in screen coordinates
 *	@param LPARAM NULL
 */
const UINT WM_TRACKRECT_ADJUST_MOUSE_POS = RegisterWindowMessage(_T("TRACKRECT_ADJUST_MOUSE_POS"));

/**
 *	WM_TRACKRECT_END_TRACK is sent via _notifyParent() to the tracker rect's parent window, if mouse tracking by the tracker rect has ended.
 *	@param WPARAM A pointer to a trackRectUpdate structure. Only the angle and rect members of the trackRectUpdate structure are valid.
 *	@param LPARAM NULL
 */
const UINT WM_TRACKRECT_END_TRACK = RegisterWindowMessage(_T("TRACKRECT_END_TRACK"));

typedef struct TRACK_RECT_UPDATE_S
{
	CRectF		rect;			/**< The current rectangle of the CTrackerRect*/
	CRectF		oldRect;		/**< The rectangle of the CTrackerRect before its last change.*/
	CPointf		ptMouseMove;	/**< The mouse movement between the last and current mouse location.*/
	float		angle;			/**< The current orientation of the tracker rect.*/
	float		oldAngle;		/**< The previous orientation of the tracker rect.*/
	bool		bRotate;		/**< Indicates, if the tracker rect is currently being rotated.*/

}trackRectUpdate, *LPTRACKRECTUPDATE;

/**
 *	A CTrackerRect represents a rectnagular region on the screen. If clicked inside a tracker rect or on one of its handles
 *	it captures the mouse input, until the mouse button is released, or ESC is pressed.<BR>
 *	The trackerrect sends window messages to its parent window to notify it that:
 *		- A new mouse position has been aquired
 *		- The trackerrect has changed
 *		- Tracking the mouse has ended
 *	<BR>
 *	Clicking on one of the handles on the exterior allows to adjust the extent of the tracker rect, while clicking inside
 *	the tracker rect allows to drag it over the screen.
 *	Holding the SHIFT-key pressed while using a handle will alter the tracker rects extent, but at the same time maintains its current 
 *	center location. Pressign the CTRL-key allows to rotate the tracker rect about its center.
 */

class CTrackerRect
{
public:
	enum TR_HandleID 
	{	NONE			= -1, 
		BOTTOM_LEFT		= 0, 
		BOTTOM_RIGHT	= 1, 
		TOP_RIGHT		= 2,  
		TOP_LEFT		= 3, 
		BOTTOM			= 4, 
		RIGHT			= 5, 
		TOP				= 6, 
		LEFT			= 7, 
		CENTER			= 8
	};

public:
	/**
	 *	Construct a new CTrackerRect.
	 *
	 *	@param pOwner Pointer to this tracker rect's parent window.
	 *	@param rect Reference to a CRectF, containing the initial rectangle of this CTrackerRect.
	 *
	 *	@remarks	The parent window of this CTrackerRect receives window messages concerning updates
	 *				and changes the CTrackerRect undergoes.
	 */
	CTrackerRect( CWnd* pOwner, const CRectF& rect );
	virtual ~CTrackerRect(void);

protected:
	CRectF	m_rect;				/**< The rectangular region of this CTrackerRect.*/
	CWnd*	m_pOwner;			/**< Pointer to the parent CWnd of this CTrackerRect.*/
	float	m_fRotation;		/**< The current rotation of this CTrackerRect, relative to the horizontal axis.*/

	int		m_nHighlightId		/**< ID of the current tracker handle.*/;

protected:
	/**
	 *	Retrieves the rectangle of this CTrackerRect, including the tracker handles.
	 *
	 *	@param rect A reference to a CRectF, recieving the rectangle of this CTrackerRect.
	 */
	void GetTrueRect(CRectF &rect) const;

	/**
	 *	Retrieve the rectangle of the tracker handle, specified by nHandleID.
	 *
	 *	@param nHandleID ID of the handle, whose rectangular region is to be retrieved.
	 *	@param pRect A pointer to a CRectF, which recieves the resulting rectangle.
	 *
	 *	@remarks If no valid handle ID is passed, this function retrieves an empty CRectF.
	 */
	void GetHandleRect(int nHandleID, CRectF *pRect) const;

	/**
	 *	Retrieves a UINT mask indicating, which of the eight tracker handles are valid.
	 *
	 *	@return A UINT, where the tirst eight bit indicate which tracker handles are valid.
	 *	@See TR_HandleID
	 */
	UINT GetHandleMask() const;

	/**
	 *	Draws the tracker rect.
	 *
	 *	@param rect The rect to be drawn.
	 *	@param bSolid Determines, if the rect is to be drawn solid, or as outline.
	 */
	void DrawRect(const CRectF &rect, BOOL bSolid) const;

	/**
	 *	Draws the a tracker handle.
	 *
	 *	@param rect The rect to be drawn.
	 *	@param bSolid Determines, if the rect is to be drawn solid, or as outline.
	 */
	void DrawHandleRect(const CRectF &rect, BOOL bSolid) const;

	/**
	 *	Determine, if a point  is inside the tracker rect or one of its handles and return the corresponding handle ID.
	 *
	 *	@param point A reference to the CPointf to be checked.
	 *
	 *	@return The ID of the corresponding handle, or -1 if the specified point is not inside the tracker rect.
	 */
	int HandleHitTest(const CPointf& point) const;

	/**
	 *	Does the actual tracking of the mouse position and dispatches incomming windows messages.
	 *	Notifies the parent window bout all relevant changes.
	 *
	 *	@param nHandle ID of the currently active handle.
	 *	@param pWnd Pointer to the CWnd, whose messages are intercepted.
	 *	@param nFlags Flag which indicates if additional function keys, such as MK_CONTROL or MK_SHIFT are pressed.
	 *
	 *	@return Returns TRUE, if the trackerrect has changed, with respect to its original size or location, otherwise FALSE.
	 */
	BOOL TrackHandle(int nHandle, CWnd* pWnd, CPointf point, UINT nFlags);

	/**
	 *	Retrieve pointers of the underlying CRectF to be modified by the specified handle.
	 *
	 *	@param nHandle The tracker handle to modify this CTrackerRect.
	 *	@param ppx A pointer to a pointer to float. Receives a pointer to the x-coordinate of the affected coordinate of the underlying CRectF.
	 *	@param ppy A pointer to a pointer to float. Receives a pointer to the y-coordinate of the affected coordinate of the underlying CRectF.
	 */
	void GetModifyPointers(int nHandle, float**ppx, float**ppy);

	/**
	 *	Transforms a point onto the rotated tracker rect.
	 *
	 *	@param point The original point, e.g. the mouse location
	 *	@param bInvert Rotate the specified point clock-wise about the center of this CTrackerRect if true, counter clock-wise if false.
	 *
	 *	@return The real point, as it corresponds to the rotated CTrackerRect.
	 */
	CPointf _getRealPoint(const CPointf &point, bool bInvert = true) const;

	/**
	 *	Sends the WM_TRACKRECT_ADJUST_MOUSE_POS message to the parent CWnd of this CTrackerRect.
	 *
	 *	@param point a Pointer to a CPointf of the current mouse position in windows screen-coordinates.
	 *
	 *	@remarks	You can treat the WM_TRACKRECT_ADJUST_MOUSE_POS and transform point to your own coordinate system.
	 *				CTrackerRect internally uses the CPointf structure pointed to by point.
	 */
	void adjustMousePos(CPointf &point);

public:
	/**
	 *	Determine, if a point is inside the tracker rect or one of its handles and return the corresponding handle ID.
	 *
	 *	@param point A reference to the CPointf to be checked.
	 *
	 *	@return The ID of the corresponding handle, or -1 if the specified point is not inside the tracker rect.
	 */
	int HitTest(const CPointf& point) const;

	/**
	 *	Retrieve a copy of the CRectF, defining rectangular region of this CTrackerRect.
	 *
	 *	@return A CRectF, defining rectangular region of this CTrackerRect.
	 *
	 *	@remarks The returned CRectF covers the region of this CTrackerRect not including the surrounding tracker handles.
	 */
	CRectF GetRect() const;

	/**
	 *	Draws this CTrackerRect and its tracker handles
	 */
	void Draw() const;

	/**
	 * Begin tracking of mouse input.
	 * Call this function to redirect all mouse and keyboard input to the tracker rect.
	 *
	 *	@param pWnd Pointer to the CWnd, whose mouse and keyboard messages are to be intercepted.
	 *	@param point A CPointf, specifying the current mouse location.
	 *	@param nFlags Flag, which indicates if additional function keys were pressed (e.g. MK_CONTROL or MK_SHIFT).
	 *
	 *	@return Returns TRUE, if the trackerrect has changed, with respect to its original size or location, otherwise FALSE.
	 */
	BOOL Track(CWnd* pWnd, CPointf point, UINT nFlags);

	/**
	 *	Specifies a tracker handle that is drawn in the current highlight colour.
	 *
	 * @param nHandleID The ID of the highlighted handle.
	 */
	void Highlight(int nHandleID) {
		m_nHighlightId = nHandleID;
	}

	/**
	 *	Retrieve the current rotation of this CTrackerRect.
	 *
	 *	@return The rotation in degree.
	 */
	float GetRotation() const;

	/**
	 *	Set the curretn rotation of this CTrackerRect.
	 *
	 * @param nDegree The new rotation in degree.
	 */
	void SetRotation(float nDegree);

	/**
	 *	Set the rectangular region of this CTrackerRect.
	 *
	 *	@param rect A reference to a CRectF, specifying the the rectangular region.
	 */
	void SetRect(const CRectF &rect);

protected:
	/**
	 *	Invoked by TrackHandle to notify the parent window, that the tracker rect has changed.
	 */
	virtual void OnChangedRect (const CRectF& rectOld, const CPointf &ptMousePos,  const CPointf &ptMousePosOld,const float oldRotation, bool bRotate);
	
	/**
	 *	Invoked by TrackHandle to notify the parent window, that mouse tracking has ended.
	 */
	void OnEndTrack();

private:
	/**
	 *	Notifies the parent window, by invoking it's SendMessage() member function.
	 *	This function only returns, if the parent window has finished handling the received message.
	 */
	BOOL _notifyParent(UINT nMsg, WPARAM wParam = NULL, LPARAM lParam = NULL);
};

