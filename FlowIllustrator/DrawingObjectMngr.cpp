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
#include "DrawingObjectMngr.h"


CDrawingObjectMngr::CDrawingObjectMngr(void)
{
}

CDrawingObjectMngr::~CDrawingObjectMngr(void)
{
	RemoveAll();
}

void CDrawingObjectMngr::Add(CDrawingObject* pObj)
{
	Add( shared_ptr<CDrawingObject>(pObj) );
}

void CDrawingObjectMngr::Add(shared_ptr<CDrawingObject> pObj)
{
	m_data.push_back( pObj );
}

void CDrawingObjectMngr::InsertAt(int nIndex, CDrawingObject* pObj)
{
	auto iter = m_data.begin() + nIndex;
	m_data.insert(iter, shared_ptr<CDrawingObject>(pObj));
}

shared_ptr<CDrawingObject> CDrawingObjectMngr::GetAt(int nIndex) const
{
	return m_data.at(nIndex);
}

int CDrawingObjectMngr::Find(const CPointf& point)
{
	for (int idx = static_cast<int>(m_data.size()-1) ; idx > -1; idx--) {
		if ( m_data[idx]->GetRect().PtInRect(point) ) {
			return idx;
		}
	}

	return -1;
}

int CDrawingObjectMngr::Find(CDrawingObject* pObj) const
{
	for (size_t idx = 0 ; idx < m_data.size(); idx++)
	{
		if (m_data[idx].get() == pObj)
			return static_cast<int>(idx);
	}

	return -1;
}

void CDrawingObjectMngr::RemoveAt(int nIndex)
{
	m_data.erase( m_data.begin() + nIndex);
}

void CDrawingObjectMngr::DeleteAt(int nIndex)
{
	RemoveAt(nIndex);
}

void CDrawingObjectMngr::Delete(CDrawingObject* pObj)
{
	int nIndx = Find(pObj);

	if (nIndx > -1) {
		DeleteAt(nIndx);
	}
}

void CDrawingObjectMngr::RemoveAll()
{
	m_data.clear();
}

size_t CDrawingObjectMngr::Size() const
{
	return m_data.size();
}

void CDrawingObjectMngr::Draw()
{
	for (auto iter = m_data.begin(); iter != m_data.end(); iter++)
	{
		(*iter)->Draw();
	}
}

int CDrawingObjectMngr::MoveToFront(int nIdxToMove)
{
	shared_ptr<CDrawingObject> pDummy = m_data[nIdxToMove];
	m_data.erase(m_data.begin()+nIdxToMove);
	m_data.push_back(pDummy);

	return static_cast<int>(m_data.size()-1);
}

int CDrawingObjectMngr::MoveForward(int nIdxToMove)
{
	if (nIdxToMove < static_cast<int>(m_data.size())-1) {
		shared_ptr<CDrawingObject> pDummy = m_data[nIdxToMove];
		m_data[nIdxToMove] = m_data[nIdxToMove+1];
		m_data[nIdxToMove + 1] = pDummy;

		return nIdxToMove + 1;
	}

	return nIdxToMove;
}

int CDrawingObjectMngr::MoveBackward(int nIdxToMove)
{
	if (nIdxToMove > 0) {
		shared_ptr<CDrawingObject> pDummy = m_data[nIdxToMove];
		m_data[nIdxToMove] = m_data[nIdxToMove-1];
		m_data[nIdxToMove - 1] = pDummy;

		return nIdxToMove - 1;
	}

	return nIdxToMove;
}

int CDrawingObjectMngr::MoveToBack(int nIdxToMove)
{
	shared_ptr<CDrawingObject> pDummy = m_data[nIdxToMove];
	m_data.erase(m_data.begin()+nIdxToMove);
	m_data.insert(m_data.begin(), pDummy);

	return 0;
}
