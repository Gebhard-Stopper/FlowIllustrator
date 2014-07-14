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
#include <string>
#include "DrawingObjectDataTypes.h"

/**
 *	Virtual base class for all template objects.
 *
 *	All classes derived from CDrawingObject must implement the following virtual functions: <BR>
 *	Draw()
 *	Duplicate()
 *	<BR>
 *	CDrawingObject implements an extensible data model, wich allows to dynamically append parameter values.
 *	Therefore, it stores its parameters in a mapping, of the parameters integral name, defined in the
 *	DrawinObjectParamName enumeration, and the associated value, stored as a CSimpleVaiant.
 *	To Retrieve and modify the parameters of a CDrawingObject, it provides the GetParams() and SetParams() methods
 *	respectively.
 *	<BR>
 *	GetParams() retrieves a copy of all parameters of a CDrawingObject, adn returns them in a CDrawingObjectParams object.
 *	SetParams on the other hand receives a reference to a CDrawingObjectParams object, and iterates over every parameter name / value pair.
 *	For each pair it the calls the virtual method SetParam(). SetParam() typically calls a specialized Set() method to apply a new parameter value.
 *	Classes derived from CDrawingObject typically overwrite SetParam() to adapt its behaviour to new, specialized parameter values.
 *	<BR>
 *	The toString() method generates a textual representation of a CDrawingObject and all of its parameters.
 *	By oberloading 	OnToString() additional information can be appended to the output produced by toString().
 *	Overloading FinalizeToString() allows to include additional information, related, but not belonging to an object.
 *	FinalizeToString() can be used, to e.g. to retrieve a string representation of subordinated objects. 
*/
class CDrawingObject
{
protected:
	shared_ptr<CDrawingObjectParams>	m_pParams;	/**< Mapping that contains all parameters. */
	bool		m_bSelected;	/**< Indicates if this CDrawingObject is currently selected. */
	vector<shared_ptr<CDrawingObject>> m_children;	/**< Vector, containing pointers to all child CDrawingObjects of this CDrawingObject */

private:
	CDrawingObject*	m_pParent;	/**< Pointer to this CDrawingObject's parent CDrawingObject. */

public:
	/**
	 *	Construct a new CDrawingObject object.
	 *
	 *	@param BBox The Rectangular bounding box of the object.
	 *	@param type Describes the type of a CDrawingObject. See DrawingObjectType.
	 *	@param pColor Pointer to a floatColor object.
	 *
	 */
	CDrawingObject(const CRectF& BBox, DrawingObjectType type, const floatColor& color);

	/**
	 *	Destroy a Drawing object and its associated data.
	 *	Derived classes may need to overwrite this function in order to delete additional data.
	 */
	virtual ~CDrawingObject(void) ;

protected:
	/**
	 *	Construct a new CDrawingObject object.
	 *	This is a specialised verion of the constructor, only available to derived classes.
	 *
	 *	@param type Describes the type of a CDrawingObject. See DrawingObjectType.
	 */
	CDrawingObject(DrawingObjectType type);

	/**
	 *	Internal helper function to initialize some CDrawingObject paraemers with default values.
	 */
	void _init(DrawingObjectType type);

public:
	/**
	 *	Set/reset the selected state of a CDrawingObject.
	 *
	 *	@param bSelected The new selection state.
	 **/
	virtual __inline void Select(bool bSelected) { 
		m_bSelected = bSelected; 
		for (auto iter = m_children.begin(); iter != m_children.end(); ++iter) {
			(*iter)->Select(m_bSelected);
		}
	}

	/**
	 *	Retrieves the current selection state.
	 *
	 *	@return true, if selected, otherwise false.
	 */
	virtual __inline bool isSelected() const { return m_bSelected; }

	/**
	 *	Retrieve acopy of the bounding rectangle of this CDrawingObject.
	 *
	 *	@return CRectF, describing the bounding rectangle of this CDrawingObject.
	 */
	virtual __inline CRectF GetRect() const { 
		CRectF r( 
			GetCenter(), 
			GetWidth(), 
			GetHeight() );
		r.Normalize();
		return r;
	}

	/**
	 *	Set the bounding retangle to the specified CRectF.
	 *
	 *	@param rect CRectF describing the new bounding rectangle.
	 *
	 *	@remarks	This function invokes _OnParamsChanged(), to perform possibly necessary 
	 *				recalculations of a CDrawingObject's related parameters.
	 */
	virtual __inline void SetRect(const CRectF& rect) { 
		SetCenter( CPointf::fromVector2D(rect.GetCenter()) );
		setWidth( rect.getWidth() );
		setHeight( rect.getHeight() );

		_OnParamsChanged();
	}

