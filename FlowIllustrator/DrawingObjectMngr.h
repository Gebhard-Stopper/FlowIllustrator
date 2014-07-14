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
#include "DrawingObject.h"
#include "Pointf.h"
#include <vector> // for now...

using namespace std;

/**
 *	This class manages objects, derived from CDrawingObject.
 *	It is designed to allow simple object handling and is used by CFlowIllustratorRenderView
 *	to maintain all objects that are to be drawn onto of a vector field.
 *
 *	CDrawingObjectMngr uses the shared_ptr class to store objects, derived from CDrawingObject.
 */
class CDrawingObjectMngr
{
private:
	vector<shared_ptr<CDrawingObject>>	m_data;	/**< Vector to hold all drawing objects */

public:
	/**
	 *	Construct a new CDrawingObjectMngr object.
	 */
	CDrawingObjectMngr(void);

	/**
	 *	Destroy this CDrawingObjectMngr object and all its data.
	 */
	~CDrawingObjectMngr(void);

public:
	/**
	 *	Add a new object of type CDrawingObject or a class derived from CDrawingObject to the end of m_data.
	 *
	 *	@param pObj Pointer to the CDrawingObject to be added.
	 */
	void Add(CDrawingObject* pObj);

	/**
	 *	Add a new object of type CDrawingObject or a class derived from CDrawingObject to the end of m_data.
	 *
	 *	@param pObj Pointer to the CDrawingObject to be added.
	 */
	void Add(shared_ptr<CDrawingObject> pObj);

	/**
	 *	Insert a new CDrawingObject at the specified location.
	 *
	 *	@param nIndex The zero-based index where the new element is to be inserted.
	 *	@param pObj Pointer the the CDrawingObject to be inserted.
	 */
	void InsertAt(int nIndex, CDrawingObject* pObj);

	/**
	 *	Retrieve the element at the specified index.
	 *
	 *	@param	nIndex The zero-based index, from where the CDrawingObject is to be retrieved.
	 *
	 *	@return	A shared_ptr to the CDrawingObject.
	 */
	shared_ptr<CDrawingObject> GetAt(int nIndex) const;

	/**
	 *	Searches for a CDrawingObject, where point lies inside its bounding rectangle.
	 *
	 *	@param	point Spatial point to search for.
	 *
	 *	@return The index of the first CDrawingObject where point lies inside its bounding rectangle. 
				If no bounding rectangle is found, thsi function returns -1.
	 */
	int Find(const CPointf& point);

	/**
	 *	Retrieved the index of a CDrawingObject at the specified memory location.
	 *
	 *	@param	pObj Pointer to a CDrawingObject whose index is to be determined.
	 *
	 *	@return The index of the CDrawingObject. 
				If no bounding rectangle is found, thsi function returns -1.
	 */
	int Find(CDrawingObject* pObj) const;

	/**
	 *	@see DeleteAt()
	 */
	void RemoveAt(int nIndex);

	/**
	 *	Deletes the CDrawingObject at the specified index.
	 *	If necessary, subsequent elements in m_data are shifted "down".
	 *
	 *	@param nIndex The index, where the CDrawingObject is to be deleted.
	 */
	void DeleteAt(int nIndex);

	/**
	 *	Deletes the CDrawingObject at the specified memory location.
	 *	If necessary, subsequent elements in m_data are shifted "down".
	 *
	 *	@param pObj Pointer to the object to be deleted.
	 *
	 *	@remarks	The object is only deleted, if it is actually stored in m_data.
					After deletion, pObj is an invalid pointer and using it can cause undefined behaviour.
	 */
	void Delete(CDrawingObject* pObj);

	/**
	 *	Deletes all elements in m_data.
	 */
	void RemoveAll();

	/**
	 *	Retrieve the number of elements in m_data.
	 *
	 *	@return The element count.
	 */
	size_t Size() const;

	/**
	 *	This function loops over all elements in m_data and invokes their draw() member function.
	 */
	void Draw();

	/**
	 *	Moves the specified CDrawingObject to the end of m_data.
	 *	This causes the specified object to be drawn last and thus appearing "on top" of all other objects.
	 *
	 *	@param nIdxToMove Index of the object to move.
	 */
	int MoveToBack(int nIdxToMove);

	/**
	 *	Moves the specified CDrawingObject one position closer to the back of m_data.
	 *
	 *	@param nIdxToMove Index of the object to move.
	 */
	int MoveBackward(int nIdxToMove);

	/**
	 *	Moves the specified CDrawingObject one position closed to the front of m_data.
	 *
	 *	@param nIdxToMove Index of the object to move.
	 */
	int MoveForward(int nIdxToMove);

	/**
	 *	Moves the specified CDrawingObject to the beginning of m_data.
	 *	This causes the specified object to be drawn first and thus appearing "below" of all other objects.
	 *
	 *	@param nIdxToMove Index of the object to move.
	 */
	int MoveToFront(int nIdxToMove);
};

