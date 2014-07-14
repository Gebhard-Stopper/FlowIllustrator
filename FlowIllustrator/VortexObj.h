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
#include "Ellipseoid.h"
#include "Arrow.h"
#include "SpeedLine.h"
#include <vector>

using namespace std;
/**
 *	This class represents vortices in 2D (time-dependent) flows.
 *	The actual type of a vortex object is based on the scheme by Helmann and Hesselink,
 *	and can be queried by calling GetStyle(). This vortex style heavily influences how a vortex object is rendered
 *	(See Draw() for details).
 *	<BR>
 *	Additionally, and independetly of the vortex type, a vortex can be displayed with additional arrows. The direction
 *	in which the arros are pointing can be modified using SetRotationDir().
 *	<BR>
 *	Finally, if a vortex is of aspiral type, SetAppearance() can be used to define if a vortex is rendered as a single spiral,
 *	or a double spiral.
 *	<BR>
 *	A vortex can also be displayed with additional speed lines. This allows to effectively encode vortex motion over several timem steps.
 *	To add speed lines, you can call the SetTrajectory() function. It automatically creates two duplicates of the supplied speed line
 *	and Offsets them in positive and negative directions, perpendicular to the direction of motion and applies m_SpeedLineParams to them. Alternatively, you can 
 *	manually insert as many speed lines as you which, by calling AddChild(). In this case however, you need to manually take care of the appearance of the individual speed lines.
 */