	/**
	 *	Retrieve the current rotation angle.
	 *
	 *	@remarks	The rotation angle in degrees as float.
	 */
	virtual __inline float GetRotation() const { 
		return m_pParams->GetValueFloat(DOP_ROTATION); 
	}

	/**
	 *	Set the current rotation angle.
	 *
	 *	@param	deg rotation angle in degrees as float.
	 */
	virtual __inline void SetRotation(float deg) { 
		m_pParams->SetValue(DOP_ROTATION, (deg < 0)? 360+fmod(deg, 360) : fmod(deg, 360));  
		_OnParamsChanged();
	}
	
	/**
	 *	Retrieve this CDrawingObject's thickness.
	 *
	 *	@return This CDrawingObject's thickness.
	 *
	 *	@remarks	The thickness of an object can have different meanings, depending on an object type.
					It can e.g. be the line thickness of a CStreamLine object.
	 */
	virtual __inline float GetThickness() const { return m_pParams->GetValueFloat(DOP_THICKNESS); }	

	/**
	 *	Set the thickness of this CDrawingObject.
	 *
	 *	@param fThickness The new thickness of this CDrawingObject.
	 *
	 *	@remarks	The thickness of an object can have different meanings, depending on an object type.
	 *				It can e.g. be the line thickness of a CStreamLine object, or the particle size. In this case, the thickness
	 *				influences the OpenGL parameters GL_LINE_WIDTH and GL_POINT_SIZE. Depending on the OpenGL implementation,
	 *				acceptable values usually range from [0..15].
	 */
	virtual __inline void SetThickness(float fThickness) { m_pParams->SetValue(DOP_THICKNESS, fThickness); }

	/**
	 *	Retrieve the actual type of this CDrawingObject.
	 *
	 *	@return The object type, as defined in DrawingObjectType.
	 */
	virtual __inline DrawingObjectType GetType() const { 
		return static_cast<DrawingObjectType>(m_pParams->GetValueInt(DOP_TYPE)); 
	}

	/**
	 *	Set this CDrawingObject's color.
	 *
	 *	@param color floatColor describing the new red, green, blue and alpha values
	 *
	 *	@remarks	This function also generates a color, used to draw a CDrawingObject in the selected state.
					By default, this color only difers in having 50% of the alpha value of the original color.
					The selected color is stored in m_arrColorSel.
	 */
	virtual __inline void SetColor(const floatColor& color) {
		floatColor dummy = color;
		m_pParams->SetValue(DOP_COLOR, CSimpleVariant(color));

		dummy.alpha *= .5f;
		m_pParams->SetValue(DOP_COLOR_SEL, color);
	}

	/**
	 *	Retrieve a pointer to the currently set color.
	 *
	 *	@return Pointer to the floatColor describing the color of this CDrawingObject. 
	 */
	virtual __inline const floatColor& GetColor() const {
		return (m_bSelected)? m_pParams->GetValueFloatColor_r(DOP_COLOR_SEL) : m_pParams->GetValueFloatColor_r(DOP_COLOR);
		//return (m_bSelected)? const_cast<LPFLOATCOLOR>(&m_arrColorSel) : const_cast<LPFLOATCOLOR>(&m_arrColor);
	}

	/**
	 *	Retrieve a pointer to the currently set color.
	 *
	 *	@return A float pointer to the color of this CDrawingObject. 
	 */
	virtual __inline const float* GetColorGL() const {
		//return (m_bSelected)? reinterpret_cast<const float*>(&m_arrColorSel) : reinterpret_cast<const float*>(&m_arrColor);
		return const_cast<float*>(reinterpret_cast<const float*>(&m_pParams->GetValueFloatColor_r(m_bSelected? DOP_COLOR_SEL : DOP_COLOR )));
	}

	/**
	 *	Set the new halo color of this CStreamLine.
	 *
	 *	@param color The new halo color as reference to a floatColor.
	 */
	__inline void SetHaloColor(const floatColor &color) { 
		m_pParams->SetValue(DOP_HALOCOLOR, color); 
	}

