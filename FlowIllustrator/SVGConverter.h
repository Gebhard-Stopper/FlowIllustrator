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


/**
 *	CSVGConverter takes care of reading and writing SVG files.
 *	It holds no inner state and thus only provides static member functions.
 *	<BR>
 *	If you extent any class derived from CDrawingObject by adding additional parameters, be sure to 
 *	also extent teh respective _get*() methods in CSVGConverter.
 *	<BR>
 *	If you add new classes derived from CDrawingObject to FlowIllustrator, you must add an accrding _get*() method
 *	end ensure it is called by _addNewDrawingObject().
 *	<BR>
 *	In later versions, CSVGConverter is intended to automatically recognize and read all parameters of any 
 *	object derived from CDrawingObject.
 */
class CSVGConverter
{
public:
	CSVGConverter(void);
	~CSVGConverter(void);

public:
	/**
	 *	Read a SVG file, and store its content to a CFlowIllustratorDoc.
	 *
	 *	@param strFileName The name of the file to be opened.
	 *	@param document Reference to a CFlowIllustratorDoc to receive the fiel content.
	 *
	 *	@return Returns TRUE on success, otherwise FALSE.
	 */
	static BOOL LoadSVG(const CString &strFileName, CDocument &document);

	/**
	 *	Retrieve a SVG string, representing all CDrawingObject-derived elements in the domain of the curretn vector field.
	 *
	 *	@return The SVG formatted string, ready to be saved to a file.
	 */
	static CString CSVGConverter::GetSVGString(CDocument &document);

private:
	/**
	 *	Detects the type of a CDrawingObject at the current location in the SVG file,
	 *	and calls the accrding _get*() method. This function also recreates the Object hierarchy of CDrawingObjects.
	 *
	 *	@param doc	Reference to CMarkup representing the DOM tree of the SVG file.
	 *	@param dataContainer Reference to a CDrawingObjectMngr to recieve the found CDrawingObjects
	 *	@param objCache Reference to a std::hash_map, used to rebuild the object hierarchy
	 *	@param fLinewidthFactor Factor to map from pixel-coordinate to domain-coordinates.
	 *
	 *	@remarks Called by _stepInside() when a new Object is discovered.
	 */
	static void _addNewDrawingObject(__in CMarkup &doc, __out CDrawingObjectMngr &dataContainer, __out hash_map<unsigned int, CDrawingObject*> &objCache, float fLinewidthFactor);
	
	/**
	 *	Checks if the file, specified by strOriginalFileName actually exists, and further determines,
	 *	if the contained data is encoded in UNICODE. The file name returned by strRealFileName is formatted
	 *	accordingly and allows to read the file's content correctly.
	 *
	 *	@param strOriginalFileName The file name to be validated.
	 *	@param strRealFileName The validated file name, possibly encoded in UNICODE
	 *
	 *	@return Returns TRUE, if the file exists, otherwise FALSE.
	 */
	static BOOL _checkFileName(__in const CString &strOriginalFileName, __out CString &strRealFileName);
	
	/**
	 *	Recursive function to traverse the DOM tree of a SVG file.
	 *
	 *	@param doc	Reference to CMarkup representing the DOM tree of the SVG file.
	 *	@param dataContainer Reference to a CDrawingObjectMngr to recieve the found CDrawingObjects
	 *	@param objCache Reference to a std::hash_map, used to rebuild the object hierarchy
	 *	@param fLinewidthFactor Factor to map from pixel-coordinate to domain-coordinates.
	 *
	 *	@return The time step for which the illustration in this file is valid.
	 */
	static int _stepInside(__in CMarkup &doc, __out CDrawingObjectMngr &dataContainer, __out hash_map<unsigned int, CDrawingObject*> &objCache, float fLinewidthFactor);
	
	/**
	 *	Extracts the color, thickness, as well as if an object is solid from an SVG-conform obejct.
	 *
	 *	@param strSource	The string to be parsed.
	 *	@param pColor		Pointer to a floatColor, that receives the RGB color values contained in strSource.
	 *	@param bSolid		Pointer to a bool, that receives if the object is solod.
	 *	@param fLineWidth	Pointer to a float, that receives the thickness of the object.
	 */
	static void parseStyleString(__in const CString& strSource, __out LPFLOATCOLOR pColor, __out BOOL &bSolid, __out float &fLineWidth);
	
