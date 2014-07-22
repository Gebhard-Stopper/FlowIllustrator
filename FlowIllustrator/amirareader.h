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
#include <vector>
#include "AmiraVectorField2D.h"
#include "BasicFileReader.h"

using namespace std;

#define FRAME_BUFFER_SIZE 50

/**
 *	CAmiraReader allows to access read data stored in amira mesh files (*.am).
 *	The provided data is accessed via memory mapped files. This has the effect, that
 *	data does not need to be copied into memory before it can be used. Instead, data
 *	is moved into memory only if needed. This allows to instantaneously access data
 *	even from files with several gigabytes in size.
 *	<BR>
 *	The disadvantage of this technique is, that ir requires exclusive access to the file being opened.
 */
class CAmiraReader : public CBasicFileReader
{
public:
	/** 
	 *	Creates a new CAmiraReader object and initializes it.
	 */
	CAmiraReader();

	/**
	 *	Destroys this CAmiraReader object and closes the currently opened file.
	 */
	virtual ~CAmiraReader();

private:
	void *m_pFileMapping;	/**< Pointer from the address space of FlowIllustrator into the mapped file. */
	HANDLE m_hMapping;		/**< Handle to the file mapping.*/
	HANDLE m_hFile;			/**< Handle to the file that is being mapped into memory.*/

	//Helpers for mapping file frames from disk to memory
private:
	size_t	m_nNumBytesToRead;	/**< Must be at least two times the size of the systems Allocation granularity in order to capture simulation frames that overlap the boundary of two allocation frames.*/
	size_t	m_nFrameSize;		/**< The size of a single simulation frame in bytes.*/
	size_t	m_nDataOffset;		/**< The offset from the beginning of the file to where the actual data starts.*/
	size_t	m_nFrameBufferSize;	/**< Number of frames to buffer in memory.*/
	size_t	m_nCurrFileOffset;	/**< Current offset within the file in bytes (must be a multiple of AllocationGranularity).*/

private:
	/**
	 *	Closes the current file mapping and releases all current resources.
	 */
	void CloseCurrentMapping();

public:
	/**
	 *	Opens an amira mesh file (*.am), maps it into the memory of the provided CAmiraVectorField2D.
	 *
	 *	@param FileName The name of the amira mesh file to be opened as pointer to char array.
	 *	@param pOutData Pointer to a CAmiraVectorField2D, that allows FlowIllustrator to access the data in the specified file.
	 *
	 *	@return This function returns true, if the specified file could be opened and was successfully mapped into memory. Otherwise it returns false.
	 */
	bool readAmiraFile(const char *FileName, CAmiraVectorField2D *pOutData);
};