/*
 *	Copyright (C) 2014, Max Planck Institut f�r Informatik, Saarbr�cken.
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

// MFCRibbonCheckBoxStub.cpp : implementation file
//

#include "stdafx.h"
#include "FlowIllustrator.h"
#include "MFCRibbonCheckBoxStub.h"


// CMFCRibbonCheckBoxStub

CMFCRibbonCheckBoxStub::CMFCRibbonCheckBoxStub()
{
}

CMFCRibbonCheckBoxStub::~CMFCRibbonCheckBoxStub()
{
}


// CMFCRibbonCheckBoxStub member functions
void CMFCRibbonCheckBoxStub::SetCheck(CMFCRibbonCheckBox* pBox, BOOL bCheck)
{
	reinterpret_cast<CMFCRibbonCheckBoxStub*>(pBox)->m_bIsChecked = bCheck;
}

BOOL CMFCRibbonCheckBoxStub::GetCheck(CMFCRibbonCheckBox* pBox)
{
	return reinterpret_cast<CMFCRibbonCheckBoxStub*>(pBox)->m_bIsChecked;
}