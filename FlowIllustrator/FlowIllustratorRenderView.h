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
#include "FlowIllustratorDoc.h"
#include "DrawingObjectMngr.h"
#include "ShaderMngr.h"
#include "StreamLine.h"


const float COORDINATE_AXIS_WIDTH = 30.0f;	/**< Width of the vertical coordinate axis in pixel. */
const float COORDINATE_AXIS_HEIGHT = 45.0f;	/**< Height ig the horizontal coordinate axis in pixel. */

/**
 *	This class derived from CView is responsible for rendering vector fields.
 *	It takes care of initializing WGL and managing the necessary rendering contexts.
 *	Further, it provides various functions to render voctor fields, so-called drawing fnctions.
 *
 */
class CFlowIllustratorRenderView : public CView
{
	DECLARE_DYNCREATE(CFlowIllustratorRenderView)

	// Attributes
protected:
	void (CFlowIllustratorRenderView::*m_pDisplayFunc)(const CAmiraVectorField2D*); /**< Pointer to the currend display function. */
	std::vector< std::pair<CString, void (CFlowIllustratorRenderView::*)(const CAmiraVectorField2D*)> > m_DisplayFunctions;	/**< Vector containling pairs of CString objects, holding function names, and pointers ot the corresponding display function. This vector holds pointers to all available display functions. */

	int			m_nDisplayFuncID;			/**< ID/index of the currently selected display function. */

	CRectF		m_rcDomain;					/**< RectF, specifying the entire domain of the currently opened data set.*/
	CRectF		m_rcViewPort;				/**< RectF, specifying the current viewport. Can be smaller or equal than m_rcDomain. */
	CPointf		m_ptViewportCenter;			/**< CPointf, specifying the center of the view port. Used to zoom near the current mouse location.*/
	int			m_xExtent;					/**< Extent of the canvas in pixels in X-direction */
	int			m_yExtent;					/**< Extent of the canvas in pixels in Y-direction */
	int			m_xMin;						/**< Offset in pixels in X-direction. Used to center the displayed image horizontally. */
	int			m_yMin;						/**< Offset in pixels in Y-direction. Used to center the displayed image vertically. */

	float		m_xyRatio;					/**< Width/height ratio of the extents (m_xExtent/m_yExtent). */
	float		m_fPixelUnitRatioX;			/**< Pixels per domain unit in X-direction. */
	float		m_fPixelUnitRatioY;			/**< Pixels per domain unit in Y-direction. */

	float		m_fIsoVal;					/**< Current iso-value to render iso contours. */
	BOOL		m_bDrawCoordinateAxes;		/**< Flag, that determines if coordinate axes are drawn around the domain. */

	CScalarField2D *m_pVortField;			/**< Pointer to the vorticity field, corresponding to the vector field of the current frame. */
	CScalarField2D *m_pVortFieldAbs;		/**< Pointer to the absolute vorticity field, corresponding to the vector field of the current frame. */
	CScalarField2D *m_pVectorMagnitudeField;
	BOOL			m_bVorticityValid;		/**< If true, the vorticity does not need to be re-computed. */
	BOOL			m_bVectorMagnitudeValid;

	int			 m_nColorOffset;			/**< Manipulator for the base color for drawing the vector field. */
	float		*m_pScreenCoordinateBuffer; /**< Buffer, which contains coordinates for each pixel on the canvas. */
	float		*m_pColorBuffer;			/**< Buffer, which contains the corresponding color values to the coordinates, sotred in m_pScreenCoordinateBuffer. */
	
	GLint		 m_nMSAASamples;

	floatColor	m_NewObjectColor;			/**< A floatColor, containing the color of a new CDrawingObject to be created. */

	//needed for WGL
protected:
	CPalette			m_cPalette;			/**< Color palette, needed by m_pDC.*/
	CPalette		   *m_pOldPalette;		/**< Buffer for m_cPalette.*/
	CRect				m_oldRect;			/**< Screen rectangle.*/
	CClientDC		   *m_pDC;				/**< Windows device context to create the OpenGl context.*/

	CShaderMngr			m_ShaderMngr;		/**< Does the nasty shader management stuff. */
	CDrawingObjectMngr *m_pDrawObjMngr;		/**< Pointer to  the CDrawingObjectMngr, owned by CFlowIllustratorDocument. */
	CDrawingObjectMngr	m_IsoLineMngr;		/**< Special CDrawingObjectMngr, who manages and keeps track of per-frame iso lines. */
	
	HGLRC					m_hRc;				/**< Handle to the OpenGL render context. */
	int						m_nPixelFormatMSAA;	/**< Index that identifies the pixel format to set. The various pixel formats supported by a device context are identified by one-based indexes. */

