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

#include "StdAfx.h"
#include "VortexObj.h"


CVortexObj::CVortexObj(float xPos, float yPos, float radius1, float radius2, float fArrowSize, float fThreshold, const floatColor& color, float numRevolutions, VORTEX_STYLE nStyle, VORTEX_ROTATION_DIR nDir)
	:	CEllipsoid(xPos, yPos, radius1, radius2, color, false, DO_VORTEX), m_bNeedRecalc(true)
{
	_init();

	IsSolid(false);
	SetNumRevolutions(numRevolutions);
	SetNumArrows(2);
	SetTrajectoryLength(10);
	SetTrajectorySmoothness(3);
	SetAppearance(NORMAL);
	SetStyle(nStyle);
	SetRotationDir(nDir);
	SetArrowSize(fArrowSize);
	SetThreshold(fThreshold);
	AutoAdjustSize(true);
	ShowTrajectory(false);
	SetArrowColor(color);
	
}

CVortexObj::CVortexObj(const CDrawingObjectParams &params)
	:	CEllipsoid(	params.GetValuePointf(DOP_CENTER), params.GetValueFloat(DOP_RADIUS1), params.GetValueFloat(DOP_RADIUS2), 
		params.GetValueFloatColor(DOP_COLOR), params.GetValueBool(DOP_ISSOLID), DO_VORTEX )
{
	_init();
	SetParams(params);
	m_bNeedRecalc = true;
	
}

CVortexObj::CVortexObj(const CVortexObj &src)
	:	CEllipsoid(src.GetCenter(), src.GetRadius1(), src.GetRadius2(), src.GetColor(), src.IsSolid(), DO_VORTEX),
		m_bNeedRecalc(true)
{
	_init();
	if (src.m_children.size() > 0) {
		SetTrajectory( reinterpret_cast<CSpeedLine*>(src.m_children[0].get()->Duplicate()) );
	}

	SetNumRevolutions(src.GetNumRevolutions());
	SetNumArrows(src.GetNumArrows());
	SetTrajectoryLength(src.GetTrajectoryLength());
	SetTrajectorySmoothness(src.GetTrajectorySmoothness());
	SetHaloColor(src.GetHaloColor());
	SetAppearance(src.GetAppearance());
	SetStyle(src.GetStyle());
	SetRotationDir(src.GetRotationDir());
	SetArrowSize(src.GetArrowSize());
	SetThreshold(src.GetThreshold());
	AutoAdjustSize(src.AutoAdjustSize());
	ShowTrajectory(src.ShowTrajectory());
	SetArrowColor(src.GetArrowColor());
	DrawHalo(src.DrawHalo());
	
}

void CVortexObj::_init()
{
	m_Spiral1 = shared_ptr<CPolyLine>( new CPolyLine(GetColor(), GetThickness()) );
	m_Spiral2 = shared_ptr<CPolyLine>( new CPolyLine(GetColor(), GetThickness()) );

	_OnParamsChanged();
	m_bNeedRecalc = false;
}

CVortexObj::~CVortexObj(void)
{
	_DestroyVertexBuffer();
}

void CVortexObj::Draw()
{
	if (m_bNeedRecalc) {
		m_arrows.clear();
	}
	
	_drawChildren();

	if (DrawHalo())
	{
		float thickness = GetThickness();
		floatColor color = GetColor();

		SetThickness(2*thickness);
		SetColor(GetHaloColor());
		_draw(m_bNeedRecalc);

		SetThickness(thickness);
		color.alpha = 1.0;
		SetColor(color);

		_draw(false);	//no need to recalc here
	} else {
		_draw(m_bNeedRecalc);
	}

	for (auto iter = m_arrows.begin(); iter != m_arrows.end(); ++iter) {
		iter->Draw();
	}
}

void CVortexObj::_draw(bool bRecalc)
{
	CDrawingObject::Draw();

	if (GetStyle() != ELLIPTIC) {
		if (bRecalc) DrawSpiral();
		DrawTransparent();
	} else {
		if (bRecalc) {
			DrawElliptic();
		}
		m_Spiral1->Draw();
	}
}

void CVortexObj::DrawTransparent()
{
	m_Spiral1->Draw();
	if (GetAppearance() == DOUBLE_SPIRAL) {
		m_Spiral2->Draw();
	}
}

