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
#include "Arrow.h"


CArrow::CArrow(CPointf ptStart, CVector2D dir, float fLength, const floatColor& color)
	:	CDrawingObject(CRectF(ptStart.x, ptStart.y, dir.x, dir.y), DO_ARROW, color),
		m_ptStart(ptStart), m_vecDir(dir), m_fLength(fLength)
{
	IsSolid(true);
}

CArrow::~CArrow(void)
{
}

void CArrow::Draw()
{
	glColor4fv(GetColorGL());

	std::vector<CVector2D> points;
	_getVertices(points);

	glBegin(GL_TRIANGLE_FAN);
	for (auto iter = points.begin(); iter != points.end(); iter++) {
		glVertex2f( iter->x, iter->y );
	}
	glEnd();
}

void CArrow::_getVertices(std::vector<CVector2D> &points) const
{
	CVector2D mainAxis = m_vecDir;
	mainAxis.Normalize();
	CVector2D secondary(-mainAxis.y, mainAxis.x);

	mainAxis *= m_fLength;
	secondary *= (m_fLength/3.0f);

	points.push_back( CVector2D(m_ptStart.x - (mainAxis.x/4.0f), m_ptStart.y - (mainAxis.y/4.0f)));
	points.push_back( CVector2D(m_ptStart.x + secondary.x, m_ptStart.y + secondary.y));
	points.push_back( CVector2D(m_ptStart.x - mainAxis.x, m_ptStart.y - mainAxis.y));
	points.push_back( CVector2D(m_ptStart.x - secondary.x, m_ptStart.y - secondary.y));
}

bool CArrow::setParam(DrawinObjectParamName paramID, const CSimpleVariant &val)
{
	bool bResult(CDrawingObject::setParam(paramID, val));

	if (!bResult)
	{
		switch (paramID)
		{
			case DOP_ARROW_ORIGIN:
				m_ptStart = val.GetPointfVal();
				bResult = true;
				break;
			case DOP_ARROW_DIR:
				m_vecDir = val.GetVector2DVal();
				bResult = true;
				break;
			case DOP_ARROW_LEN:
				m_fLength = val.GetFloatVal();
				bResult = true;
				break;
		}
	}

	return bResult;
}
 
void CArrow::Translate(float tx, float ty)
{
	CDrawingObject::Translate(tx, ty);
	m_ptStart += CPointf(tx, ty);
}

#ifdef WIN32
CString CArrow::toString(float fLineWidthFactor) const
{
	CString str, dummy;
	CString style	= getStyleString(fLineWidthFactor);

	CRectF rect = GetRect();
	rect.Normalize();

	std::vector<CVector2D> points;
	_getVertices(points);

	str.Format(_T("<polygon\n\
					objectType=\"%d\"\n\
					style=%s\n\
					arrowLen=\"%f\"\n\
					id=\"%p\"\n\
					points=\""),
					GetType(), style, m_fLength, this);

	for (auto iter = points.begin(); iter != points.end(); iter++) {
		dummy.Format(_T("%f,%f "),iter->x, iter->y);
		str += dummy;
	}

	str += _T("\"\n/>\n");

	return str;
}
#endif

CDrawingObject* CArrow::Duplicate() const
{
	CArrow *pNewObj = new CArrow(m_ptStart, m_vecDir, m_fLength, GetColor());

	pNewObj->CopyParams(this);

	return pNewObj;
}