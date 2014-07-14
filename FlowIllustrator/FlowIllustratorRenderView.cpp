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

// FlowIllustratorRenderView.cpp : implementation file
//

#include "stdafx.h"
#include "FlowIllustrator.h"
#include "FlowIllustratorRenderView.h"
#include "MainFrm.h"
#include "Line.h"
#include "OpenGlDummyWnd.h"

// CFlowIllustratorRenderView

const GLuint MSAA_SAMPLES = 4;
#define RENDER_2D
const float PI = 3.14159265f;

unsigned char threeto8[8] =
{
	0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377
};

unsigned char twoto8[4] =
{
	0, 0x55, 0xaa, 0xff
};

unsigned char oneto8[2] =
{
	0, 255
};

static PIXELFORMATDESCRIPTOR pfd =
{
	sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
	1,                              // version number
	PFD_DRAW_TO_WINDOW |            // support window
		PFD_SUPPORT_OPENGL |        // support OpenGL
		PFD_DOUBLEBUFFER,           // double buffered
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

IMPLEMENT_DYNCREATE(CFlowIllustratorRenderView, CView)

CFlowIllustratorRenderView::CFlowIllustratorRenderView()
	: m_NewObjectColor(0,0,0)
{
	m_pDC						= nullptr;
	m_pOldPalette				= nullptr;
	m_pScreenCoordinateBuffer	= nullptr;
	m_pColorBuffer				= nullptr;
	m_pVortField				= nullptr;
	m_pVortFieldAbs				= nullptr;
	m_pDrawObjMngr				= nullptr;
	m_pLICNoiseTex				= nullptr;
	m_fPixelUnitRatioX			= 1.0f;
	m_fPixelUnitRatioY			= 1.0f;
	gZoomFactor					= 1.0;
	m_fIsoVal					= 0.0f;
	m_nColorOffset				= 0;
	m_xExtent					= 0;
	m_yExtent					= 0;
	m_nLicWidth					= 0;
	m_nLicHeight				= 0;
	m_nLicSeed					= 42;
	m_nLicIntegrationLen		= 1000;
	m_bVorticityValid			= FALSE;
	m_bLICNoiseTexValid			= FALSE;
	m_bDrawCoordinateAxes		= TRUE;

	m_ptViewportCenter			= CPointf(-1,-1);

	m_pDisplayFunc				= &CFlowIllustratorRenderView::renderColorCoding;

	m_nDisplayFuncID	= 1;

	//Add available display functions to vector
	m_DisplayFunctions.push_back(	std::pair<CString, void (CFlowIllustratorRenderView::*)(const CAmiraVectorField2D*)> 
									("None", nullptr) );

	m_DisplayFunctions.push_back(	std::pair<CString, void (CFlowIllustratorRenderView::*)(const CAmiraVectorField2D*)> 
									("Vector color coding", &CFlowIllustratorRenderView::renderColorCoding) );

	m_DisplayFunctions.push_back(	std::pair<CString, void (CFlowIllustratorRenderView::*)(const CAmiraVectorField2D*)> 
									("Vector magnitude", &CFlowIllustratorRenderView::renderVectorMagnitude) );

	m_DisplayFunctions.push_back(	std::pair<CString, void (CFlowIllustratorRenderView::*)(const CAmiraVectorField2D*)> 
									("Vorticity", &CFlowIllustratorRenderView::renderVorticity) );

	m_DisplayFunctions.push_back(	std::pair<CString, void (CFlowIllustratorRenderView::*)(const CAmiraVectorField2D*)> 
									("Derivatives in X", &CFlowIllustratorRenderView::renderDerivatives_x) );

	m_DisplayFunctions.push_back(	std::pair<CString, void (CFlowIllustratorRenderView::*)(const CAmiraVectorField2D*)> 
									("Derivatives in Y", &CFlowIllustratorRenderView::renderDerivatives_y) );

	m_DisplayFunctions.push_back(	std::pair<CString, void (CFlowIllustratorRenderView::*)(const CAmiraVectorField2D*)> 
									("Iso contours", &CFlowIllustratorRenderView::renderIsoLines) );

	m_DisplayFunctions.push_back(	std::pair<CString, void (CFlowIllustratorRenderView::*)(const CAmiraVectorField2D*)> 
									("LIC", &CFlowIllustratorRenderView::renderLIC) );
}

CFlowIllustratorRenderView::~CFlowIllustratorRenderView()
{
}

BEGIN_MESSAGE_MAP(CFlowIllustratorRenderView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CFlowIllustratorRenderView drawing

void CFlowIllustratorRenderView::OnDraw(CDC* /*pDC*/)
{
	if (m_pColorBuffer && m_pScreenCoordinateBuffer)
			DrawScene();
}


// CFlowIllustratorRenderView diagnostics

#ifdef _DEBUG
void CFlowIllustratorRenderView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CFlowIllustratorRenderView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif

CFlowIllustratorDoc* CFlowIllustratorRenderView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFlowIllustratorDoc)));
	return (CFlowIllustratorDoc*)m_pDocument;
}
#endif //_DEBUG

void CFlowIllustratorRenderView::Init()
{
	m_pDC = new CClientDC(this);

	ASSERT(m_pDC != NULL);

	if (bSetupPixelFormat())
		glEnable(GL_MULTISAMPLE_ARB);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//GLUT insits on at least a program name as parameter
	char *my_argv[] = { "FlowIllustrator", NULL };
	int   my_argc = 1;
	glutInit(&my_argc, my_argv);
	
	glewInit();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	GetClientRect(&m_oldRect);
	glDisable(GL_DEPTH_TEST);

	SetProjection(0, m_oldRect.Width(), 0, m_oldRect.Height());
}

