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
#include "BSpline.h"


//TODO: calc Bounding rect

CBSpline::CBSpline(int numPoints, const floatColor& color)
	: CDrawingObject(CRectF(0.0f, 0.0f, 0.0f, 0.0f), DO_BSPLINE, color), m_nNumPoints(numPoints)
{
	m_pPoints	= new CPointf[numPoints];
	m_pKnots	= new float[numPoints+ORDER];
}


CBSpline::~CBSpline(void)
{
	delete [] m_pPoints;
	delete [] m_pKnots;
}

void CBSpline::Draw()
{
	CDrawingObject::Draw();
	//loop over Knot-vector in steps of e.g. 0.1 (this is then t)
	//Implement deBoor algo as on slide 28 of the b-spline lecture
	//done
	float step = 0.01f;

	int numSteps	= static_cast<int>(m_pKnots[m_nNumPoints+(ORDER-1)] / step);
	CPointf *points = new CPointf[numSteps];
	int i=0;

	for (float t = 0; t<m_pKnots[m_nNumPoints+(ORDER-1)]; t += step, i++)
	{
		int r = getR(t); // we go backwards, r ==Max(i)
		points[i] = deBoor(DEGREE, r, t);
	}

	glBegin(GL_LINE_STRIP);
	for (int i=0; i < numSteps; i++)
	{
		glVertex2f(points[i].x, points[i].y);
	}
	glEnd();
}

CPointf CBSpline::deBoor(int j, int i, float t)
{
	if (j==0) return m_pPoints[i];

	float alpha = (t - m_pKnots[i])/(m_pKnots[i+ORDER-j] - m_pKnots[i]);

	return deBoor(j-1, i-1, t) *(1.0f-alpha) + deBoor(j-1, i, t) * alpha;
}

int CBSpline::getR(float t)
{
	for (int i = 1; i < m_nNumPoints + ORDER - 1 ; i++)
	{
		if ( t < m_pKnots[i] ) return i-1;
	}

	return -1; // this is bad!
}

void CBSpline::CalcKnotVector()
{
	//calculates a uniform knot vektor
	
	int deg		= ORDER-1;
	//int denom	= m_nNumPoints-deg;
	float max	= static_cast<float>(m_nNumPoints - deg);

	for (int i=0; i<m_nNumPoints+ORDER; i++)
	{
		if (i < (ORDER)) m_pKnots[i] = 0.0f;
		else if(i > deg && i < m_nNumPoints) m_pKnots[i] = float(i-deg);///(m_nNumPoints-deg);
		else m_pKnots[i] = max;//1.0f;

		//m_pKnots[i] = double((i-deg))/(m_nNumPoints-deg);
	}

	/*int numKnots = m_nNumPoints + ORDER;

	for (int i=0; i <numKnots; i++) 
	{
		/*if (i<ORDER) knotVector[i] = 0.0;
		else if (i>DEGREE && i<NumControlPoints) knotVector[i] = double((i-DEGREE))/(NumControlPoints-DEGREE);
		else  knotVector[i] = 1.0;
		m_pKnots[i] = double((i-DEGREE))/(m_nNumPoints-DEGREE);
	}*/
}

bool CBSpline::setParam(DrawinObjectParamName paramID, const CSimpleVariant &val)
{
	bool bResult(CDrawingObject::setParam(paramID, val));

	if (!bResult)
	{
		switch (paramID)
		{
			//todo
		}
	}

	return bResult;
}

#ifdef WIN32
CString CBSpline::toString(float fLineWidthFactor) const
{
	CString str;

	return str;
}
#else
std::string CBSpline::toString() const
{
	std::string str;

	return str;
}
#endif

CDrawingObject* CBSpline::Duplicate() const
{
	return new CBSpline(*this);
}