bool CVortexObj::setParam(DrawinObjectParamName paramID, const CSimpleVariant &val)
{
	bool bResult(CEllipsoid::setParam(paramID, val));

	if (!bResult)
	{
		switch (paramID)
		{
			case DOP_REVOLUTIONS:
					SetNumRevolutions(val.GetFloatVal());
					bResult = true;
					break;
				case DOP_VORTEXSTYLE:
					SetStyle(static_cast<VORTEX_STYLE>(val.GetIntVal()));
					bResult = true;
					break;
				case DOP_ARROWSIZE:
					SetArrowSize(val.GetFloatVal());
					bResult = true;
					break;
				case DOP_AUTOADJUSTSIZE:
					AutoAdjustSize(val.GetBoolVal());
					bResult = true;
					break;
				case DOP_THRESHOLD:
					SetThreshold(val.GetFloatVal());
					bResult = true;
					break;
				case DOP_VORTEXDIR:
					SetRotationDir(static_cast<VORTEX_ROTATION_DIR>(val.GetIntVal()));
					bResult = true;
					break;
				case DOP_NUMARROWS:
					SetNumArrows(val.GetIntVal());
					bResult = true;
					break;
				case DOP_TRAJECTORYSTEPS:
					SetTrajectoryLength(val.GetIntVal());
					bResult = true;
					break;
				case DOP_SHOWTRAJECTORY:
					ShowTrajectory(val.GetBoolVal());
					bResult = true;
					break;
				case DOP_SMOOTHNESS:
					SetTrajectorySmoothness(val.GetIntVal());
					bResult = true;
					break;
				case DOP_APPEARANCE:
					SetAppearance(static_cast<VORTEX_APPEARANCE>(val.GetIntVal()));
					bResult = true;
					break;
				case DOP_ARROWCOLOR:
					SetArrowColor(val.GetColorVal());
					bResult = true;
					break;
		}
	}
	return bResult;
}

void CVortexObj::OnSetParams()
{
	_OnParamsChanged();
}

void CVortexObj::_applyArrowColor()
{
	for (auto iter = m_arrows.begin(); iter != m_arrows.end(); ++iter) {
		iter->SetColor(GetArrowColor());
	}
}

void CVortexObj::GetParams(CDrawingObjectParams &params) const
{
	CEllipsoid::GetParams(params);

	if (HasChildren()) {
		params.SetChildParams( m_SpeedLineParams );
	}
}

void CVortexObj::Translate(float tx, float ty)
{
	CPointf ptCenter(GetCenter());
	SetCenter( ptCenter + CPointf(tx,ty) );
}

void CVortexObj::DrawElliptic()
{
	float step = static_cast<float>(2.0*M_PI/NUM_ELLIPSE_POINTS);
	float rad = DegToRad(GetRotation());	
	float r1 = GetRadius1();
	float r2 = GetRadius2();
	CVector2D center = CPointf::toVector2D(GetCenter());

	vector<CPointf> *points = m_Spiral1->GetDataPoints();
	points->clear();

	//calculate points around origin and rotate them
	for (int i=0; i < NUM_ELLIPSE_POINTS; i++)
	{
		CVector2D vec(	r1 * cos(i*step), r2 * sin(i*step));
		vec.Rotate(rad);

		vec += center;

		points->push_back( CPointf::fromVector2D(vec) );	
	}

	points->push_back( (*points)[0] );
	points->push_back( (*points)[1] );

	AddArrows(*points);
}

void CVortexObj::DrawSpiral()
{
	float fRevolutions(GetNumRevolutions());
	vector<CPointf> *points = m_Spiral1->GetDataPoints();

	calcSpiral(fRevolutions, 0.0f, *points);
	AddArrows(*points);

	if (GetAppearance() == DOUBLE_SPIRAL) {
		points = m_Spiral2->GetDataPoints();
		calcSpiral(fRevolutions, static_cast<float>(M_PI), *points);
		AddArrows(*points);
	}
}

void CVortexObj::calcSpiral(float fNumRevolutions, float fAngularOffset, vector<CPointf> &points)
{
	points.clear();

	float step = static_cast<float>(2.0*M_PI/NUM_ELLIPSE_POINTS);
	float rad = DegToRad(GetRotation());

	float rStep1 = GetRadius1() / (NUM_ELLIPSE_POINTS*fNumRevolutions);
	float rStep2 = GetRadius2() / (NUM_ELLIPSE_POINTS*fNumRevolutions);
	float r1, r2;

	if (GetStyle() == SPIRAL_CLOCKWISE)
	{
		r1 = 0;
		r2 = 0;
	} else {
		r1 = GetRadius1();
		r2 = GetRadius2();
		rStep1 *= -1;
		rStep2 *= -1;
	}

	int nNumPoints( static_cast<int>(fNumRevolutions * NUM_ELLIPSE_POINTS) );

	CVector2D center = CPointf::toVector2D(GetCenter());

	for (int i=0; i < nNumPoints; i++)
	{
		float factor = ((2*i/NUM_ELLIPSE_POINTS) /fNumRevolutions);
		if (GetStyle() != SPIRAL_CLOCKWISE) {
			factor = 2.0f - factor;
		}

		CVector2D vec (	r1 * cosf(i * (step )+fAngularOffset),
						r2 * sinf(i * (step )+fAngularOffset));
		
		vec.Rotate(rad) ;

		vec += center;

		points.push_back( CPointf::fromVector2D(vec) );

		r1 += rStep1*factor;
		r2 += rStep2*factor;
	}

}

