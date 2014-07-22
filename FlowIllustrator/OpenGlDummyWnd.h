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


/**
 *	This class serves the only purpose to nintialize a dummy OpenGL context
 *	in order to setup multisampling for the main application window by 
 *	choosing an appropriate pixel format, using wglChoosePixelFormatARB.
 */

class COpenGlDummyWnd : public CWnd
{
	DECLARE_DYNAMIC(COpenGlDummyWnd)

public:
	COpenGlDummyWnd();
	virtual ~COpenGlDummyWnd();

protected:
	CClientDC	*m_pDC;
	HGLRC		 m_hRc;

public:
	int			 m_nPixelFormat;

public:
	HDC GetSafeHDC() const {
		return m_pDC->GetSafeHdc();
	}

protected:
	void Init();

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