	/**
	 *	Retrieve a reference to the floatColor variable that stores the value for the halo color of this 
	 *	CDrawingObject.
	 *
	 *	@return A reference to a floatColor.
	 */
	__inline const floatColor& GetHaloColor() const {
		return m_pParams->GetValueFloatColor_r(DOP_HALOCOLOR);
	}

#ifdef WIN32
	/**
	 *	Retrieve a pointer to the currently set color.
	 *
	 *	@return A COLORREF structure, compatible to Windows GDI, holding the RGB values of the color components of this CDrawingObject. 
	 *			The values are transfrmed from [0..1] to [0..255]. Returned value has the following structure: 00BBGGRR
	 *
	 *	@remarks This function is only available, if compiled, if  WIN32  is defined.
	 */
	virtual __inline const COLORREF GetColorWin32() const {
		const floatColor &col(GetColor());

		COLORREF color =	static_cast<int>(col.red*255) |
							static_cast<int>(col.green*255) << 8 | 
							static_cast<int>(col.blue*255) << 16;

		return color;
	}
#endif

	/**
	 *	Retrieve the center point of an object, based on the bounding rectangle.
	 *
	 *	@return The center as CPointf.
	 */
	virtual __inline CPointf GetCenter() const {
		return m_pParams->GetValuePointf(DOP_CENTER);
		//return CPointf::fromVector2D( m_rcBounds.GetCenter() );
	}

	/**
	 *	Set, if an object is to be drawn solid, or just as countour.
	 *
	 *	@param bSolid If true, the object is to be drawn solid.
	 */
	virtual __inline void IsSolid(bool bSolid) {
		m_pParams->SetValue(DOP_ISSOLID, bSolid);
	}

	/**
	 *	Retrieve, if an object is to be drawn solid, or just as countour.
	 *
	 *	@return If an object is to be drawn sold this function returns true, otherise false.
	 */
	virtual __inline bool IsSolid() const {
		return m_pParams->GetValueBool(DOP_ISSOLID);
	}

	/**
	 *	Set the center point of an object.
	 *
	 *	@param point The new center of this CDrawingObject.
	 *
	 *	@remarks	This function Maintains the extent of the bounding rectangle, and translates it such that
					point is its new center.
	 */
	virtual void SetCenter(const CPointf &point);

	/**
	 *	Copy all the the parameters of a CDrawingObject to this CDrawingObject.
	 *
	 *	@param pSrc A CDrawingObject whose parameters are to be copied.
	 */
	void CopyParams(const CDrawingObject *pSrc);

	/**
	 *	Retrieve if this object is drawn with or without a halo.
	 *
	 *	@return This function returns true, if this object is drawn with a halo, otherwise false.
	 *
	 *	@remarks Classes derived from CDrawingObject should regard this value, if they provide the option to be rendered with a halo. 
	 */
	virtual bool DrawHalo() const { return m_pParams->GetValueBool(DOP_HAS_HALO); }

	/**
	 *	Modify if this object is drawn with a halo.
	 *
	 *	@param bDrawHalo Set this value to ture, if this object shall be drawn with a halo, otherwise set it to false.
	 *
	 *	@remarks Classes derived from CDrawingObject should regard this value, if they provide the option to be rendered with a halo. 
	 */
	virtual void DrawHalo(bool bDrawHalo) { m_pParams->SetValue(DOP_HAS_HALO, bDrawHalo); }

#ifdef WIN32
	/**
	 *	Retrieve a textual representation of this CDrawingObject.
	 *	By default, this produces a SVG compatible representation of this object.
	 *
	 *	@param fLineWidthFactor Factor to downscale or upscale object sizes, to match the dimensions of the output format.
	 *
	 *	@return Textual representation of this object as CString.
	 *
	 *	@remarks This function internally calls GetParams() to retrieve all parameters of a CDrawingObject.
	 *			 Only the parameters obtained by GetParams() are appended to the output string by default.
	 */
	virtual CString toString(float fLineWidthFactor = 1.0f) const;

protected:
	/**
	 *	Helper function to retrieve a string describing the visual appearance of a CDrawingObject in SVG format.
	 *
	 *	@return Textual representation of the appearance of this object as CString.
	 */
	virtual CString getStyleString(float fLineWidthFactor) const;

	/**
	 *	This function is called by toString() to allow for additional parameters in derived classes.
	 *	It is called, after all default parameters have been appended to the output string, but before the
	 *	SVG object is closed.
	 *
	 *	@param	str The string geerated by toString().
		@param fLineWidthFactor The fLineWidthFactor passed to toString().
	 *	
	 */
	virtual void OnToString(CString &str, float fLineWidthFactor) const;