	/**
	 *	Extracts the rotation of an SVG-conform obejct.
	 *
	 *	@param strSource	The string to be parsed.
	 *	@param rotation		Pointer to a float to receive the rotation about the object's center.
	 */
	static void parseTransformString(__in const CString& strSource, __out float &rotation);
	
	/**
	 *	Extract a single float attribute of the current Object.
	 *
	 *	@param doc	Reference to a CMarkup obejct, maintaining the current position inside the SVG file.
	 *	@param strAttribName	A reference to a string, specifying the attribute's name.
	 *	@param fDefault	The default value to return, if the attribute is not found.
	 *
	 *	@return If the attribute is found, this function returns the corresponding value, otherwise it returns the specified defualt value.
	 */
	static float _getFloatAttrib(__in CMarkup &doc, __in const CString& strAttribName, float fDefault=0.0f);
	
	/**
	 *	Extract a single integer attribute of the current Object.
	 *
	 *	@param doc	Reference to a CMarkup obejct, maintaining the current position inside the SVG file.
	 *	@param strAttribName	A reference to a string, specifying the attribute's name.
	 *	@param nDefault	The default value to return, if the attribute is not found.
	 *
	 *	@return If the attribute is found, this function returns the corresponding value, otherwise it returns the specified defualt value.
	 */
	static int _getIntAttrib(__in CMarkup &doc, __in const CString& strAttribName, int nDefualt=0);

	/**
	 *	Extract a single unsigned integer attribute of the current Object.
	 *
	 *	@param doc	Reference to a CMarkup obejct, maintaining the current position inside the SVG file.
	 *	@param strAttribName	A reference to a string, specifying the attribute's name.
	 *	@param nDefault	The default value to return, if the attribute is not found.
	 *
	 *	@return If the attribute is found, this function returns the corresponding value, otherwise it returns the specified defualt value.
	 */
	static unsigned int _getHexAttrib(CMarkup &doc, const CString& strAttribName, int nDefault=0);
	
	/**
	 *	Extract a single bool attribute of the current Object.
	 *
	 *	@param doc	Reference to a CMarkup obejct, maintaining the current position inside the SVG file.
	 *	@param strAttribName	A reference to a string, specifying the attribute's name.
	 *	@param bDefault	The default value to return, if the attribute is not found.
	 *
	 *	@return If the attribute is found, this function returns the corresponding value, otherwise it returns the specified defualt value.
	 */
	static bool _getBoolAttrib(__in CMarkup &doc, __in const CString& strAttribName, bool bDefault=false);
	
	/**
	 *	Extract a single floatColor attribute of the current Object.
	 *
	 *	@param doc	Reference to a CMarkup obejct, maintaining the current position inside the SVG file.
	 *	@param strAttribName	A reference to a string, specifying the attribute's name.
	 *	@param defaultValue	The default value to return, if the attribute is not found
	 *
	 *	@return If the attribute is found, this function returns the corresponding value, otherwise it returns the specified defualt value.
	 */
	static floatColor _getColorAttrib(__in CMarkup &doc, __in const CString& strAttribName, const floatColor &defaultValue);

	/**
	 *	Extract a single CPointf attribute of the current Object.
	 *
	 *	@param doc	Reference to a CMarkup obejct, maintaining the current position inside the SVG file.
	 *	@param strAttribName	A reference to a string, specifying the attribute's name.
	 *	@param defaultValue	The default value to return, if the attribute is not found
	 *
	 *	@return If the attribute is found, this function returns the corresponding value, otherwise it returns the specified defualt value.
	 */
	static CPointf _getPointfAttrib(__in CMarkup &doc, __in const CString& strAttribName, const CPointf &defaultValue = CPointf(0,0));
	
	/**
	 *	Extracts afloat color from the supplied string.
	 *
	 *	@param str	The string to be parsed.
	 *
	 *	@return The flaotColor found in the string argument.
	 *
	 *	@remarks	This function expects the supplied string to contain a RGB color in hexadecimal representationin one of the following formats:
	 *					- #RRGGBB
	 *					- RRGGBB
	 *				If the supplied string does not contain a valid RGB color, this function returns a floatColor with undefined RGB values.
	 *				The alpah value of the returned floatColor, by default is set to 1.0f .
	 */
	static floatColor _extractFloatColor(__in const CString &str);
	
