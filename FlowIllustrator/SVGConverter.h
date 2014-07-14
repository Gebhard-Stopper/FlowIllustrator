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
#include "DrawingObjectMngr.h"
#include "Markup.h"
#include <hash_map>

using namespace std;

class CSVGConverter
{
public:
	CSVGConverter(void);
	~CSVGConverter(void);

public:
	static BOOL LoadSVG(const CString &strFileName, CDocument &document);
	static CString CSVGConverter::GetSVGString(CDocument &document);

private:
	static void _addNewDrawingObject(__in CMarkup &doc, __out CDrawingObjectMngr &dataContainer, __out hash_map<unsigned int, CDrawingObject*> &objCache, float fLinewidthFactor);
	static BOOL _checkFileName(__in const CString &strOriginalFileName, __out CString &strRealFileName);
	static int _stepInside(__in CMarkup &doc, __out CDrawingObjectMngr &dataContainer, __out hash_map<unsigned int, CDrawingObject*> &objCache, float fLinewidthFactor);
	static void parseStyleString(__in const CString& strSource, __out LPFLOATCOLOR pColor, __out BOOL &bSolid, __out float &fLineWidth);
	static void parseTransformString(__in const CString& strSource, __out float &rotation);
	
	static float _getFloatAttrib(__in CMarkup &doc, __in const CString& strAttribName, float fDefault=0.0f);
	static int _getIntAttrib(__in CMarkup &doc, __in const CString& strAttribName, int nDefualt=0);
	static unsigned int _getHexAttrib(CMarkup &doc, const CString& strAttribName, int nDefault=0);
	static bool _getBoolAttrib(__in CMarkup &doc, __in const CString& strAttribName, bool bDefault=false);
	static floatColor _getColorAttrib(__in CMarkup &doc, __in const CString& strAttribName, const floatColor &defaultValue);
	static CPointf _getPointfAttrib(__in CMarkup &doc, __in const CString& strAttribName, const CPointf &defaultValue = CPointf(0,0));

	static DrawingObjectType _getDrawingObjectType(__in CMarkup &doc);
	static CDrawingObject* _getTriangle(__in CMarkup &doc);
	static CDrawingObject* _getEllipse(__in CMarkup &doc);
	static CDrawingObject* _getRectangle(__in CMarkup &doc);
	static CDrawingObject* _getVortex(__in CMarkup &doc);
	static CDrawingObject* _getStreamLine(__in CMarkup &doc);
	static CDrawingObject* _getPathLine(__in CMarkup &doc);
	static CDrawingObject* _getStreakLine(__in CMarkup &doc);
	static CDrawingObject* _getTimeLine(__in CMarkup &doc);
	static CDrawingObject* _getSpeedLine(__in CMarkup &doc);

	static floatColor _extractFloatColor(__in const CString &str);
	static void _getVertices(__in CMarkup &doc, __in const CString& strAttribName, __out vector<CPointf>* pDest, int numVertices);

	static void _getBaseParams(__in CMarkup& doc, CDrawingObjectParams &params);
	static void _getStreamLineParams(__in CMarkup& doc, CDrawingObjectParams &params);

	static float _getLineWidthFactor(CDocument &document);
};