class CVortexObj :
	public CEllipsoid
{
public: 
	enum VORTEX_STYLE { ELLIPTIC, SPIRAL_CLOCKWISE, SPIRAL_COUNTERCLOCKWISE };	/**< Encodes the vortex type, based on the definition by Helmann and Hesselink.*/
	enum VORTEX_ROTATION_DIR { CLOCK_WISE, COUNTER_CLOCKWISE };					/**< Encodes the principal direction in which the arrows of a vortex are pointing.*/
	enum VORTEX_APPEARANCE { NORMAL, DOUBLE_SPIRAL};							/**< Encodes, if a vortex is renderd as a single or a doubel spiral.*/

protected:
	shared_ptr<CPolyLine>	m_Spiral1;						/**< Vertices for the single spiral and elliptic representation.*/
	shared_ptr<CPolyLine>	m_Spiral2;						/**< Vertices for the second spiral.*/
	vector<CArrow>			m_arrows;						/**< Arrows, displayed ontop of a vortex to encode the rotation direction.*/
	bool					m_bNeedRecalc;					/**< Flag which indicates if the vertices of a vortex need to be recalculated.*/
	shared_ptr<CDrawingObjectParams>	m_SpeedLineParams;	/**< Parameters for the speed lines of a vortex, to illustrate vortex motion.*/

public:
	/**
	 *	Create a new CVortexObj object.
	 *
	 *	@param xPos				The x-coordinate of the vortex center (core) as float.
	 *	@param yPos				The y-coordinate of the vortex center (core) as float.
	 *	@param radius1			The radius of the vortex parallel to the x-axis as float.
	 *	@param radius2			The radius of the vortex parallel to the y-axis	as float.
	 *	@param fArrowSize		The size of the arrows of this CVortexObj in domain space, as float.
	 *	@param fThreshold		The threshold, to which the extent of this CVortexObj is measured as float, mus be in range [0 .. 1].
	 *	@param color			The main color as floatColor.
	 *	@param numRevolutions	The number of revolution around the vortex core as flaot. Only relevant if the vortex style is SPIRAL_CLOCKWISE or SPIRAL_COUNTERCLOCKWISE.
	 *	@param nStyle			The style of this CVortexObject. Must be either ELLIPTIC, SPIRAL_CLOCKWISE or SPIRAL_COUNTERCLOCKWISE.
	 *	@param nDir				The rotation direction, in which the arrows are pointing. Must be either CLOCK_WISE or COUNTER_CLOCKWISE.
	 */
	CVortexObj(float xPos, float yPos, float radius1, float radius2, float fArrowSize, float fThreshold, const floatColor& color, float numRevolutions = 1.5f, VORTEX_STYLE nStyle = ELLIPTIC, VORTEX_ROTATION_DIR nDir = CLOCK_WISE);
	virtual ~CVortexObj(void);

protected:
	CVortexObj(const CVortexObj &src);
	CVortexObj(const CDrawingObjectParams &params);

public:
	/**
	 *	Set the trajectory of this CVortexObj, based on a single CSpeedLine.
	 *	
	 *	@param pTrajectory A pointer to a CSpeedLine object, representing the path of the vortex core.
	 *	
	 *	@remarks	This function ensures that the trajectory of a vortex is represented by three speed lines.
	 *				Therefore, it duplicates the supplied SCpeedLine two times and offsets the two copies perpendicularly
	 *				to the direction of motion. It also moves the speedlines "behind" the vortex, s.t. they always start near the 
	 *				trailing edge of the vortex. Additionally, the trajectories are smoothed, by the specified smoothing factor.
	 *				<BR>
	 *				Please keep in mind, that after this process, the trajectories are no longer an exact representation of the actual 
	 *				vortex trajectory.
	 *				<BR>
	 *				The three CSpeedLine objects are treated as children of this CVortexObject.
	 *				<BR>
	 *				The ownership of the CSpeedLine supplied as pTrajectory is transferred to this CVortexObject and must not be deleted
	 *				by the calling function.
	 */
	void SetTrajectory(CSpeedLine *pTrajectory);
	
	/**
	 *	Set the smoothing factor for speed lines of this CVortexObj.
	 *
	 *	@param nSmoothness The Smoothness as int
	 *
	 *	@param The specified smoothness is used as the half-size of a box filter, used to smooth the vertices of the speed lines.
	 */
	__inline void SetTrajectorySmoothness(int nSmoothness) {
		m_pParams->SetValue(DOP_SMOOTHNESS, nSmoothness);
	}

	/**
	 *	Retrieve the smoothing factor for speed lines of this CVortexObj.
	 *
	 *	@return The Smoothness as int
	 *
	 *	@param The specified smoothness is used as the half-size of a box filter, used to smooth the vertices of the speed lines.
	 */
	__inline int GetTrajectorySmoothness() const {
		return m_pParams->GetValueInt(DOP_SMOOTHNESS);
	}

	/** 
	 * Retrieve the length of the vortex trajectory, in time steps (vertices).
	 *
	 *	@return The trajectoriy lenght as int.
	 *
	 *	@remarks	The trajectory length does not necessarily reflect the actual length of the attached CSpeedLine objects.
	 *				Rather, it is the amount of time steps, for which the trajectory is calculated at maximum, if enough past time steps
	 *				are available.
	 */
	__inline int GetTrajectoryLength() const {
		return m_pParams->GetValueInt(DOP_TRAJECTORYSTEPS);
	}

	/** 
	 * Set the length of the vortex trajectory, in time steps (vertices).
	 *
	 *	@param nLength The trajectoriy lenght as int.
	 *
	 *	@remarks	The trajectory length does not necessarily reflect the actual length of the attached CSpeedLine objects.
	 *				Rather, it is the amount of time steps, for which the trajectory is calculated at maximum, if enough past time steps
	 *				are available.
	 */
	__inline void SetTrajectoryLength(int nLength) {
		m_pParams->SetValue(DOP_TRAJECTORYSTEPS, nLength);
	}

	/**
	 *	Set the style of this CVortexObj.
	 *
	 *	@param nStyle The vortex style as VORTEX_STYLE.
	 *
	 *	@remarks The vortex style (type) is based on the definition by Helmann and Hesselink.
	 */
	__inline void SetStyle(VORTEX_STYLE nStyle) {
		m_pParams->SetValue(DOP_VORTEXSTYLE, nStyle);
	}

	/**
	 *	Retrieve the style of this CVortexObj.
	 *
	 *	@return The vortex style as VORTEX_STYLE.
	 *
	 *	@remarks The vortex style (type) is based on the definition by Helmann and Hesselink.
	 */
	__inline VORTEX_STYLE GetStyle() const {
		return static_cast<VORTEX_STYLE>(m_pParams->GetValueInt(DOP_VORTEXSTYLE));
	}

	/**
	 *	Set the rotation direction of this vortex.
	 *
	 *	@param nDir The rotation direction as VORTEX_ROTATION_DIR.
	 *
	 *	@remarks	The rotation direction determines the direction in which the arrows attached to this CVortexObj are pointing.
	 *				The rotation direction is independent of the vortex style.
	 */
	__inline void SetRotationDir(VORTEX_ROTATION_DIR nDir) {
		m_pParams->SetValue(DOP_VORTEXDIR, nDir);
	}

	/**
	 *	Retrieve the rotation direction of this vortex.
	 *
	 *	@return		The rotation direction as VORTEX_ROTATION_DIR.
	 *
	 *	@remarks	The rotation direction determines the direction in which the arrows attached to this CVortexObj are pointing.
	 *				The rotation direction is independent of the vortex style.
	 */
	__inline VORTEX_ROTATION_DIR GetRotationDir() const {
		return static_cast<VORTEX_ROTATION_DIR>(m_pParams->GetValueInt(DOP_VORTEXDIR));
	}

	/**
	 *	Set the size of the attached arrows.
	 *
	 *	@param fSize The size as float in domain coordinates.
	 *
	 *	@remarks	The size directly translates to the arrow length. The arrow width is
	 *				derived from the length, being 2/3 of the arrow length.
	 */
	__inline void SetArrowSize(float fSize) {
		m_pParams->SetValue(DOP_ARROWSIZE, fSize);
	}

	/**
	 *	Retrieve the size of the attached arrows.
	 *
	 *	@return		The size as float in domain coordinates.
	 *
	 *	@remarks	The size directly translates to the arrow length. The arrow width is
	 *				derived from the length, being 2/3 of the arrow length.
	 */
	__inline float GetArrowSize() const {
		return m_pParams->GetValueFloat(DOP_ARROWSIZE);
	}

	/**
	 *	Set the threshold to which the extent of this CVortexObj is measured.
	 *
	 *	@param  fThreshold The threshold as float.
	 *
	 *	@remarks	The threshold specifies a percentage of the vorticity value at the vortex core, to which the extent of 
	 *				this CVortexObj is measured. It must be in range [0 .. 1].
	 */
	__inline void SetThreshold(float fThreshold) {
		m_pParams->SetValue(DOP_THRESHOLD, fThreshold);
	}

	/**
	 *	Retrieve the threshold to which the extent of this CVortexObj is measured.
	 *
	 *	@return  The threshold as float.
	 *
	 *	@remarks	The threshold specifies a percentage of the vorticity value at the vortex core, to which the extent of 
	 *				this CVortexObj is measured. It must be in range [0 .. 1].
	 */
	__inline float GetThreshold() const {
		return m_pParams->GetValueFloat(DOP_THRESHOLD);
	}

	/**
	 *	Set, if the extent and orientation of this vortex is automatically adjusted by FlowIllustrator,
	 *	every time the current time step changes.
	 *
	 *	@param bAutoAdjust Set this value to true, if FlowIllustrator should automatically adjust the extent of this CVortexObj, otherwise false.
	 */
	__inline void AutoAdjustSize(bool bAutoAdjust) {
		m_pParams->SetValue(DOP_AUTOADJUSTSIZE, bAutoAdjust);
	}

	/**
	 *	Determines, if the extent and orientation of this vortex is automatically adjusted by FlowIllustrator,
	 *	every time the current time step changes.
	 *
	 *	@return This function return true, if FlowIllustrator should automatically adjust the extent of this CVortexObj, otherwise false.
	 */
	__inline bool AutoAdjustSize() const{
		return m_pParams->GetValueBool(DOP_AUTOADJUSTSIZE);
	}

	/**
	 *	Set, if this vortex is renderd with attached speed lines.
	 *
	 *	@param bShow If true, this vortex is to be rendered with attached speed lines.
	 *
	 *	@remarks If bShow is set to false, this function automatically destroy the currently attached CSpeedLine objects.
	 */
	__inline void ShowTrajectory(bool bShow) {
		m_pParams->SetValue(DOP_SHOWTRAJECTORY, bShow);
		if (!bShow) {
			DeleteAllChildren();
		}
	}

	/**
	 *	Determine, if this vortex is renderd with attached speed lines.
	 *
	 *	@return If true, this vortex is to be rendered with attached speed lines.
	 *
	 *	@remarks	FlowIllustrator uses this function to determine, if it needs to calculate the trajectory of the vortex represented by this CVortexObject.
	 *				FlowIllustrator automatically attaches a newly calculated Trajectory to this CVortexObject, by calling SetTrajectory().
	 */
	__inline bool ShowTrajectory() const {
		return m_pParams->GetValueBool(DOP_SHOWTRAJECTORY);
	}

	/**
	 *	Set a distinct arrow color.
	 *
	 *	@param color The new arrow color as floatColor.
	 */
	__inline void SetArrowColor(const floatColor &color) {
		m_pParams->SetValue(DOP_ARROWCOLOR, color);
		_applyArrowColor(); 
	}

	/**
	 *	Retrieve the current arrow color.
	 *
	 *	@return A reference to a floatColor, containing the curretn arrow color.
	 */
	__inline const floatColor& GetArrowColor() const {
		return m_pParams->GetValueFloatColor_r(DOP_ARROWCOLOR);
	}

public:
	/**
	 *	Draws this CVortexObj as ellipse or spiral, based on this object's vortex style.
	 *	If the specified amount of arrows is >0, additional CArrow objects are displayed
	 *	ontop of this CVortexObj. Otherwise no arrows are shown.
	 */
	virtual void Draw();

	/** 
	 *	Translate This CVortexObj and all attached childred.
	 *
	 *	@param tx Translation parallel to the x-axis, in domain coordinates, as float.
	 *	@param ty Translation parallel to the y-axis, in domain coordinates, as float.
	 */
	virtual void Translate(float tx, float ty);

	/**
	 *	Set a new center location for the vortex core.
	 *	This function is similar to translate, but it receives absolute coordinates of the new center.
	 *
	 *	@param point A reference to a Cpointf containing the new center location.
	 */
	virtual void SetCenter (const CPointf& point);

	/**
	 *	Retrieve a new CVortexObject, with an exact copy the the parameters of this CVortexObj.
	 *
	 *	@return A pointer to the new CVortexObj.
	 */
	virtual CDrawingObject* Duplicate() const;
	
	/**
	 *	Set the appearance of this CVortexObj.
	 *
	 *	@param nAppearance The new appearance of this CVortexObj, can bei either NORMAL or DOUBLE_SPIRAL.
	 *
	 *	@remarks This only has an effect, if the vortex style is either SPIRAL_CLOCKWISE or SPIRAL_COUNTERCLOCKWISE.
	 */
	__inline void SetAppearance(VORTEX_APPEARANCE nAppearance) {
		m_pParams->SetValue(DOP_APPEARANCE, nAppearance);
	}

	/**
	 *	Retrieve the appearance of this CVortexObj.
	 *
	 *	@return The appearance of this CVortexObj, can bei either NORMAL or DOUBLE_SPIRAL.
	 */
	__inline VORTEX_APPEARANCE GetAppearance() const {
		return static_cast<VORTEX_APPEARANCE>(m_pParams->GetValueInt(DOP_APPEARANCE));
	}
	
	/**
	 *	Set the amount of arrows displayed ontop of this CVortexObj.
	 *
	 *	@param nNumArrows The amount of arrows as int. Must be >= 0.
	 *
	 *	@remarks If nNumArrows is 0, the vortex is displayed without arrows.
	 */
	__inline void SetNumArrows(int nNumArrows) {
		m_pParams->SetValue(DOP_NUMARROWS, nNumArrows);
	}

	/**
	 *	Retrieve the amount of arrows displayed ontop of this CVortexObj.
	 *
	 *	@return The amount of arrows as int. Must be >= 0.
	 *
	 *	@remarks If nNumArrows is 0, the vortex is displayed without arrows.
	 */
	__inline int GetNumArrows() const {
		return m_pParams->GetValueInt(DOP_NUMARROWS);
	}
	
	/**
	 *	Set the number of revolutions for spiralling vortices.
	 *
	 *	@param nRevolutions The number of revolutions as float.
	 *
	 *	@remarks The number of revolutions has no effect, if the vortex style is ELLIPTIC.
	 */
	__inline void SetNumRevolutions(float nRevolutions) {
		m_pParams->SetValue(DOP_REVOLUTIONS, nRevolutions);
	}

	/**
	 *	Retrieve the number of revolutions for spiralling vortices.
	 *
	 *	@return The number of revolutions as float.
	 *
	 *	@remarks The number of revolutions has no effect, if the vortex style is ELLIPTIC.
	 */
	__inline float GetNumRevolutions() const {
		return m_pParams->GetValueFloat(DOP_REVOLUTIONS);
	}

#ifdef WIN32
	/**
	 *	Ensures special CVortexObj parameters are contained in the string, returned by ToString().
	 */
	virtual void OnToString(CString &str, float fLineWidthFactor) const;

	/**
	 *	Overwrite of CDrawingObj::FinalizeToString()
	 *
	 *	Ensures, that the attached arrows, if any, are also contained in the string,
	 *	returned by ToString()
	 */
	virtual void FinalizeToString(CString &str, float fLineWidthFactor) const;

	virtual CString toString(float fLineWidthFactor = 1.0f) const;
#else
#endif

protected:
	virtual void GetParams(CDrawingObjectParams &params) const; 
	virtual bool SetChildObjParams(const CDrawingObjectParams &params);
	virtual void _OnParamsChanged();
	virtual bool setParam(DrawinObjectParamName paramID, const CSimpleVariant &val);
	virtual void OnSetParams();

private:
	void DrawSpiral();
	void DrawElliptic();
	void AddArrows(vector<CPointf> &points);
	void calcSpiral(float fNumRevolutions, float fAngularOffset, vector<CPointf> &points);
	void DrawTransparent();

	void _draw(bool bRecalc);
	void _applyArrowColor();

	void _init();

private:
	CVector2D _getTangentialVec(float angle, float r1, float r2) const;

	friend class CSVGConverter;
};

