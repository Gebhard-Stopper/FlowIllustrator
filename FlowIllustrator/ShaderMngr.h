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
#include <fstream>
#include <vector>

using namespace std;

typedef struct shaderData_t
{
public:
	~shaderData_t();

	char	*pStrName;	//User defined name of the shader
	GLchar	*pSrc;		//pointer to the source code
	GLint	 nLen;		//Length of the source code in bytes
	GLint	 nShaderID;	//OpenGL shader name
	GLint	 nProgID;	//OpenGL program name
}shaderData;

class CShaderMngr
{
protected:
	vector<std::pair<string, shaderData*>>	m_Shaders;

public:
	CShaderMngr(void);
	~CShaderMngr(void);

public:
	bool LoadShader(const char *strFileName, GLenum nShaderType, const char *strName);
	bool UseShader(const char *strName);
	void SetUniformData(const char *strShaderName, const char *strVariableName, float *pData, int nNumElements);
	void SetUniformData(const char *strShaderName, const char *strVariableName, int *pData, int nNumElements);

	shaderData* GetShaderData(const char *strProgName);

private:
	int	_loadShader(const char *strFileName, shaderData *pData);
	bool _compileShader(shaderData *pData, GLenum nShaderType);
	unsigned long _getFileLength(ifstream &inFile) const;
};