	/**
	 *	This function is called by toString() to allow for additional parameters in derived classes.
	 *	It is called, after all the SVG object is closed. 
	 *
	 *	@param	str The string geerated by toString().
	 *	@param fLineWidthFactor The fLineWidthFactor passed to toString().
	 *	
	 */
	virtual void FinalizeToString(CString &str, float fLineWidthFactor) const;

#else
	/**
	 *	Retrieve a textual representation of this CDrawingObject.
	 *	By default, this produces a SVG compatible representation of this object.
	 *
	 *	@param fLineWidthFactor Factor to downscale or upscale object sizes, to match the dimensions of the output format.
	 *
	 *	@return Textual representation of this object as std::string.
	 *
	 *	@remarks This function internally calls GetParams() to retrieve all parameters of a CDrawingObject.
	 *			 Only the parameters obtyned by GetParams() are appended to the output string by default.
	 */
	virtual std::string toString() const;
#endif

	/**
	 *	This function is called, to indicate that some important parameter(s) have changed.
	 *	Derivec classes can use this to e.g. update their geometry.
	 *	
	 */
	virtual void _OnParamsChanged() {};

	/**
	 *	Set the params of all child objects, matching the object type,
	 *	specified in params.
	 *
	 *	@params params Reference to a CDrawingObjectParams, containing parameter-value pairs to be set.
	 */
	virtual bool SetChildObjParams(const CDrawingObjectParams &params);

public:
	/**
	 *	Retrive all parameters of a CDrawingObject.
	 *	
	 *	@param params A refernece to a CDrawingObjectParams object to receive a copy of the parameters of this CDrawingObject.
	 *
	 *	@remarks	Derived classes can re-implemented this function, and ensure this way all important parameters
	 *				are appended to params.
	 **/
	virtual void GetParams(CDrawingObjectParams &params) const; 

	/**
	 *	Set one or more parameters to a CDrawingObject.
	 *
	 *	@param	params Reference to a CDrawingObjectParams, containing parameter-value pairs to be set.
	 *
	 *	@remarks	The parameters of a drawing object are only set, if the supplied
	 *				CDrawingObjectParams object has the same type as the CDrawingObject it is applied to.
	 *				Otherwise, this function tries to apply the parameters to subordinated CDrawingObjects,
	 *				if any, and returns. <BR>
	 *				This function calls setParam() to actually apply the parameter values.
	 *
	 *	@see SetParam()
	 */
	virtual bool SetParams(const CDrawingObjectParams &params);

	/**
	 *	Retrive all parameters of a CDrawingObject.
	 *
	 *	@return	A CDrawingObjectParams object, that contains all parameters of this CDrawingObject.
	 */
	virtual CDrawingObjectParams GetParams() const; 

	/**
	 *	The mein rendering function of a CDrawingObject.
	 *
	 *	@remarks This function must be implemented by derived classes. It is called by the main rendering function of CFlowIllustratorRenderView.
	 */
	virtual void Draw() = NULL;

	/**
	 *	Translate this CDrawingObject.
	 *
	 *	@param tx Translation in X-direction
	 *	@param ty Translation in Y-direction
	 */
	virtual __inline void Translate(float tx, float ty) { 
		//m_rcBounds.Translate(tx, ty); 
		CPointf ptCenter(GetCenter());
		SetCenter (ptCenter + CPointf(tx, ty));

		_OnParamsChanged();
	}

	/**
	 *	This function returns a full copy of this CDrawingObject.
	 *
	 *	@return A pointer to a new CDrawingObject. The user must take care to delete this object, one it is no longer needed.
	 *
	 *	@remarks This function must be implemented in derived classed.
	 */
	virtual CDrawingObject* Duplicate() const = NULL; //creates a deep copy

	/**
	 *	Set this CDrawingObject's parent object.
	 *
	 *	@param pParent Pointer to a CDrawing object, representing this CDrawingObject's new parent.
	 */
	__inline void SetParent(CDrawingObject* pParent) {
		m_pParent = pParent;
	}

	/**
	 *	Retrieve this CDrawingObject's paret CDrawingObject.
	 *
	 *	@return Pointer to this CDrawingObject's parent CDrawingObject. If this CDrawingObject has not parent,
				this function returns nullptr;
	 */
	__inline CDrawingObject* GetParent() const {
		return m_pParent;
	}