BOOL CFlowIllustratorRenderView::bSetupPixelFormat()
{
	COpenGlDummyWnd dummy;

	BOOL bResult = dummy.Create(_T("STATIC"), NULL, WS_CHILD, CRect(0,0,50,50), this, 1234);
	
	GLint maxSamples;

	//get the maximum supported number of MSAA samples
	glGetIntegerv(GL_MAX_SAMPLES_EXT, &maxSamples);
	if (maxSamples > 16) maxSamples = 16;

	int attributes[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,     32,
		WGL_ALPHA_BITS_ARB,     8,
		WGL_DEPTH_BITS_ARB,     24,
		WGL_STENCIL_BITS_ARB,   8,
		WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
		WGL_SAMPLE_BUFFERS_ARB, 1,
		WGL_SAMPLES_ARB,        maxSamples,
		0, 0
	};

	UINT numFormats = 0;
	BOOL bStatus = FALSE;
      
	bStatus = wglChoosePixelFormatARB(dummy.GetSafeHDC(), attributes, 0, 1,
				&m_nPixelFormatMSAA, &numFormats);
				
	if ( !bStatus == TRUE || !numFormats) 
	{
		m_nPixelFormatMSAA = dummy.m_nPixelFormat;
	} 

	return bSetupDummyPixelFormat(m_pDC->GetSafeHdc());
}

BOOL CFlowIllustratorRenderView::bSetupDummyPixelFormat(HDC hDC)
{
	if (SetPixelFormat(hDC, m_nPixelFormatMSAA, &pfd) == FALSE) 
		return FALSE;

	m_hRc = wglCreateContext(hDC);
	if (!m_hRc)
		return FALSE;

	if (!wglMakeCurrent(hDC, m_hRc))
		return FALSE;

	return TRUE;

}

bool ExtensionSupported(const char *pszExtensionName)
{
    static const char *pszGLExtensions = 0;
    static const char *pszWGLExtensions = 0;

    if (!pszGLExtensions)
        pszGLExtensions = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

    if (!pszWGLExtensions)
    {
        // WGL_ARB_extensions_string.

        typedef const char *(WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC);

        PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB =
            reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(
            wglGetProcAddress("wglGetExtensionsStringARB"));

        if (wglGetExtensionsStringARB)
            pszWGLExtensions = wglGetExtensionsStringARB(wglGetCurrentDC());
    }

    if (!strstr(pszGLExtensions, pszExtensionName))
    {
        if (!strstr(pszWGLExtensions, pszExtensionName))
            return false;
    }

    return true;
}

void CFlowIllustratorRenderView::SetProjection(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45.0f, fAspect, fNearPlane, fFarPlane);
	glOrtho ( left, right, bottom, top, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
}

void CFlowIllustratorRenderView::DrawScene(void)
{
	CFlowIllustratorDoc *pDoc = GetDocument();

	if (pDoc) { //Onbly perform actual rendering of the flow field if it is dirty
		if (pDoc->isDirty()) {
			renderVectorField();
			pDoc->ResetDirtyBit();
		}
	}

	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(2);
	glLineWidth(1.0f);

	//Draw the buffer containing the rendered flow field
	if(m_pDisplayFunc)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glColorPointer(3, GL_FLOAT, 0, m_pColorBuffer);
		glVertexPointer(2, GL_FLOAT, 0, m_pScreenCoordinateBuffer);

		glDrawArrays(GL_POINTS, 0, m_yExtent * m_xExtent);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}

	if (m_IsoLineMngr.Size() > 0) {
		m_IsoLineMngr.Draw();	//Draw iso lines
	}

	if (m_pDrawObjMngr)	{	//Draw flow templates
		m_pDrawObjMngr->Draw();
	}

	OnDrawScene();	//allow child-class to draw additional stuff

	//Draw the coordinate axes
	if (m_bDrawCoordinateAxes)
		DrawCoordinateAxes();

	glFinish();

	SwapBuffers(wglGetCurrentDC());
}

void CFlowIllustratorRenderView::DrawCoordinateAxes(void)
{
	if(m_rcViewPort.IsEmptyRect()) return;

	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(1.0f);
	static char str[24];

	//Draw Y-Axis
	glViewport(	static_cast<GLint>(m_xMin-COORDINATE_AXIS_WIDTH),
				static_cast<GLint>(m_yMin), 
				static_cast<GLsizei>(COORDINATE_AXIS_WIDTH), 
				static_cast<GLsizei>(m_yExtent) );

	SetProjection(0, COORDINATE_AXIS_WIDTH, m_rcViewPort.m_Min.y, m_rcViewPort.m_Max.y);

	float stepY = m_rcViewPort.getHeight()/10.0f;

	for (float j=m_rcViewPort.m_Min.y; j <=m_rcViewPort.m_Max.y; j+= stepY )
	{
		glBegin(GL_LINES);
		glVertex2f(3*COORDINATE_AXIS_WIDTH/4, j);
		glVertex2f(COORDINATE_AXIS_WIDTH, j);
		glEnd();

		sprintf_s(str, 24, "%.2f", j);
		glRasterPos2f(0,j);
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (unsigned char*)str);
	}


	//Draw X-Axis
	glViewport(	static_cast<GLint>(m_xMin),
				static_cast<GLint>(m_yMin-COORDINATE_AXIS_WIDTH), 
				static_cast<GLsizei>(m_xExtent), 
				static_cast<GLsizei>(COORDINATE_AXIS_WIDTH) );

	SetProjection(m_rcViewPort.m_Min.x, m_rcViewPort.m_Max.x, 0, COORDINATE_AXIS_WIDTH);

	float stepX =  m_rcViewPort.getWidth()/10.0f;

	for (float i=m_rcViewPort.m_Min.x; i <=m_rcViewPort.m_Max.x; i+=stepX)
	{
		glBegin(GL_LINES);
		glVertex2f(i, COORDINATE_AXIS_HEIGHT/2.0f);
		glVertex2f(i, COORDINATE_AXIS_WIDTH);
		glEnd();

		sprintf_s(str, 24, "%.2f", i);
		glRasterPos2f(i,10);
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (unsigned char*)str);
	}


	//Reset viewport
	glViewport(m_xMin, m_yMin, m_xExtent, m_yExtent);
	SetProjection(m_rcViewPort.m_Min.x, m_rcViewPort.m_Max.x, m_rcViewPort.m_Min.y, m_rcViewPort.m_Max.y);
}

