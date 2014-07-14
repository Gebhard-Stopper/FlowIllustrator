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
#include "drawingobject.h"
#include "Pointf.h"


class CLine :
	public CDrawingObject
{
public:
	CLine(const CPointf &from, const CPointf& to, const floatColor& color);
	virtual ~CLine(void);

public:
	virtual void Draw();
	virtual CDrawingObject* Duplicate() const;

	__inline void SetLineStart(const CPointf& point) {
		m_pParams->SetValue(DOP_PTSTART, point);
	}

	__inline const CPointf& GetLineStart() const {
		return m_pParams->GetValuePointf_r(DOP_PTSTART);
	}

	__inline void SetLineEnd(const CPointf& point) {
		m_pParams->SetValue(DOP_PTEND, point);
	}

	__inline const CPointf& GetLineEnd() const {
		return m_pParams->GetValuePointf_r(DOP_PTEND);
	}

protected:
	virtual void _OnParamsChanged() {}
	virtual bool setParam(DrawinObjectParamName paramID, const CSimpleVariant &val);
};