	//LIC
protected:
	int				m_nLicWidth;			/**< Width of the LIC noise texture in pixel. */
	int				m_nLicHeight;			/**< Height of the LIC noise texture in pixel. */
	int				m_nLicSeed;				/**< Seed used for the LIC random number generator.  */
	int				m_nLicIntegrationLen;	/**< Number of LIC integration steps (Kernel size). */
	BOOL			m_bLICNoiseTexValid;	/**< If TRUE, the NOISE texture is valid (no need to generate a new one each time). */
	CScalarField2D *m_pLICNoiseTex;			/**< Pointer to the noise texture. */

protected:
	CFlowIllustratorRenderView();           /**<	Protected constructor used by dynamic creation */
	virtual ~CFlowIllustratorRenderView();	/**<	Destroy this CFlowIllustratorRenderView and all its data. */ 


protected:
	/**
	 *	Initializes OpenGL (WGL), GLUT and GLEW.bSetupPixelFormat()
	 */
	void Init();

	/**
	 *	Setup the pixel format of this CFlowIllustratorRenderView's device context.
	 *	This function tries to setup a pixel format that supports MSAA, in order to
	 *	achieve a smooth output image.
	 *
	 *	@return TRUE, if successful, otherwise false.
	 *
	 *	@remarks	If this function fails, the pixel format could not be set correctly.
	 *				This might lead display errors.
	 */
	BOOL bSetupPixelFormat();

	/**
	 *	Helper function, called by bSetupPixelFormat().
	 *
	 *	@remarks	If this function fails, the pixel format could not be set correctly.
	 *				This might lead display errors.
	 */
	BOOL bSetupDummyPixelFormat(HDC hDC);

	/**
	 *	Set the orthographic projection to the specified screen region.
	 *
	 *	@param left		The coordinates for the left vertical clipping plane.
	 *	@param right	The coordinates for the right vertical clipping plane.
	 *	@param bottom	The coordinates for the bottom horizontal clipping plane.
	 *	@param top		The coordinates for the top horizontal clipping plans.
	 *	@param zNear	The distances to the nearer depth clipping plane. This distance is negative if the plane is to be behind the viewer.
	 *	@param zFar		The distances to the farther depth clipping plane. This distance is negative if the plane is to be behind the viewer.
	 */
	void SetProjection(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear = 0.0, GLdouble zFar = -1.0);

public:
	/**
	 *	Retrieves the names of all registered drawing functions.
	 *
	 *	@param arrNames Reference to a CStringArray, to recieve the function names.
	 */
	void GetDisplayFunctionNames(CStringArray &arrNames) const;

	/**
	 *	Retrieve the ID of the current display function.
	 *
	 *	@return The ID as int.
	 */
	int GetDisplayFunctionID() const;

	/**
	 *	Set the current display function.
	 *
	 *	@param nID ID of the new display function.
	 *
	 *	The supplied nID must be a valid index of a registerd display function, stored in m_DisplayFunctions.
	 */
	void SetDisplayFunction(int nID);

	//LIC
public:
	/**
	 *	Retrieve the width of the LIC texture.
	 *
	 *	@return The width of the lic textire in pixel.
	 */
	int GetLICWidth() const;

	/**
	 *	Retrieve the height of the LIC texture.
	 *
	 *	@return The height of the lic textire in pixel.
	 */
	int GetLICHeight() const;

	/**
	 *	Retrieve the half size of the box filter used by LIC.
	 *
	 *	@return The half size of the filter kernel.
	 */
	int GetLICHalfSize() const;

	/**
	 *	Retrieve the seed, used by the random number generator (RNG), used to create the LIC noise texture.
	 *
	 *	@return The seed as int.
	 */
	int GetLICSeed() const; 

	/**
	 *	Set the width of the LIC texture.
	 *
	 *	@param nWidth The new width of the LIC texture in pixel.
	 */
	void SetLICWidth(int nWidth);

	/**
	 *	Set the height of the LIC texture.
	 *
	 *	@param nHeight The new height of the lic texture in pixel.
	 */
	void SetLICHeight(int nHeight);

	/**
	 *	Set the half size of the box filter used by LIC.
	 *
	 *	@param nHalfSize The new half size of the LIC box filter.
	 */
	void SetLICHalfSize(int nHalfSize);

	/**
	 *	Set the seed, used by the random number generator(RNG), used to create the LIC noise texture.
	 *
	 *	@param nSeed The new seed as int.
	 */
	void SetLICSeed(int nSeed); 

	/**
	 *	Set the vorticity iso value, used to render iso-lines.
	 *
	 *	@param IsoVal The new iso value as float.
	 */
	void SetIsoValue(float IsoVal);

	/**
	 *	Reset the viewport to display the entire domain of the currently opened vector field.
	 */
	void ResetViewport();