void CFlowIllustratorRenderView::GetDisplayFunctionNames(CStringArray &arrNames) const
{
	arrNames.RemoveAll();

	for (size_t i = 0; i < m_DisplayFunctions.size(); i++)
	{
		arrNames.Add(m_DisplayFunctions[i].first);
	}
}

int CFlowIllustratorRenderView::GetDisplayFunctionID() const
{
	return m_nDisplayFuncID;
}

void CFlowIllustratorRenderView::SetDisplayFunction(int nID)
{
	if(nID >= 0 && nID < static_cast<int>(m_DisplayFunctions.size()) )
	{
		m_IsoLineMngr.RemoveAll();
		m_nDisplayFuncID = nID;
		m_pDisplayFunc = m_DisplayFunctions[nID].second;
		Dirty();
	}
}

int CFlowIllustratorRenderView::GetLICWidth() const
{
	return m_nLicWidth;
}

int CFlowIllustratorRenderView::GetLICHeight() const
{
	return m_nLicHeight;
}

int CFlowIllustratorRenderView::GetLICHalfSize() const
{
	return m_nLicIntegrationLen;
}

int CFlowIllustratorRenderView::GetLICSeed() const
{
	return m_nLicSeed;
}

void CFlowIllustratorRenderView::SetLICWidth(int nWidth)
{
	if (m_nLicWidth != nWidth) {
		m_nLicWidth = nWidth;
		m_bLICNoiseTexValid = FALSE;
	}
}

void CFlowIllustratorRenderView::SetLICHeight(int nHeight)
{
	if (m_nLicHeight != nHeight) {
		m_nLicHeight = nHeight;
		m_bLICNoiseTexValid = FALSE;
	}
}

void CFlowIllustratorRenderView::SetLICHalfSize(int nHalfSize)
{
	m_nLicIntegrationLen = nHalfSize;
}

void CFlowIllustratorRenderView::SetLICSeed(int nSeed)
{
	if (m_nLicSeed != nSeed) {
		m_nLicSeed = nSeed;
		m_bLICNoiseTexValid = FALSE;
	}
}

void CFlowIllustratorRenderView::SetIsoValue(float IsoVal)
{
	m_fIsoVal = IsoVal;
	Dirty();
}

void CFlowIllustratorRenderView::SetMinMaxIsoValue(float fMin, float fMax)
{
	CMainFrame *pMainFrm = reinterpret_cast<CMainFrame*>(GetParent());
	if (pMainFrm)
	{
		pMainFrm->SetMinMaxIsoValue(fMin, fMax);
	}
}

void CFlowIllustratorRenderView::SetColrOffset(int nBaseColor)
{
	m_nColorOffset = nBaseColor;
	Dirty();
}

void CFlowIllustratorRenderView::ResetViewport()
{
	CFlowIllustratorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) {
		return;
	}

	CRect rcClient;
	GetClientRect(&rcClient);

	gZoomFactor = 1.0f;

	const CAmiraVectorField2D *pVecField = pDoc->GetVectorfield();
	if (pVecField) {
		m_rcViewPort = m_rcDomain;
	} else {
		m_rcViewPort = CRectF::FromCRect(rcClient);
	}

	CalcExtents();
	AdjustViewport();
	m_bLICNoiseTexValid = FALSE;
}

void CFlowIllustratorRenderView::Dirty()
{
	CFlowIllustratorDoc *pDoc = GetDocument();
	if(pDoc)
		pDoc->isDirty(TRUE);
}

BOOL CFlowIllustratorRenderView::AcquireVorticityField()
{
	if (m_bVorticityValid) return m_bVorticityValid;

	CFlowIllustratorDoc *pDoc = GetDocument();
	if (pDoc)
	{
		const CAmiraVectorField2D *pVecField = pDoc->GetVectorfield();
		if (pVecField)
		{
			if (m_pVortField) {delete m_pVortField; m_pVortField = nullptr;}
			if (m_pVortFieldAbs) {delete m_pVortFieldAbs; m_pVortFieldAbs = nullptr;}

			m_pVortField = pVecField->GetVorticityField();

			if (m_pVortField)
			{
				m_pVortField->Smooth(5);
				m_pVortFieldAbs = m_pVortField->Abs();

				SetMinMaxIsoValue(m_pVortField->GetMinValue(), m_pVortField->GetMaxValue());

				return (m_bVorticityValid = TRUE);
			}
		}
	}

	return (m_bVorticityValid = FALSE);
}