	/**
	 *	Extract 2D coordinates.
	 *
	 *	@param doc	Reference to a CMarkup obejct, maintaining the current position inside the SVG file.
	 *	@param strAttribName A reference to a string, specifying the attribute's name.
	 *	@param pDest	A pointer to a std::vector to receive the coordinate pairs as CPointf objects
	 *	@param numVertices	The approximated number of vertices to extract.
	 *
	 *	@remarks	The data of the attribute specified by strAttribName are parsed to find pairs of x,y -coordinates.
	 *				Thereby, the coordinates must be in the following format: x1,y1 x2,y2 ... xN,yN
	 *				numVertices Is used to reserve enough space in the std::vector, specified in pDest.
	 *				This function parses the entire data-string of the given attribute. The resulting number of points
	 *				may therefore be more or less than specified by numVertices.
	 */
	static void _getVertices(__in CMarkup &doc, __in const CString& strAttribName, __out vector<CPointf>* pDest, int numVertices);

	/**
	 *	Helper function to read parameters common to all CDrawingObjects.
	 *
	 *	@param doc	Reference to a CMarkup obejct, maintaining the current position inside the SVG file.
	 *	@param params	Reference to a CDrawingObjectParams object to receive the parameter/value pairs.
	 */
	static void _getBaseParams(__in CMarkup& doc, CDrawingObjectParams &params);

	/**
	 *	Helper function to read parameters common to all objects derived from CStreamLine.
	 *
	 *	@param doc	Reference to a CMarkup obejct, maintaining the current position inside the SVG file.
	 *	@param params	Reference to a CDrawingObjectParams object to receive the parameter/value pairs.
	 */
	static void _getStreamLineParams(__in CMarkup& doc, CDrawingObjectParams &params);

	/**
	 *	This function retrieves the factor to map from pixel-coordinate to domain-coordinates.
	 *
	 *	@return The line-width-factor as float.
	 */
	static float _getLineWidthFactor(CDocument &document);

	/**
	 *	This function retrieves the type of CDrawingObject at the current location in the SVG file.
	 *
	 *	@param doc	Reference to a CMarkup obejct, maintaining the current position inside the SVG file.

	 *	@return The object type as DrawingObjectType.
	 */
	static DrawingObjectType _getDrawingObjectType(__in CMarkup &doc);

	/**
	 *	Extract a CTriangle from the current location in the SVG file.
	 *
	 *	@return Pointer to a new CTriangleObject.
	 */
	static CDrawingObject* _getTriangle(__in CMarkup &doc);

	/**
	 *	Extract a CEllipsoid from the current location in the SVG file.
	 *
	 *	@return Pointer to a new CEllipsoid.
	 */
	static CDrawingObject* _getEllipse(__in CMarkup &doc);

	/**
	 *	Extract a CRectangle from the current location in the SVG file.
	 *
	 *	@return Pointer to a new CRectangle.
	 */
	static CDrawingObject* _getRectangle(__in CMarkup &doc);

	/**
	 *	Extract a CVortexObj from the current location in the SVG file.
	 *
	 *	@return Pointer to a new CVortexObj.
	 */
	static CDrawingObject* _getVortex(__in CMarkup &doc);

	/**
	 *	Extract a CStreamLine from the current location in the SVG file.
	 *
	 *	@return Pointer to a new CStreamLine.
	 */
	static CDrawingObject* _getStreamLine(__in CMarkup &doc);

	/**
	 *	Extract a CPathLine from the current location in the SVG file.
	 *
	 *	@return Pointer to a new CPathLine.
	 */
	static CDrawingObject* _getPathLine(__in CMarkup &doc);

	/**
	 *	Extract a CStreakLine from the current location in the SVG file.
	 *
	 *	@return Pointer to a new CStreakLine.
	 */
	static CDrawingObject* _getStreakLine(__in CMarkup &doc);

	/**
	 *	Extract a CTimeLine from the current location in the SVG file.
	 *
	 *	@return Pointer to a new CTimeLine.
	 */
	static CDrawingObject* _getTimeLine(__in CMarkup &doc);

	/**
	 *	Extract a CSpeedLine from the current location in the SVG file.
	 *
	 *	@return Pointer to a new CSpeedLine.
	 */
	static CDrawingObject* _getSpeedLine(__in CMarkup &doc);
};