	/**
	 *	Set the status of the displayed vector field to dirty, requiring the vector field to be 
	 *	renderd, the next time drawScene().
	 */
	void Dirty();

protected:
	/**
	 *	Retrieves the matching vorticity field of the vector field's currently displayed time step.
	 *
	 *	@return TURE, if the vorticity field could be retrieved successfully, otherwise false.
	 *
	 *	@remarks	The vorticity field can be accessed via m_pVortField. 
	 *				Additionally, the absolute vorticity field can be accessed via m_pVortFieldAbs.
	 *				Moreover, m_bVorticityValid indicates if m_pVortField and m_pVortFieldAbs point to 
	 *				valid vorticity fields.
	 */
	BOOL AcquireVorticityField();

	BOOL AcquireVectorMagnitudeField();

	/**
	 *	Adjust the viewport, according to the current zoom factor. 
	 *
	 *	@param bCenterOnMousePos If TRUE, the current mouse location is used as the new viewport center.
	 *	
	 */	
	void AdjustViewport(BOOL bCenterOnMousePos=FALSE);

	/**
	 *	Allocates and initialises the color and coordinate buffer, after the viewport or the
	 *	window size have changed.
	 */
	void AllocateScreenBuffers();

	/**
	 *	Calculates the extent of the viewport in X and Y direction, with regard to 
	 *	the current zoom factor.
	 */
	void CalcExtents(void);

public:
	/**
	 *	Set the range of valid iso-values in the iso-value slider control.
	 *
	 *	@param fMin Miminum iso-value
	 *	@param fMax Maximum iso-value
	 */
	void SetMinMaxIsoValue(float fMin, float fMax);

	/**
	 *	Set the color offset, used by drawing functions.
	 *
	 *	@param nBaseColor Color offset, as angle in degree.
	 *
	 *	@remarks Drawing functions use the HSV color space to generate output colors.
	 */
	void SetColrOffset(int nBaseColor);

	/**
	 *	Retrieve the rectangle, describing the current viewport.
	 *
	 *	@return RectF, describing the curretn viewport.
	 */
	CRectF GetViewportRect() const {
		return m_rcViewPort;
	}

protected:
	/**
	 *	Convert a given point from screen coordinates to domain coordinates.
	 *
	 *	@param point A reference to a CPoint in screen coordinates to be converted.
	 *
	 *	@return A CPointf, with the corresponding domain coordinates.
	 */
	CPointf ScreenToDomain(const CPoint &point);

	/**
	 *	Convert a given point from screen coordinates to domain coordinates.
	 *
	 *	@param point A reference to a CPointf in screen coordinates to be converted.
	 *
	 *	@return A CPointf, with the corresponding domain coordinates.
	 */
	CPointf ScreenToDomainf(const CPointf &point);

	/**
	 *	Convert a given point from screen coordinates to OpenGl coordinates.
	 *
	 *	@param point A reference to a CPoint in screen coordinates to be converted.
	 *
	 *	@return A CPointf, with the corresponding OpenGL coordinates.
	 *
	 *	@remarks	In Windows, the coordinate origin is in the upper-left corner of the screen,
	 *				whereas in OpenGL, the origin is located in the lower left corner of the screen.
	 */
	CPointf ScreenToGL(const CPoint &point) const;

	/**
	 *	Convert a given rectangle from screen coordinates to OpenGl coordinates.
	 *
	 *	@param rect A reference to a CRect in screen coordinates to be converted.
	 *
	 *	@return A CRectF, with the corresponding OpenGL coordinates.
	 *
	 *	@remarks	In Windows, the coordinate origin is in the upper-left corner of the screen,
	 *				whereas in OpenGL, the origin is located in the lower left corner of the screen.
	 */
	CRectF ScreenToGL(const CRect &rect) const;

	/**
	 *	Determine, if a given point is inside the domain of the currently opended vector field.
	 *
	 *	@param point Reference to a point in domain coordinates.
	 *
	 *	@return TRUE, if the point is inside the domain, otherwise FALSE.
	 */
	BOOL IsPointInsideDomain(const CPointf& point);

	//Render functions
public:

	/**
	 *	This is the main drawing function of FlowIllustrator.
	 *	It takes care of clearing the screen, displaying the vector field
	 *	with the currently selected drawing function and displaying all flow templates.
	 *
	 *	After drawing all flow templates, this function calls CFlowIllustratorRenderView::OnDrawScene(),
	 *	to allow for custom draw operations in derived classes.
	 */
	void DrawScene(void);	//Main rendering routine

protected:
	/**
	 *	Draws the coordinate axes on the left and bottom of the viewport.
	 *	This function is only called, if m_bDrawCoordinateAxes is set to TRUE.
	 */
	void DrawCoordinateAxes(void);

	/**
	 *	Virtual function to allows custom draw operations in derived classes.
	 */
	virtual void OnDrawScene() {};