void CFlowIllustratorRenderView::AdjustViewport(BOOL bCenterOnMousePos)
{
	CFlowIllustratorDoc* pDoc = GetDocument();
	if (pDoc) {
		const CAmiraVectorField2D *pVecField = pDoc->GetVectorfield();
		if (pVecField) {
			CRect r;
			GetClientRect(&r);

			CVector2D center = m_rcViewPort.GetCenter();

			if (bCenterOnMousePos && m_rcDomain.PtInRect(m_ptViewportCenter)) {
				center -= (center-CVector2D(m_ptViewportCenter.x, m_ptViewportCenter.y));
			}

			float DomainExtX;
			float DomainExtY;

			if (pVecField->GetExtentX() > pVecField->GetExtentY())
			{
				DomainExtX = gZoomFactor*r.Width();
				DomainExtY = DomainExtX/m_xyRatio;
			}
			else
			{
				DomainExtY = gZoomFactor*r.Height();
				DomainExtX = DomainExtY*m_xyRatio;
			}

			if (m_bDrawCoordinateAxes) {
				DomainExtX -= (2*COORDINATE_AXIS_WIDTH);
				DomainExtY -= (COORDINATE_AXIS_HEIGHT);
			}

			if (DomainExtX >= m_xExtent ) {
				if (gZoomFactor == 1.0f) {
					m_rcViewPort.m_Min.x = m_rcDomain.m_Min.x;
					m_rcViewPort.m_Max.x = m_rcDomain.m_Max.x;
				} else {

					float w = m_rcDomain.getWidth() / (DomainExtX/m_xExtent);

					m_rcViewPort.m_Min.x = center.x - (w/2.0f);
					m_rcViewPort.m_Max.x = center.x + (w/2.0f);

					if (m_rcViewPort.m_Min.x < m_rcDomain.m_Min.x) {
						float diff = m_rcDomain.m_Min.x - m_rcViewPort.m_Min.x;
						m_rcViewPort.m_Min.x = m_rcDomain.m_Min.x;
						m_rcViewPort.m_Max.x += diff;
					}
					else if (m_rcViewPort.m_Max.x > m_rcDomain.m_Max.x) {
						float diff = m_rcViewPort.m_Max.x - m_rcDomain.m_Max.x;
						m_rcViewPort.m_Max.x = m_rcDomain.m_Max.x;
						m_rcViewPort.m_Min.x -= diff;
					}
				}
			}

			if (DomainExtY > m_yExtent) {
				if (gZoomFactor == 1.0f) {
					m_rcViewPort.m_Min.y = m_rcDomain.m_Min.y;
					m_rcViewPort.m_Max.y = m_rcDomain.m_Max.y;
				} else {

					float h = m_rcDomain.getHeight() / (DomainExtY/m_yExtent);

					m_rcViewPort.m_Min.y = center.y - (h/2.0f);
					m_rcViewPort.m_Max.y = center.y + (h/2.0f);

					if (m_rcViewPort.m_Min.y < m_rcDomain.m_Min.y) {
						float diff = m_rcDomain.m_Min.y - m_rcViewPort.m_Min.y;
						m_rcViewPort.m_Min.y = m_rcDomain.m_Min.y;
						m_rcViewPort.m_Max.y += diff;
					}
					else if (m_rcViewPort.m_Max.y > m_rcDomain.m_Max.y) {
						float diff = m_rcViewPort.m_Max.y - m_rcDomain.m_Max.y;
						m_rcViewPort.m_Max.y = m_rcDomain.m_Max.y;
						m_rcViewPort.m_Min.y -= diff;
					}
				}
			}
		}
	}

	m_fPixelUnitRatioX = m_xExtent / m_rcViewPort.getWidth();
	m_fPixelUnitRatioY = m_yExtent / m_rcViewPort.getHeight();

	glViewport(m_xMin, m_yMin, m_xExtent, m_yExtent);
	SetProjection(m_rcViewPort.m_Min.x, m_rcViewPort.m_Max.x, m_rcViewPort.m_Min.y, m_rcViewPort.m_Max.y);

	AllocateScreenBuffers();
}

void CFlowIllustratorRenderView::CalcExtents(void)
{
	CFlowIllustratorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) {
		return;
	}

	const CAmiraVectorField2D *pVecField = pDoc->GetVectorfield();

	if (!pVecField)
		return;

	CRect rcView;
	GetClientRect(&rcView);

	m_xExtent = pVecField->GetExtentX();
	m_yExtent = pVecField->GetExtentY();

	m_xyRatio = (float(m_xExtent) / m_yExtent);

	//Adjust width or height s.t. the screen is filled in one direction
	//also, the output is centered at the middle of the client rect

	//This needs to be done upon resizing and when a new file is loaded
	if (m_xExtent > m_yExtent)
	{
		m_xExtent = rcView.Width();
		if (m_bDrawCoordinateAxes) {
			m_xExtent -= static_cast<int>(2*COORDINATE_AXIS_WIDTH);
		}

		m_yExtent = static_cast<int>(gZoomFactor*m_xExtent / m_xyRatio);

		if (m_yExtent > (rcView.Height() - COORDINATE_AXIS_HEIGHT)) {
			m_yExtent = static_cast<int>(rcView.Height() - COORDINATE_AXIS_HEIGHT);
		}

		m_xMin = static_cast<int>(COORDINATE_AXIS_WIDTH);
		m_yMin = ((rcView.Height() / 2) - ((m_yExtent)/2));	
	}
	else
	{
		m_yExtent = rcView.Height();

		if (m_bDrawCoordinateAxes) {
			m_yExtent -= static_cast<int>(COORDINATE_AXIS_HEIGHT);
		}

		m_xExtent = static_cast<int>((m_xyRatio*gZoomFactor) * m_yExtent);

		if (m_xExtent > (rcView.Width() - COORDINATE_AXIS_WIDTH)) {
			m_xExtent = static_cast<int>(rcView.Width() - COORDINATE_AXIS_WIDTH);
		}

		m_xMin = (rcView.Width()/2) - (m_xExtent/2);
		m_yMin = static_cast<int>(COORDINATE_AXIS_WIDTH);
	}

	gPixelPerUnit = pVecField->GetDomainRect().getWidth() / m_xExtent;

	pDoc->isDirty(TRUE);
}

GLint glExtCheckFramebufferStatus(CString &errorMessage)
{
	GLenum status;
	status=glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			//Choose different formats
			errorMessage = _T("Framebuffer object format is unsupported by the video hardware. (GL_FRAMEBUFFER_UNSUPPORTED_EXT)(FBO - 820)");
			return -1;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			errorMessage = _T("Incomplete attachment. (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT)(FBO - 820)");
			return -1;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			errorMessage = _T("Incomplete missing attachment. (GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT)(FBO - 820)");
			return -1;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			errorMessage = _T("Incomplete dimensions. (GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)(FBO - 820)");
			return -1;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			errorMessage = _T("Incomplete formats. (GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT)(FBO - 820)");
			return -1;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			errorMessage = _T("Incomplete draw buffer. (GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT)(FBO - 820)");
			return -1;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			errorMessage = _T("Incomplete read buffer. (GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT)(FBO - 820)");
			return -1;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
			errorMessage = _T("Incomplete multisample buffer. (GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT)(FBO - 820)");
			return -1;
		default:
			//Programming error; will fail on all hardware
			errorMessage = _T("Some video driver error or programming error occured. Framebuffer object status is invalid. (FBO - 823)");
		return -2;
	}
	return 1;
}

