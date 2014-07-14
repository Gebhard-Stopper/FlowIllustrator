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

// OpenGlDummyWnd.cpp : implementation file
//

#include "stdafx.h"
#include "FlowIllustrator.h"
#include "OpenGlDummyWnd.h"


// COpenGlDummyWnd

IMPLEMENT_DYNAMIC(COpenGlDummyWnd, CWnd)

COpenGlDummyWnd::COpenGlDummyWnd()
{

}

COpenGlDummyWnd::~COpenGlDummyWnd()
{
}


BEGIN_MESSAGE_MAP(COpenGlDummyWnd, CWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// COpenGlDummyWnd message handlers




BOOL COpenGlDummyWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CWnd::PreCreateWindow(cs);
}


int COpenGlDummyWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	Init();

	return 0;
}

void COpenGlDummyWnd::Init()
{
	m_pDC = new CClientDC(this);
	ASSERT(m_pDC != NULL);

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
		1,                              // version number
		PFD_DRAW_TO_WINDOW |            // support window
			PFD_SUPPORT_OPENGL |          // support OpenGL
			PFD_DOUBLEBUFFER,             // double buffered
		PFD_TYPE_RGBA,                  // RGBA type
		32,                             // 24-bit color depth
		0, 0, 0, 0, 0, 0,               // color bits ignored
		8,                              // 8-bit alpha buffer
		0,                              // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
		24,                             // 32-bit z-buffer
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};

	m_nPixelFormat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);

	if (SetPixelFormat(m_pDC->GetSafeHdc(), m_nPixelFormat, &pfd) == FALSE) 
		return;

	m_hRc = wglCreateContext(m_pDC->GetSafeHdc());

	if ( !m_hRc )
		return;

	if (!wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRc))
		return;

	glewInit();

	return;
}