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
#include "Line.h"


CLine::CLine(const CPointf &from, const CPointf& to, const floatColor& color)
	:CDrawingObject(CRectF(	(from.x < to.x)?from.x:to.x,
							(from.y < to.y)?from.y:to.y,
							(from.x > to.x)?from.x:to.x,
							(from.y > to.y)?from.y:to.y), DO_LINE, color)
{
	SetLineStart(from);
	SetLineEnd(to);
}


CLine::~CLine(void)
{
}

void CLine::Draw()
{
	CDrawingObject::Draw();

	glBegin(GL_LINES);
	glVertex2fv( reinterpret_cast<const GLfloat*>(&GetLineStart()) );
	glVertex2fv( reinterpret_cast<const GLfloat*>(&GetLineEnd()) );
	glEnd();
}

bool CLine::setParam(DrawinObjectParamName paramID, const CSimpleVariant &val)
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

CDrawingObject* CLine::Duplicate() const
{
	CLine *pNewObj = new CLine(GetLineStart(), GetLineEnd(), GetColor());

	pNewObj->CopyParams(this);

	return pNewObj;
}