void CFlowIllustratorRenderView::AllocateScreenBuffers()
{
	CFlowIllustratorDoc* pDoc = GetDocument();
	if (!pDoc) {
		return;
	}

	if (m_yExtent <= 0 || m_xExtent <= 0) return;

	//InitRenderTargetFBO();

	//Needs to be done when a new file is loaded
	if (m_pColorBuffer)
		delete [] m_pColorBuffer;
	
	m_pColorBuffer = new float[m_yExtent * m_xExtent * 3];

	//Needs to be done when a new file is loaded
	if (m_pScreenCoordinateBuffer)
		delete [] m_pScreenCoordinateBuffer;

	m_pScreenCoordinateBuffer = new float[m_yExtent * m_xExtent * 2];

	float stepX = m_rcViewPort.getWidth()/m_xExtent;
	float stepY = m_rcViewPort.getHeight()/m_yExtent;

	for (int j=0; j<m_yExtent; j++) {
		for (int i=0; i<m_xExtent; i++) {
			reinterpret_cast<CVector2D*>(m_pScreenCoordinateBuffer)[j*m_xExtent+i] = 
				CVector2D(m_rcViewPort.m_Min.x + i*stepX, m_rcViewPort.m_Min.y + j*stepY);
		}
	}

	pDoc->isDirty(TRUE);
}

void CFlowIllustratorRenderView::renderColorCoding(const CAmiraVectorField2D *pVectorField)
{
	/*GLuint buff;
	glGenBuffers( 1, &buff);
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, buff);

	m_ShaderMngr.UseShader("ColCode");
	glBufferData( GL_SHADER_STORAGE_BUFFER, m_yExtent*m_xExtent*sizeof(CVector2D), pVectorField, GL_STATIC_DRAW);
	glDispatchCompute( 32, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	//read data

	float *pColBuff = reinterpret_cast<float*>( glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY) );
	memcpy (m_pColorBuffer, pColBuff, m_xExtent * m_yExtent *3);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glDeleteBuffers(1, &buff);
	
	m_ShaderMngr.UseShader("VertexShdr");
	m_ShaderMngr.UseShader("FragmentShdr");*/

	
	static CVector2D xAxis(1, 0); 

	//calculate the colors for the vectors
	float *pColor = &m_pColorBuffer[0];

	float saturation (1.0f);

	for (int y = m_yMin; y < (m_yExtent+m_yMin); y++) 
	{
		for (int x = m_xMin; x < (m_xExtent+m_xMin); x++) 
		{
			CPointf pos(ScreenToDomainf(CPointf( static_cast<float>(x), static_cast<float>(y))));

			CVector2D v( pVectorField->GetVectorAt(pos) );

			float val (v.abs());
			float dummy ( (xAxis * v) / val );
			float alpha ( (acos(dummy) * 180.0f / PI) + m_nColorOffset ); 

			if (alpha > 360.0) alpha = fmod(alpha, 360.0f);
			if (val > 1.0) val = 1.0f;

			HSVtoRGB(&pColor[0], &pColor[1], &pColor[2], alpha, saturation, val);
			pColor += 3;
		}
	}
}

void CFlowIllustratorRenderView::renderVectorMagnitude(const CAmiraVectorField2D *pVectorField)
{
	//calculate the colors for the vectors
	float *pColor = &m_pColorBuffer[0];

	int numVals = m_yExtent*m_xExtent;
	float *vals = new float [numVals];
	float max = 0.0;

	for (int y = m_yMin; y < (m_yExtent+m_yMin); y++) 
	{
		for (int x = m_xMin; x < (m_xExtent+m_xMin); x++) 
		{
			CPointf pos = ScreenToDomain(CPoint(x, y));

			CVector2D v = pVectorField->GetVectorAt(pos);
			float val = v.abs();

			if (val > max) max = val;
			vals[ (y-m_yMin)*m_xExtent + (x-m_xMin)] = val;
		}
	}

	for (int i=0;i < numVals; i++)
	{
		float val = vals[i]/max;
		float alpha = (acos(val) * 180.0f / PI)+ 90.f + m_nColorOffset;
		if (alpha > 360.0) alpha = fmod(alpha, 360.0f);

		HSVtoRGB(&pColor[0], &pColor[1], &pColor[2], alpha, val, val);
		pColor += 3;
	}

	delete [] vals;
}

void CFlowIllustratorRenderView::renderVorticity(const CAmiraVectorField2D* /*pVectorField*/)
{
	//calculate the colors for the vectors
	float *pColor = &m_pColorBuffer[0];

	if (!AcquireVorticityField()) return;

	float max(m_pVortField->GetMaxValue());
	float min(m_pVortField->GetMinValue());
	float colVar(0);

	for (int y = m_yMin; y < (m_yExtent+m_yMin); y++) 
	{
		for (int x = m_xMin; x < (m_xExtent+m_xMin); x++) 
		{
			CPointf pos(ScreenToDomain(CPoint(x, y)));
			
			float dummy( m_pVortField->GetValue(pos) );

			if (dummy >= 0.0f) {
				dummy = dummy/max;
				colVar = 90.0f;
			} else {
				dummy = -dummy/min;
				colVar = -90.0f;
			}

			float alpha ( (acos(dummy) * 180.0f / PI) + colVar + m_nColorOffset );
			
			dummy*=2.0f;
			float saturation( fabs(dummy) );
			float val( 1.0f - saturation );
			if (alpha > 360.0) alpha = fmod(alpha, 360.0f);
		
			HSVtoRGB(&pColor[0], &pColor[1], &pColor[2], alpha, saturation, val);
			pColor += 3;
		}
	}
}