	/**
	 *	This function performs the actual rendering of the current vector field,
	 *	by invoking the currently selected display function.
	 *
	 *	@remarks	This function is only called, if the dirty-flag in this 
	 *				CFlowIllustratorRenderView's CFlowIllustratorDoc is set to TRUE.
	 */
	void renderVectorField();

	/**
	 *	Renders the supplied vector field according to the color-coding scheme,
	 *	described in [Data Visualization Principles and Practice, Alexandru Telea].
	 *
	 *	@param pVectorField Pointer to the vector field to be rendered.
	 */
	void renderColorCoding(const CAmiraVectorField2D *pVectorField);

	/**
	 *	Renders the current vector field by encoding vector magnitude with color values.
	 *
	 *	@param pVectorField Pointer to the vector field to be rendered.
	 */
	void renderVectorMagnitude(const CAmiraVectorField2D *pVectorField);

	/**
	 *	Calculates the vorticity field of the vector field's current time step.
	 *	The vorticity values are then normalized and used to choose a color
	 *	from the HSV color space. To achieve a stronger visual separation
	 *	of clockwise and counter clockwise rotating vortices, an offset of
	 *	+90 and -90 degrees is added to the color angle of counter clockwise
	 *	and clockwise rotating vortices, respectively.
	 *
	 *	@param pVectorField Pointer to the vector field to be rendered.
	 */
	void renderVorticity(const CAmiraVectorField2D *pVectorField);

	void _renderScalarField(CScalarField2D *pSrc);

	/**
	 *	Calculates the partial derivative in X-direction and uses
	 *	the magnitude of the derivative to choose a color angle from the HSV color space.
	 *
	 *	@param pVectorField Pointer to the vector field to be rendered.
	 */
	void renderDerivatives_x(const CAmiraVectorField2D *pVectorField);

	/**
	 *	Calculates the partial derivative in Y-direction and uses
	 *	the magnitude of the derivative to choose a color angle from the HSV color space.
	 *
	 *	@param pVectorField Pointer to the vector field to be rendered.
	 */
	void renderDerivatives_y(const CAmiraVectorField2D *pVectorField);

	/**
	 *	Displays the supplied vector field as vorticity rendering and 
	 *	calculates vorticity iso lines, based on the currently set iso-value.
	 *
	 *	@param pVectorField Pointer to the vector field to be rendered.
	 *
	 *	@see renderVorticity()
	 *	@see m_fIsoVal
	 */
	void renderIsoLines(const CAmiraVectorField2D *pVectorField);

	/**
	 *	Performes Line Integral Convolution (LIC) to render the supplied vector field.
	 *
	 *	@param pVectorField Pointer to the vector field to be rendered.
	 *
	 *	@remarks	This function uses the fast LIC algorithm, proposed by Stalling and Hege.			
	 */
	void renderLIC(const CAmiraVectorField2D *pVectorField);

	/**
	 *	Enhances the contrast of a monochrome image, using histogram equalisation.
	 *
	 *	@param 	texture A reference to a CScalarField2D representing the monochrome image.
	 */
	void EnhanceContrast(CScalarField2D &texture);

	/**
	 *	Generate arandom noise texture, used by LIC.
	 *
	 *	@param rcDomain Reference to a CRectF, describing the domain, for which the 
	 *					noise texture is created.
	 *
	 *	@return A pointer to a CScalarField2D, representing the noise texture.
	 *
	 *	@remarks	This function uses m_nLicSeed as seed to initialize the random number generator (RNG)
	 *				in order to create the noise texture.
	 *				A pointer to the noise texture is stored in m_pLICNoiseTex, and managed by this CFlowIllustratorRenderView.
	 */
	CScalarField2D* _generateRandomNoiseTexture(const CRectF &rcDomain);


	/**
	 *	Calculate a stream line, based on the current vector field, originating at the specified point.
	 *	
	 *	@param point		Reference to a CPointf, specifying the origin of the stream line.
	 *	@param pStreamLine	Pointer to a CStreamLine object, to receive the integrated data.
	 *	@param bForward		Flag, that specifies, if forward or backward integration is to be performed.
	 *
	 *	@remarks	The stream line integration is started at the current time step of the vector field,
	 *				Stored in the CFlowIllustratorDoc of this CFlowIllustratorRenderView. 
	 */
	void calcStreamLine(const CPointf& point, CStreamLine *pStreamLine, bool bForward);

public:
	CFlowIllustratorDoc* GetDocument() const;

	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

#ifndef _DEBUG  // debug version in FlowIllustratorRenderView.cpp
inline CFlowIllustratorDoc* CFlowIllustratorRenderView::GetDocument() const
   { return reinterpret_cast<CFlowIllustratorDoc*>(m_pDocument); }
#endif