	/**
	 *	Add a new Child to this CDrawingObject.
	 *
	 *	@param pChild Pointer to a CDrawingObject.
	 *
	 *	Subordinated CDrawingObjects are managed by smart pointers (shared_ptr).
	 *	Therefore, it is recommended not to store additional pointers, afer a CDrawingObject was
	 *	added as a child to another CDrawingObject.
	 */
	__inline bool AddChild(CDrawingObject* pChild) {
		if (!pChild->GetParent()) {
			pChild->SetParent(this);
			m_children.push_back(shared_ptr<CDrawingObject>(pChild));
			return true;
		}
		return false;
	}

	/**
	 *	Deletes all children from this CDrawingObject.
	 */
	__inline void DeleteAllChildren() {
		m_children.clear();
	}

	/**
	 *	Retrieve, if a CDrawingObject has subordinated CDrawingObjects.
	 *
	 *	If this CDrawingObject is the parent object of other CDrawingObejcts, this function return true, otherwise false.
	 */
	__inline bool HasChildren() const {
		return (m_children.size() > 0);
	}

	/**
	 *	Set a flag, which determines if this object is draw with stipples.
	 *
	 *	@param bStippled Set this value to true, if this object is to be rendered stippled, otherwise false.
	 */
	__inline void DrawStippled(bool bStippled) {
		m_pParams->SetValue(DOP_DRAW_STIPPLED, bStippled);
	}

	/**
	 *	Retrieve if this object is drawn stippled.
	 *
	 *	@return This function return true, if this CDrawingObject is drawn stippled, therwise false.
	 */
	__inline bool DrawStippled() const {
		return m_pParams->GetValueBool(DOP_DRAW_STIPPLED);
	}

	/*
	 *	Retrieve the width of this CDrawingObject.
	 *
	 *	@return The width as float.
	 *
	 *	@remarks The with always refers to the horizontal extent of a Drawing object (parallel to the x-axis).
	 *			 It does not regard any rotation.
	 */
	__inline float GetWidth() const {
		return m_pParams->GetValueFloat(DOP_WIDTH);
	}

	/*
	 *	Retrieve the height of this CDrawingObject.
	 *
	 *	@return The height as float.
	 *
	 *	@remarks The height always refers to the vertical extent of a Drawing object (parallel to the y-axis).
	 *			 It does not regard any rotation.
	 */
	__inline float GetHeight() const {
		return m_pParams->GetValueFloat(DOP_HEIGHT);
	}

protected:
	/*
	 *	Set the width of this CDrawingObject.
	 *
	 *	@param fWidth The new width as float.
	 *
	 *	@remarks The width always refers to the horizontal extent of a Drawing object (parallel to the x-axis).
	 *			 It does not regard any rotation.
	 */
	__inline void setWidth(float fWidth) {
		m_pParams->SetValue(DOP_WIDTH, fWidth);
	}

	/*
	 *	Set the height of this CDrawingObject.
	 *
	 *	@param fHeight The new height as float.
	 *
	 *	@remarks The height always refers to the vertical extent of a Drawing object (parallel to the y-axis).
	 *			 It does not regard any rotation.
	 */
	__inline void setHeight(float fHeight) {
		m_pParams->SetValue(DOP_HEIGHT, fHeight);
	}

	/**
	 *	Loop over all children and call their repsective draw() method.
	 */
	__inline void _drawChildren() {
		for (auto iter = m_children.begin(); iter != m_children.end(); ++iter) {
			(*iter)->Draw();
		}
	}

	/**
	 *	Apply a new parameter value.
	 *
	 *	@param paramID	The integral parameter name as DrawinObjectParamName.
	 *	@param val		The new value of the parameter, as CSimpleVariant.
	 *
	 *	@return This function returns true, if the parameter value was successfully set.
	 *
	 *	@remarks	Typically, derived classes overwrite this function to handle the parameters they introduce. 
	 *				
	 */
	virtual bool setParam(DrawinObjectParamName paramID, const CSimpleVariant &val);

	/**
	 *	This function is called by SetParams(), after all parameters have been set.
	 *	Derived classes can overwrite this function to perform additional computations,
	 *	necessary when parameters have been updated. This can be, e.g. calculate a new bounding box,
	 *	or update vertex positions.
	 */
	virtual void OnSetParams();

private:
	/**
	 *	Set the type of this drawing object.
	 *	This function is usually called by the constructor of CDrawingObject only!
	 *	The object type can be critial do determine which operations can be performed
	 *	on a certain object.
	 *
	 *	You should never call this function directly.
	 */
	void SetType(DrawingObjectType nType) {
		m_pParams->SetValue(DOP_TYPE, nType);
	}

friend class CDrawingObjectMngr;
};