void CFlowIllustratorRenderView::renderDerivatives_x(const CAmiraVectorField2D *pVectorField)
{
	float *pColor = &m_pColorBuffer[0];

	arma::fmat22 jacobian;

	for (int y = m_yMin; y < (m_yExtent+m_yMin); y++) 
	{
		for (int x = m_xMin; x < (m_xExtent+m_xMin); x++) 
		{
			CPointf pos = ScreenToDomain(CPoint(x, y));

			float val, dummy, alpha, saturation;

			pVectorField->GetJacobian(pos.x, pos.y, &jacobian);
			val = sqrt(jacobian(0,0)*jacobian(0,0) + jacobian(1,0)*jacobian(1,0));
			dummy = 0.0f;
			saturation = 1.0f;

			alpha = (acos(dummy) * 180.0f / PI) + m_nColorOffset; //h

			if (alpha > 360.0) alpha = fmod(alpha, 360.0f);

			HSVtoRGB(&pColor[0], &pColor[1], &pColor[2], alpha, saturation, val);
			pColor += 3;
		}
	}
}

void CFlowIllustratorRenderView::renderDerivatives_y(const CAmiraVectorField2D *pVectorField)
{
	float *pColor = &m_pColorBuffer[0];

	arma::fmat22 jacobian;

	for (int y = m_yMin; y < (m_yExtent+m_yMin); y++) 
	{
		for (int x = m_xMin; x < (m_xExtent+m_xMin); x++) 
		{
			CPointf pos = ScreenToDomain(CPoint(x, y));

			float val, dummy, alpha, saturation;

			pVectorField->GetJacobian(pos.x,pos.y, &jacobian);
			val = sqrt(jacobian(0,1)*jacobian(0,1) + jacobian(1,1)*jacobian(1,1));
			dummy = 1.0f;
			saturation = 0.5;

			alpha = (acos(dummy) * 180.0f / PI) + m_nColorOffset; //h

			if (alpha > 360.0) alpha = fmod(alpha, 360.0f);

			HSVtoRGB(&pColor[0], &pColor[1], &pColor[2], alpha, saturation, val);
			pColor += 3;
		}
	}
}

void CFlowIllustratorRenderView::renderIsoLines(const CAmiraVectorField2D *pVectorField)
{
	renderVorticity(pVectorField);

	int extX = m_pVortField->GetExtentX();
	int extY = m_pVortField->GetExtentY();

	CScalarField2D *pDummyField = m_pVortField;

	CPointf ptFrom, ptTo;
	CPointf special1, special2;
	bool bSpecial = false;


	m_IsoLineMngr.RemoveAll();

	for (int y = 0; y < extY-1; y++) 
	{
		for (int x = 0; x < extX-1; x++) 
		{			
			float val1 = pDummyField->GetAt(x,y);
			float val2 = pDummyField->GetAt(x+1,y);
			float val3 = pDummyField->GetAt(x+1,y+1);
			float val4 = pDummyField->GetAt(x,y+1);
			
			DWORD ms = 0;

			if (val1 > m_fIsoVal)
				ms |= 0x00000001;

			if (val2 > m_fIsoVal)
				ms |= 0x00000002;

			if (val3 > m_fIsoVal)
				ms |= 0x00000004;

			if (val4 > m_fIsoVal)
				ms |= 0x00000008;

			switch(ms)
			{
				case 0x00000001:
				case 0x0000000E:
					ptFrom	= pDummyField->GetDomainCoordinates(x + 0.5f, static_cast<float>(y));
					ptTo	= pDummyField->GetDomainCoordinates(static_cast<float>(x), y + .5f );
				break;
				case 0x00000002:
				case 0x0000000D:
					ptFrom	= pDummyField->GetDomainCoordinates(x + 0.5f, static_cast<float>(y));
					ptTo	= pDummyField->GetDomainCoordinates(x + 1.0f, y + 0.5f);
				break;
				case 0x00000003:
				case 0x0000000C:
					ptFrom	= pDummyField->GetDomainCoordinates(static_cast<float>(x), y + 0.5f);
					ptTo	= pDummyField->GetDomainCoordinates(x + 1.0f, y + .5f);
				break;
				case 0x00000004:
				case 0x0000000B:
					ptFrom	= pDummyField->GetDomainCoordinates(x + 1.0f, y + 0.5f);
					ptTo	= pDummyField->GetDomainCoordinates(x + 0.5f, y + 1.0f);
				break;
				case 0x00000005:
					ptFrom		= pDummyField->GetDomainCoordinates(x + .5f, static_cast<float>(y));
					ptTo		= pDummyField->GetDomainCoordinates(x + 1.0f, y + .5f);
					special1	= pDummyField->GetDomainCoordinates(static_cast<float>(x), y+.5f);
					special2	= pDummyField->GetDomainCoordinates(x + .5f, y + 1.0f);
					bSpecial	= true;
				break;
				case 0x00000006:
				case 0x00000009:
					ptFrom	= pDummyField->GetDomainCoordinates(x + 0.5f, static_cast<float>(y));
					ptTo	= pDummyField->GetDomainCoordinates(x + 0.5f, y + 1.0f);
				break;
				case 0x00000007:
				case 0x00000008:
					ptFrom	= pDummyField->GetDomainCoordinates(static_cast<float>(x), y + 0.5f);
					ptTo	= pDummyField->GetDomainCoordinates(x + 0.5f, y + 1.0f);
				break;
				case 0x0000000A:
					ptFrom		= pDummyField->GetDomainCoordinates(static_cast<float>(x), y + .5f);
					ptTo		= pDummyField->GetDomainCoordinates(x + 0.5f, static_cast<float>(y));
					special1	= pDummyField->GetDomainCoordinates(x + 0.5f, y + 1.0f);
					special2	= pDummyField->GetDomainCoordinates(x + 1.0f, y + 0.5f);
					bSpecial	= true;
				break;

				default:	//0x00 && 0x0F
					continue;
					break;
			}

			m_IsoLineMngr.Add(new CLine(ptFrom, ptTo, m_NewObjectColor));

			if (bSpecial) {
				m_IsoLineMngr.Add(new CLine(special1, special2, m_NewObjectColor));
				bSpecial = false;
			}
		}
	}
}