void CVortexObj::AddArrows(vector<CPointf> &points)
{
	int nArrows(GetNumArrows());

	if (nArrows <= 0) return;
	VORTEX_STYLE nStyle(GetStyle());

	int nNumTransparentPts = (nStyle != ELLIPTIC)? (NUM_ELLIPSE_POINTS*GetNumRevolutions())/8 : 0;
	nNumTransparentPts /= 2;

	int nDiff = static_cast<int>( (nStyle == ELLIPTIC)? (points.size()-1) : (NUM_ELLIPSE_POINTS*(GetNumRevolutions()-.5f)-nNumTransparentPts) );
	nDiff /= nArrows;

	int tanPt(0);

	for (int i=0; i < nArrows; i++) {
		int nPoint = (nStyle==SPIRAL_COUNTERCLOCKWISE)? (nNumTransparentPts + i*nDiff):(points.size()-1) - (nNumTransparentPts + i*nDiff);
		
		switch (nStyle)
		{
		case ELLIPTIC:
			if(GetRotationDir() == COUNTER_CLOCKWISE) {
				tanPt = nPoint;
				nPoint --;
			} else {
				tanPt = nPoint-1;
			}
			break;
		case SPIRAL_CLOCKWISE:
			if(GetRotationDir()== CLOCK_WISE) {
				tanPt = nPoint -3;
			} else {
				tanPt = nPoint;
				nPoint -= 3;
			}
			break;
		case SPIRAL_COUNTERCLOCKWISE:
			if(GetRotationDir()== CLOCK_WISE) {
				tanPt = nPoint;
				nPoint += 3;
			} else {
				 tanPt = nPoint+3;
			}
			break;
		}

		CPointf pt (points[nPoint].x+points[tanPt].x, points[nPoint].y+points[tanPt].y);
		pt.x /= 2.0f;
		pt.y /= 2.0f;

		m_arrows.push_back(CArrow ( pt , points[nPoint] - points[tanPt], GetArrowSize(), GetArrowColor()));
	}
}

CVector2D CVortexObj::_getTangentialVec(float angle, float r1, float r2) const
{
	if (GetRotationDir() == CLOCK_WISE) {
		return CVector2D(-r1*sin(angle), r2*cos(angle));
	}

	return CVector2D(-r1*sin(angle), r2*cos(angle));
}

#ifdef WIN32

void CVortexObj::OnToString(CString &str, float /*fLineWidthFactor*/) const
{
	CString dummy;
	dummy.Format(_T("%s=\"%f\"\n\
					 %s=\"%f\"\n"),	DrawingObjNames.GetParamName(DOP_RADIUS1), GetRadius1(),
									DrawingObjNames.GetParamName(DOP_RADIUS2), GetRadius2());
	str += dummy;

	str += _T("points=\"");

	vector<CPointf> *points = m_Spiral1->GetDataPoints();

	for (auto iter = points->begin(); iter != points->end(); ++iter) {
		str += iter->ToString() + _T(" ");
	}

	if (GetStyle() == ELLIPTIC) {
		str += (*points)[0].ToString() + _T(" ");
	} else if (GetAppearance() != NORMAL) {
		points = m_Spiral2->GetDataPoints();
		for (auto iter = points->begin(); iter != points->end(); ++iter) {
			str += iter->ToString() + _T(" ");
		}
	}

	str += "\"\n";
}

void CVortexObj::FinalizeToString(CString &str, float fLineWidthFactor) const
{
	for (auto iter = m_arrows.begin(); iter != m_arrows.end(); ++iter) {
		str += iter->toString(fLineWidthFactor);
	}

	CDrawingObject::FinalizeToString(str, fLineWidthFactor);
}

CString CVortexObj::toString(float fLineWidthFactor) const
{
	return CDrawingObject::toString(fLineWidthFactor);
}
#endif

