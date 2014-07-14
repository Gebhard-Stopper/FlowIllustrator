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

//Create a memory mapping for the specified file
//if a file is already open, it must be closed in order to open another file.
class CAmiraReader : public CBasicFileReader
{
public:
	CAmiraReader() :	m_pFileMapping(NULL), m_hMapping(NULL), m_hFile(NULL),
						m_nFrameBufferSize(FRAME_BUFFER_SIZE), m_nFrameSize(0), m_nDataOffset(0),
						m_nCurrFileOffset(0){}
	virtual ~CAmiraReader();

private:
	void *m_pFileMapping;
	HANDLE m_hMapping;
	HANDLE m_hFile;

	//Helpers for mapping file frames from disk to memory
private:
	size_t	m_nNumBytesToRead;	//Must be at least two times the size of the systems Allocation granularity in order to capture silulation frames that overlap
								//the boundary of two allocation frames
	size_t	m_nFrameSize;		//The size of a single simulation frame in bytes
	size_t	m_nDataOffset;		//The offset from the beginning of the file to where the actual data starts
	size_t	m_nFrameBufferSize;	//Number of frames to buffer in memory

	size_t	m_nCurrFileOffset;	//Current offset within the file in bytes (must be a multiple of AllocationGranularity)

private:
	void CloseCurrentMapping();

public:
	bool readAmiraFile(const char *FileName, CAmiraVectorField2D *pOutData);
	void requireTimestep(size_t nTimeStep, CAmiraVectorField2D *pOutData);	//Ensures the data required for nTimeStep is mapped to memory
	int GetMinMappedFrame();
	int GetMaxMappedFrame();
};