void CFlowIllustratorRenderView::renderLIC(const CAmiraVectorField2D* pVectorField)
{
	CScalarField2D *pNoiseTex= _generateRandomNoiseTexture(m_rcViewPort); 
	CScalarField2D LicTex(m_rcViewPort, m_nLicWidth, m_nLicHeight);
	LicTex.Zero();

	float fPixelSize = min (pVectorField->GetExtentX() / float(m_nLicWidth), pVectorField->GetExtentY() / float(m_nLicHeight)); //min(pt.x, pt.y);
	float minStep = 1e-2f*pVectorField->GetSamplesPerUnitX();
	if (fPixelSize < minStep) 
		fPixelSize = minStep;

	if (fPixelSize > .1f)
		fPixelSize = .1f;

	int nKSize			= m_nLicIntegrationLen;
	CPointf pos(0.0f, 0.0f);

	CStreamLine StreamF(pos, nKSize, 0, m_NewObjectColor, fPixelSize);
	CStreamLine StreamB(pos, nKSize, 0, m_NewObjectColor, fPixelSize);
	vector<CPointf> StreamLine;

	const unsigned int dimX = LicTex.GetExtentX();
	const unsigned int dimY = LicTex.GetExtentY();

	vector<int> visited(dimX * dimY, 0);

	for (unsigned int y = 0; y < dimY; y++) 
	{
		for (unsigned int x = 0; x < dimX; x++) 
		{
			int index = y*dimX + x;
			if (visited[index] > 0) continue;

			pos = pNoiseTex->GetDomainCoordinates(static_cast<float>(x), static_cast<float>(y));

			calcStreamLine(pos, &StreamF, true);
			calcStreamLine(pos, &StreamB, false);

			vector<CPointf> *pSLB = StreamB.GetDataPoints();
			vector<CPointf> *pSLF = StreamF.GetDataPoints();
			StreamLine.clear();

			if (StreamLine.capacity() < StreamF.Size() + StreamB.Size() - 1) StreamLine.reserve(StreamF.Size() + StreamB.Size() - 1);

			StreamLine.insert(StreamLine.begin(), pSLB->begin(), pSLB->end());
			StreamLine.insert(StreamLine.end(), pSLF->begin()+1, pSLF->end());

			int numSamples = static_cast<int>(StreamLine.size());

			if (numSamples < 2) { 
				visited[index]++;
				continue;
			}

			int f((nKSize) < numSamples ? (nKSize) : numSamples); //Forward

			// - init the kernel
			float sum = 0.0f;
			int nNum = 0;

			for(int c=0;c<f;c++)
			{
				sum += pNoiseTex->GetValue( StreamLine[c] );
			}
			nNum = f;
			
			for (int i=0; i < numSamples; i++)
			{
				for (int k = -nKSize; k <= nKSize; k+=(2*nKSize))
				{
					if (i+k > -1 &&  i+k < numSamples) 
					{
						if (k>0) {
							sum += pNoiseTex->GetValue(StreamLine[k+i]);
							nNum++;
						}
						else if (k<0) {
							sum -= pNoiseTex->GetValue(StreamLine[k+i]);
							nNum--;
						}
					}
				}

				CPointf PixelNode = LicTex.GetClosestSamplePos(StreamLine[i]);
				LicTex.SetAt( static_cast<int>(PixelNode.x), static_cast<int>(PixelNode.y), 
					LicTex.GetAt(static_cast<int>(PixelNode.x), static_cast<int>(PixelNode.y)) + sum/float(nNum));
				visited[ static_cast<int>(PixelNode.y*dimX+PixelNode.x) ]++;
			}
		}
	} 

	//Normalize
	for(unsigned int j=0;j<dimY;j++)
	{
		for(unsigned int i=0;i<dimX;i++)
		{
			int NumVisited = visited[j*dimX + i];
			if (NumVisited == 0) NumVisited = 1;
			float val = LicTex.GetAt(i, j) / (float)(NumVisited);
			if (val > 1.0f) val = 1.0f;
			LicTex.SetAt(i, j, val);
		}
	}

	EnhanceContrast(LicTex);

	float *pColor = &m_pColorBuffer[0];

	for (int y = m_yMin; y < (m_yExtent+m_yMin); y++) 
	{
		for (int x = m_xMin; x < (m_xExtent+m_xMin); x++) 
		{
			pos = ScreenToDomain(CPoint(x, y));
			pColor[0] = pColor[1] = pColor[2] = LicTex.GetValue(pos.x, pos.y);
			pColor += 3;
		}
	}
}

#define HISTO_SIZE 256
void CFlowIllustratorRenderView::EnhanceContrast(CScalarField2D &texture)
{
	//Calc the histogram;

	int histo[HISTO_SIZE];
	int histoCumulative[HISTO_SIZE];
	memset(histo, 0, HISTO_SIZE * sizeof(int));

	//min and max color values
	int min(HISTO_SIZE);
	int max(0);

	int extX(texture.GetExtentX());
	int extY(texture.GetExtentY());
	int numPxl = extX*extY;

	for (int j=0; j<extY; j++) {
		for (int i=0; i<extX; i++) {
			int indx =  static_cast<int>(texture.GetAt(i,j) * (HISTO_SIZE-1));
			histo[ indx ] ++;
			if (indx < min) min = indx;
			else if (indx > max) max = indx;
		}
	}

	//compute cumulative histogram
	histoCumulative[0] = histo[0];
	for (int i=0; i < HISTO_SIZE; i++) {
		histoCumulative[i] = histoCumulative[i-1]+histo[i];
	}


	//Loop over the image
	for (int j=0; j<extY; j++) {
		for (int i=0; i<extX; i++) {
			int indx =  static_cast<int>(texture.GetAt(i,j) * (HISTO_SIZE-1));

			//Compute the output pixel
			texture.SetAt(i,j, (texture.GetAt(i,j)+histoCumulative[indx]/float(numPxl))/2.0f);
		}
	}
}

