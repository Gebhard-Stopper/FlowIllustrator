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

#include "amirareader.h"
#include <stdio.h>
#include <string>
#include <assert.h>


const char* FindAndJump(const char* buffer, const char* SearchString)
{
    const char* FoundLoc = strstr(buffer, SearchString);
    if (FoundLoc) return FoundLoc + strlen(SearchString);
    return buffer;
}

CAmiraReader::CAmiraReader() 
	:	m_pFileMapping(NULL), m_hMapping(NULL), m_hFile(NULL),
		m_nFrameBufferSize(FRAME_BUFFER_SIZE), m_nFrameSize(0), m_nDataOffset(0),
		m_nCurrFileOffset(0)
{}

CAmiraReader::~CAmiraReader()
{
	CloseCurrentMapping();
}

#include <strsafe.h>
void ErrorExit(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);

#ifdef DEBUG
	ExitProcess(dw);
#endif
}


bool CAmiraReader::readAmiraFile(const char *FileName, CAmiraVectorField2D *pOutData)
{
	CloseCurrentMapping();

	m_hFile = ::CreateFileA(FileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_hFile != NULL)
	{
		char buffer[2048];
		DWORD numBytesRead = 0;

		if (!ReadFile(m_hFile, buffer, 2048, &numBytesRead, NULL))
		{
			CloseHandle(m_hFile);
			return false;
		}

		if (!strstr(buffer, "# AmiraMesh BINARY-LITTLE-ENDIAN 2.1"))
		{
			CloseHandle(m_hFile);
			return false;
		}

		//Find the Lattice definition, i.e., the dimensions of the uniform grid
		int xDim(0), yDim(0), zDim(0);
		sscanf_s(FindAndJump(buffer, "define Lattice"), "%d %d %d", &xDim, &yDim, &zDim);

		//Find the BoundingBox
		float xmin(1.0f), ymin(1.0f), zmin(1.0f);
		float xmax(-1.0f), ymax(-1.0f), zmax(-1.0f);
		sscanf_s(FindAndJump(buffer, "BoundingBox"), "%g %g %g %g %g %g", &xmin, &xmax, &ymin, &ymax, &zmin, &zmax);

		//Is it a uniform grid? We need this only for the sanity check below.
		const bool bIsUniform = (strstr(buffer, "CoordType \"uniform\"") != NULL);

		//Type of the field: scalar, vector
		int NumComponents(0);
		if (strstr(buffer, "Lattice { float Data }"))
		{
			//Scalar field
			NumComponents = 1;
		}
		else
		{
			//A field with more than one component, i.e., a vector field
			sscanf_s(FindAndJump(buffer, "Lattice { float["), "%d", &NumComponents);
		}

		//Sanity check
		if (xDim <= 0 || yDim <= 0 || zDim <= 0
			|| xmin > xmax || ymin > ymax || zmin > zmax
			|| !bIsUniform || NumComponents <= 0)
		{
			CloseHandle(m_hFile);
			return false;
		}

		//Find the beginning of the data section
		m_nDataOffset = strstr(buffer, "# Data section follows") - buffer;
		if (m_nDataOffset > 0)
		{
			m_nDataOffset += strlen("# Data section follows\n@1\n");

			SYSTEM_INFO sysInfo;
			::GetNativeSystemInfo(&sysInfo);

			m_nFrameSize = xDim * yDim * NumComponents * sizeof(float);

			//NumToRead holds at least 25 frames from the simulation file, but it also rounds the reserved space up to the next multiple of 
			//the allocation granularity. This way, it is easy to use multiples of NumToRead to specify an offset within the file
			m_nNumBytesToRead = m_nFrameSize * m_nFrameBufferSize;
			int remainder = m_nNumBytesToRead % sysInfo.dwAllocationGranularity;
			m_nNumBytesToRead = sysInfo.dwAllocationGranularity + m_nNumBytesToRead - remainder;

			m_nCurrFileOffset = 0;
			//NumToRead must be at least 2*sysInfo.dwAllocationGranularity in order to being able to shift the frame
			//of the fileView, s.t. simulation frames that overlap a physical memory boundary can be loaded and displayed correctly

			//Now, we need to calculate the offset to the actual data frame and shift the data-pointer accordingly
			//for the first frame, this is idxStartData
			//for subsequent frames we need to calculate the offset

			if (pOutData)
			{
				m_hMapping = CreateFileMappingA(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
				if (m_hMapping)
				{
					m_pFileMapping = MapViewOfFile(m_hMapping, FILE_MAP_READ, 0, m_nCurrFileOffset, /*m_nNumBytesToRead*/0);
					if (m_pFileMapping != nullptr)
					{
						//*pOutData = new CAmiraVectorField2D(CRectangle(xmin, ymin, xmax, ymax), xDim, yDim, zDim, reinterpret_cast<CVector2D*>((char*)m_pFileMapping + m_nDataOffset));
						pOutData->Init(	CRectF(xmin, ymin, xmax, ymax),
										zmax,
										xDim, 
										yDim, 
										zDim, 
										reinterpret_cast<CAmiraVectorField2D*>((char*)m_pFileMapping + m_nDataOffset)
										);
						return true;
					}
				}
			}
		}
	}

	ErrorExit(_T("Read Amira File"));
	return false;
}

void CAmiraReader::CloseCurrentMapping()
{
	if(m_pFileMapping && m_hMapping && m_hFile)
	{
		::UnmapViewOfFile(m_pFileMapping);
		::CloseHandle(m_hMapping);
		::CloseHandle(m_hFile);

		m_pFileMapping	= NULL;
		m_hMapping		= NULL;
		m_hFile			= NULL;
	}
}