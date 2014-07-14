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
#include "ShaderMngr.h"

//code based on: http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/loading.php


CShaderMngr::CShaderMngr(void)
{
}


CShaderMngr::~CShaderMngr(void)
{
	for (auto iter = m_Shaders.begin();  iter != m_Shaders.end(); iter++) {
		glDetachObjectARB(iter->second->nProgID, iter->second->nShaderID);
		glDeleteObjectARB(iter->second->nProgID);
		glDeleteObjectARB(iter->second->nShaderID);

		delete iter->second;
	}

	m_Shaders.clear();
}

bool CShaderMngr::LoadShader(const char *strFileName, GLenum nShaderType, const char *strName)
{
	shaderData *sData = new shaderData;

	if (_loadShader(strFileName, sData) == 0)
	{
		if (_compileShader(sData, nShaderType))
		{
			m_Shaders.push_back(std::pair<string, shaderData*>(string(strName), sData));
			return true;
		}
	}

	return false;
}

bool CShaderMngr::UseShader(const char *strName)
{
	if (strName == nullptr) 
	{
		glUseProgramObjectARB(0);
		return true;
	}

	shaderData *pData = GetShaderData(strName);

	if (pData)
	{
		glUseProgramObjectARB(pData->nProgID);
		return true;
	}

	return false;
}

int	CShaderMngr::_loadShader(const char *strFileName, shaderData *pData)
{
	ifstream file;
	file.open(strFileName, ios::in); // opens as ASCII!
	if(!file) return -1;
    
	pData->nLen = _getFileLength(file);
    
	if (pData->nLen == 0) return -2;   // Error: Empty File 
    
	pData->pSrc = (GLchar*) new char[pData->nLen+1];
	if (pData->pSrc == 0) return -3;   // can't reserve memory
   
	// len isn't always strlen cause some characters are stripped in ascii read...
	// it is important to 0-terminate the real length later, len is just max possible value... 
	pData->pSrc[pData->nLen] = 0; 

	unsigned int i=0;
	while (file.good())
	{
		pData->pSrc[i] = file.get();       // get character from file.
		if (!file.eof())
		i++;
	}
    
	pData->pSrc[i] = 0;  // 0-terminate it at the correct position
    
	file.close();
      
	return 0; // No Error
}

unsigned long CShaderMngr::_getFileLength(ifstream &inFile) const
{
	if(!inFile.good()) return 0;
    
    streamoff pos = inFile.tellg();
    inFile.seekg(0,ios::end);
    streamoff len = inFile.tellg();
    inFile.seekg(pos);
    
    return static_cast<unsigned long>(len);
}

bool CShaderMngr::_compileShader(shaderData *pData, GLenum nShaderType)
{
	pData->nShaderID = glCreateShaderObjectARB(nShaderType);

	if (pData->nShaderID == 0) return false;	//creating the shader object failed

	glShaderSourceARB(pData->nShaderID, 1, (const_cast<const GLcharARB**>(&pData->pSrc)), &(pData->nLen));
	glCompileShader(pData->nShaderID);


	int sz;
	char errLog[4096];
	glGetShaderInfoLog(pData->nShaderID, 4096, &sz, errLog);

	GLint bResult;

	glGetShaderiv(pData->nShaderID, GL_COMPILE_STATUS, &bResult);
	if (bResult != GL_TRUE) return false;	//Compilation failed

	pData->nProgID = glCreateProgramObjectARB();
	glAttachObjectARB(pData->nProgID, pData->nShaderID);

	glLinkProgramARB(pData->nProgID);

	glGetProgramiv(pData->nProgID, GL_LINK_STATUS, &bResult);
	return (bResult==GL_TRUE);
}

shaderData_t::~shaderData_t()
{
	 if (pSrc != 0)
		delete [] pSrc;
}

shaderData* CShaderMngr::GetShaderData(const char *strProgName)
{
	string name(strProgName);

	for (auto iter = m_Shaders.begin();  iter != m_Shaders.end(); iter++) {
		if (iter->first.compare( name ) == 0) {
			return iter->second;
		}
	}

	return nullptr;
}

void CShaderMngr::SetUniformData(const char *strShaderName, const char *strVariableName, float *pData, int nNumElements)
{
	shaderData *pShaderData = GetShaderData(strShaderName);

	if (pShaderData)
	{
		const GLint location = glGetUniformLocation(pShaderData->nProgID, strVariableName);
		glUniform1fv(location, nNumElements, pData);
	}
}

void CShaderMngr::SetUniformData(const char *strShaderName, const char *strVariableName, int *pData, int nNumElements)
{
	shaderData *pShaderData = GetShaderData(strShaderName);

	if (pShaderData)
	{
		const GLint location = glGetUniformLocation(pShaderData->nProgID, strVariableName);
		glUniform1iv(location, nNumElements, pData);
	}
}