void CVortexObj::SetTrajectory(CSpeedLine *pTrajectory)
{
	DeleteAllChildren();

	if (pTrajectory)
	{
		pTrajectory->Smooth(GetTrajectorySmoothness());

		vector<CPointf> *pOriginal	= pTrajectory->GetDataPoints();
		
		CVector2D dir; // Direction from which the vortex came from

		//find a barely large enough vector
		float maxI = pOriginal->size()/5.0f;
		if (maxI < 1.0f) maxI = 1.0f;

		for (int i = 1; i < maxI; i++) {
			dir += (*pOriginal)[i] - (*pOriginal)[i-1];
			if (dir.abs() > 1e-10) break;
		}

		dir /= maxI;

		CVector2D offset = dir;
		CVector2D r(GetRadius1(), GetRadius2());

		if (dir != CVector2D(0,0)) {
			//follow the direction, untill outside of vortex
			while (offset.abs() < r.abs()) {
				offset += dir/2;
			}
		}
		
		CVector2D dirOrth(-offset.y, offset.x);
		dirOrth /= 3;

		pTrajectory->Translate(offset.x, offset.y);

		if ( !m_SpeedLineParams ) {
			CDrawingObjectParams *pParams = new CDrawingObjectParams(DO_SPEEDLINE);
			pTrajectory->GetParams(*pParams);
			m_SpeedLineParams = shared_ptr<CDrawingObjectParams>(pParams);
		} else {
			pTrajectory->SetParams( *m_SpeedLineParams.get() );
		}

		CSpeedLine *pClone1 = reinterpret_cast<CSpeedLine*>(pTrajectory->Duplicate());
		CSpeedLine *pClone2 = reinterpret_cast<CSpeedLine*>(pTrajectory->Duplicate());

		pClone1->Translate(dirOrth.x, dirOrth.y);
		pClone2->Translate(-dirOrth.x, -dirOrth.y);
		
		AddChild(pTrajectory);
		AddChild(pClone1);
		AddChild(pClone2);
	}
}

void CVortexObj::SetCenter (const CPointf& point)
{
	CPointf center = GetCenter();
	m_bNeedRecalc = true;

	float distX(point.x - center.x);
	float distY(point.y - center.y);

	for (auto iter = m_children.begin(); iter != m_children.end(); ++iter) {
		reinterpret_cast<CSpeedLine*>((*iter).get())->Translate(distX, distY);
	}

	CDrawingObject::SetCenter(point);
}

CDrawingObject* CVortexObj::Duplicate() const
{
	CVortexObj *pVortex = new CVortexObj(*this);

	return pVortex;
}

bool CVortexObj::SetChildObjParams(const CDrawingObjectParams &params)
{
	bool bResult (CEllipsoid::SetChildObjParams(params));

	if (bResult)
	{
		//update our struct
		if (params.GetValueInt(DOP_TYPE) == DO_SPEEDLINE)
		{
			for (auto iter = params.dataMap.begin(); iter != params.dataMap.end(); ++iter)
			{
				m_SpeedLineParams->SetValue( iter->first, iter->second );
			}
		}
	}

	return bResult;
}

void CVortexObj::_OnParamsChanged()
{
	m_Spiral1->SetThickness(GetThickness());
	m_Spiral2->SetThickness(GetThickness());

	m_Spiral1->SetColor(GetColor());
	m_Spiral2->SetColor(GetColor());

	m_Spiral1->SetHaloColor(GetHaloColor());
	m_Spiral2->SetHaloColor(GetHaloColor());

	m_Spiral1->SetNumTransparentSteps((NUM_ELLIPSE_POINTS*GetNumRevolutions())/8);
	m_Spiral2->SetNumTransparentSteps((NUM_ELLIPSE_POINTS*GetNumRevolutions())/8);

	m_Spiral1->SetNumGrowSteps(m_Spiral1->GetNumTransparentSteps()/8);
	m_Spiral2->SetNumGrowSteps(m_Spiral2->GetNumTransparentSteps()/8);

	m_Spiral1->SetAlpha(1.0f, .1f);
	m_Spiral2->SetAlpha(1.0f, .1f);

	m_Spiral1->SetThicknessMin(GetThickness()*.75f);
	m_Spiral2->SetThicknessMin(GetThickness()*.75f);

	switch (GetStyle())
	{
		case ELLIPTIC:
			m_Spiral1->SetStyle( SL_THICKNESS_CONST | SL_TRANSPARENCY_CONST);
			m_Spiral2->SetStyle( SL_THICKNESS_CONST | SL_TRANSPARENCY_CONST);
			break;
		case SPIRAL_CLOCKWISE:
			m_Spiral1->SetStyle( SL_THICKNESS_DECREASE | SL_TRANSPARENCY_DECREASE);
			m_Spiral2->SetStyle( SL_THICKNESS_DECREASE | SL_TRANSPARENCY_DECREASE);
			break;
		case SPIRAL_COUNTERCLOCKWISE:
			m_Spiral1->SetStyle( SL_THICKNESS_DECREASE | SL_TRANSPARENCY_INCREASE);
			m_Spiral2->SetStyle( SL_THICKNESS_DECREASE | SL_TRANSPARENCY_INCREASE);
			break;
	}

	m_bNeedRecalc = true;
}