CScalarField2D* CFlowIllustratorRenderView::_generateRandomNoiseTexture(const CRectF &rcDomain)
{
	if (!m_bLICNoiseTexValid)
	{
		if (m_pLICNoiseTex)
			delete m_pLICNoiseTex;

		srand(m_nLicSeed);

		m_pLICNoiseTex = new CScalarField2D(rcDomain, m_nLicWidth, m_nLicHeight); //For now

		for (int y = 0; y < m_nLicHeight; y++) 
		{
			for (int x = 0; x < m_nLicWidth; x++) 
			{
				m_pLICNoiseTex->SetAt(x,y, (rand()%255)/255.0f);
			}
		}

		m_bLICNoiseTexValid = TRUE;
	}

	return m_pLICNoiseTex;
}

void CFlowIllustratorRenderView::renderVectorField()
{
	if (m_pDisplayFunc)
	{
		CFlowIllustratorDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc) {
			return;
		}

		const CAmiraVectorField2D *pVecField = pDoc->GetVectorfield();

		if (!pVecField)
			return;

		(this->*m_pDisplayFunc)(pVecField);
	}
}

CPointf CFlowIllustratorRenderView::ScreenToDomain(const CPoint &point)
{
	return ScreenToDomainf(CPointf( static_cast<float>(point.x), static_cast<float>(point.y) ));
}

CPointf CFlowIllustratorRenderView::ScreenToDomainf(const CPointf &point)
{
	CFlowIllustratorDoc *pDoc = GetDocument();

	if (pDoc)
	{
		const CVectorField2D *pVecField = pDoc->GetVectorfield();

		if (pVecField)
		{
			return CPointf(	m_rcViewPort.m_Min.x + ((point.x - m_xMin)/(m_xExtent-1) * m_rcViewPort.getWidth()) ,
							m_rcViewPort.m_Min.y + ((point.y - m_yMin)/(m_yExtent-1) * m_rcViewPort.getHeight()) );
		}
	}

	
	return CPointf();
}

CPointf CFlowIllustratorRenderView::ScreenToGL(const CPoint &point)const
{
	//Need to flip y-axis. OpenGL ornderliigin is set to bottom left instead of top left
	return CPointf( static_cast<float>(point.x), 
					static_cast<float>(m_oldRect.Height() - point.y) );
}

CRectF CFlowIllustratorRenderView::ScreenToGL(const CRect &rect) const
{
	CRectF retVal (	ScreenToGL(rect.TopLeft()), 
					ScreenToGL(rect.BottomRight()));

	retVal.Normalize();

	return retVal;
}

BOOL CFlowIllustratorRenderView::IsPointInsideDomain(const CPointf& point)
{
	if (m_rcDomain.IsEmptyRect()) return false;
	return m_rcDomain.PtInRect(point.x, point.y);
}

int CFlowIllustratorRenderView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	Init();

	bool bVert = m_ShaderMngr.LoadShader("./shaders/vertexshader_default.glsl", GL_VERTEX_SHADER, "VertexShdr");
	bool bFrag = m_ShaderMngr.LoadShader("./shaders/fragmentshader_default.glsl", GL_FRAGMENT_SHADER, "FragmentShdr");
		
	if (!bVert || !bFrag ) {
		//AfxMessageBox(_T("Error while loading shaders"));
	} else {
		m_ShaderMngr.UseShader("VertexShdr");
		m_ShaderMngr.UseShader("FragmentShdr");
	}

	return 0;
}


void CFlowIllustratorRenderView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if(cy > 0)
	{
		m_oldRect.right = cx;
		m_oldRect.bottom = cy;

		CalcExtents();
		AdjustViewport();

		m_bLICNoiseTexValid = FALSE;
	}
}

void CFlowIllustratorRenderView::OnDestroy()
{
	CView::OnDestroy();

	::wglMakeCurrent(NULL,  NULL);

	if (m_hRc)
		::wglDeleteContext(m_hRc);

	if (m_pOldPalette)
		m_pDC->SelectPalette(m_pOldPalette, FALSE);

	if (m_pDC)
		delete m_pDC;

	if (m_pColorBuffer) 
		delete [] m_pColorBuffer;

	if (m_pScreenCoordinateBuffer)
		delete [] m_pScreenCoordinateBuffer;

	if (m_pVortField)
		delete m_pVortField;

	if (m_pVortFieldAbs)
		delete m_pVortFieldAbs;

	if (m_pLICNoiseTex)
		delete m_pLICNoiseTex;
}

BOOL CFlowIllustratorRenderView::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CFlowIllustratorRenderView::calcStreamLine(const CPointf& point, CStreamLine *pStreamLine, bool bForward)
{
	CFlowIllustratorDoc *pDoc = GetDocument();

	if (pDoc)
	{
		const CAmiraVectorField2D *pVecField = pDoc->GetVectorfield();

		if (pVecField)
		{
			vector<CPointf>* pData = pStreamLine->GetDataPoints();
			pData->clear();
			CVector3D vec(point.x, point.y, 0);
			pVecField->integrateRK4( vec  , static_cast<int>(pStreamLine->GetMaxIntegrationLen()), pStreamLine->GetStepSize(), bForward,  pData, m_rcViewPort);
			pStreamLine->SetOrigin( point );
		}
	}
}

BOOL CFlowIllustratorRenderView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}


