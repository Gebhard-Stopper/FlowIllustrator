#include "StdAfx.h"


#include "SimpleXML.h"


namespace simplexml
{
	CSimpleXMLAttribute::CSimpleXMLAttribute(const CString& attribName, const CString& attribValue, BOOL bLargeVal)
		: strAttribName(attribName), strAttribValue(attribValue), bLargeValue(bLargeVal)
	{}

	CString CSimpleXMLAttribute::ToString() const
	{
		CString str;
		if (bLargeValue) {
			str.Format(_T("%s=\"%s\"\n"), strAttribName, strAttribValue);
		} else {
			str.Format(_T("%s=%s\n"), strAttribName, strAttribValue);
		}
		
		return str;
	}

	CSimpleXMLNode::CSimpleXMLNode(const CString& nodeName, const CString& nodeValue)
		: strNodeName(nodeName), strNodeValue(nodeValue)
	{}

	CSimpleXMLNode::CSimpleXMLNode(const CString& nodeName)
		: strNodeName(nodeName)
	{}

	void CSimpleXMLNode::AddAttribute(const CString& attribName, const CString& attribValue, BOOL bLargeVal)
	{
		AddAttribute(new CSimpleXMLAttribute(attribName, attribValue, bLargeVal));
	}

	void CSimpleXMLNode::AddAttribute(CSimpleXMLAttribute* pNewAttribute)
	{
		m_attributes.push_back(shared_ptr<CSimpleXMLAttribute>(pNewAttribute));
	}

	void CSimpleXMLNode::AddChildNode(const CString& nodeName, const CString& nodeValue)
	{
		AddChildNode(new CSimpleXMLNode(nodeName, nodeValue));
	}

	void CSimpleXMLNode::AddChildNode(CSimpleXMLNode* pNewChildNode)
	{
		m_children.push_back(shared_ptr<CSimpleXMLNode>(pNewChildNode));
	}

	BOOL CSimpleXMLNode::HasAttributes() const
	{
		return m_attributes.size() != 0;
	}

	vector<shared_ptr<CSimpleXMLAttribute>>::iterator CSimpleXMLNode::GetFirstAttribute()
	{
		return m_attributes.begin();
	}

	vector<shared_ptr<CSimpleXMLAttribute>>::iterator CSimpleXMLNode::GetLastAttribute()
	{
		return m_attributes.end();
	}

	BOOL CSimpleXMLNode::HasChildren() const
	{
		return m_children.size() != 0;
	}

	vector<shared_ptr<CSimpleXMLNode>>::iterator CSimpleXMLNode::GetFirstChild()
	{
		return m_children.begin();
	}

	vector<shared_ptr<CSimpleXMLNode>>::iterator CSimpleXMLNode::GetLastChild()
	{
		return m_children.end();
	}

	shared_ptr<CSimpleXMLNode>CSimpleXMLNode::FindChildNode(const CString& strNodeName)
	{
		if (HasChildren())
		{
			auto result = _findChildNode(strNodeName);
			if (result != m_children.end()) {
				return (*result);
			}
		}

		return shared_ptr<CSimpleXMLNode>(nullptr);
	}

	shared_ptr<CSimpleXMLNode> CSimpleXMLNode::PopChildNode(const CString& strNodeName)
	{
		if (HasChildren())
		{
			auto result = _findChildNode(strNodeName);
			if (result != m_children.end()) {
				shared_ptr<CSimpleXMLNode> retVal = (*result);
				m_children.erase(result);
				return retVal;
			}
		}

		return shared_ptr<CSimpleXMLNode>(nullptr);
	}

	shared_ptr<CSimpleXMLAttribute> CSimpleXMLNode::FindAttribute(const CString& strAttributeName)
	{
		if (HasAttributes())
		{
			auto result = _findAttribute(strAttributeName);
			if (result != m_attributes.end()) {
				return (*result);
			}
		}

		return shared_ptr<CSimpleXMLAttribute>(nullptr);
	}

	shared_ptr<CSimpleXMLAttribute> CSimpleXMLNode::PopAttribute(const CString& strAttributeName)
	{
		if (HasAttributes())
		{
			auto result = _findAttribute(strAttributeName);
			if (result != m_attributes.end()) {
				shared_ptr<CSimpleXMLAttribute> retVal = (*result);
				m_attributes.erase(result);
				return retVal;
			}
		}

		return shared_ptr<CSimpleXMLAttribute>(nullptr);
	}

	vector<shared_ptr<CSimpleXMLNode>>::iterator CSimpleXMLNode::_findChildNode(const CString& strNodeName)
	{	
		for (auto iter = m_children.begin(); iter != m_children.end(); ++iter) {
			if ((*iter)->strNodeName == strNodeName) {
				return iter;
			}
		}

		return m_children.end();
	}

	vector<shared_ptr<CSimpleXMLAttribute>>::iterator CSimpleXMLNode::_findAttribute(const CString& strAttributeName)
	{
		for (auto iter = m_attributes.begin(); iter != m_attributes.end(); ++iter) {
			if ((*iter)->strAttribName == strAttributeName) {
				return iter;
			}
		}

		return m_attributes.end();
	}

	CString CSimpleXMLNode::ToString() const
	{
		CString str;
		str = _T("<") + strNodeName;
		
		if (HasAttributes())
		{
			str += _T("\n");
			for (auto iter = m_attributes.begin(); iter != m_attributes.end(); ++iter)
			{
				str +=  iter->get()->ToString();
			}

			str.TrimRight();
		}

		if (HasChildren())
		{
			str  += _T(">");

			str += _T("\n");

			for (auto iter = m_children.begin(); iter != m_children.end(); iter++)
			{
				str += iter->get()->ToString();
			}

			str += _T("</") + strNodeName + _T(">\n");
		}
		else
		{
			if (strNodeValue.IsEmpty()) {
				str += _T("/>\n");
			}
			else {
				str += _T(">") + strNodeValue;
				str += _T("</") + strNodeName + _T(">\n");
			}
		}

		return str;
	}

	CSimpleXML::CSimpleXML()
	{
	}


	CSimpleXML::~CSimpleXML()
	{
	}

	CString CSimpleXML::ToString() const
	{
		if (pRoot) {
			auto pActualRoot = pRoot->GetFirstChild();	
			return _T("<?") + docString + _T("?>\n") + pActualRoot->get()->ToString();
		}
			

		else
			return _T("EMPTY");
	}

	BOOL CSimpleXML::LoadXML(const CString& strFilePath)
	{
		CString strXML;
		BOOL bResult = _openFile(strFilePath, strXML);
		if (bResult)
		{
			bResult = ParseXML(strXML);
		}

		return bResult;
	}

	BOOL CSimpleXML::SaveXML(const CString& strFilePath)
	{
		CStdioFile File;
		BOOL bResult = File.Open(strFilePath, CFile::modeWrite | CFile::modeCreate);

		if (bResult)
		{
			File.WriteString(ToString());
			File.Close();
		}

		return bResult;
	}

	BOOL CSimpleXML::ParseXML(const CString& strXML)
	{
		return _parseXMLString(strXML);
	}

	shared_ptr<CSimpleXMLNode> CSimpleXML::GetRootNode() const
	{
		return pRoot;
	}

	BOOL CSimpleXML::_openFile(__in const CString& strFilePath, __out CString& strFileData)
	{
		CStdioFile File;
		BOOL bResult = File.Open(strFilePath, CFile::modeRead);

		if (bResult)
		{
			CString strBuffer;

			while (File.ReadString(strBuffer)) {
				strFileData.Append(strBuffer+_T("\n"));
			}

			File.Close();
		}

		return bResult;
	}

	BOOL CSimpleXML::_parseXMLString(const CString& strXML, int start)
	{
		CSimpleXMLNode *pRootNode = new CSimpleXMLNode(_T("ROOT"));
		pRoot = shared_ptr<CSimpleXMLNode>(pRootNode);

		return _parse(strXML, start, pRootNode);
	}

	BOOL CSimpleXML::_parse(const CString& strXML, int& idx, CSimpleXMLNode* pParent)
	{
		ASSERT(pParent);

		TCHAR c;
		idx -= 1;

		BOOL bResult(TRUE);

		enum ParserState
		{
			PS_NONE			= 0x00,
			PS_DOCSTRING	= 0x01,
			PS_TAG_OPEN		= 0x02,
			PS_TAG_CLOSE	= 0x04,
			PS_TAG_VALUE	= 0x08,
			PS_ATTRIBUTE	= 0x10,
		};

		DWORD dwState(PS_NONE);
		CString strBuffer;
		CSimpleXMLNode* pCurrentNode(nullptr);
		BOOL bAbort = FALSE;

		int nSize = strXML.GetLength();

		while (++idx < nSize)
		{
			c = strXML[idx];

			if (bAbort) break;

			switch (c)
			{
				case _T('<'):
				{
					switch (strXML[idx + 1])
					{
						case _T('?'): dwState |= PS_DOCSTRING; ++idx; break;
						case _T('/'): 
							if (dwState & PS_TAG_VALUE) {
								dwState ^= PS_TAG_VALUE;
								pCurrentNode->strNodeValue = strBuffer.Trim();
							} else if (dwState == PS_NONE) {
								bAbort = TRUE;	//we reached the end of this level of the dom tree
								break;
							}
							dwState |= PS_TAG_CLOSE; 
							++idx; 
							break;
						default:
							if (dwState & PS_TAG_VALUE) { //if we find another opening tag while we expect a value -> recurse
								_parse(strXML, idx, pCurrentNode);
								dwState |= PS_TAG_CLOSE;
							}
							else {
								dwState |= PS_TAG_OPEN;
							}
							break;
					}
					strBuffer.Empty();
					break;
				}
				case _T('?'):
					if (dwState & PS_DOCSTRING) {
						if (strXML[idx + 1] == _T('>')) {
							++idx;
							docString = strBuffer;
							strBuffer.Empty();
							dwState = PS_NONE;
						}
					}
					break;
				case _T('>'):
				{
					if (dwState & PS_TAG_OPEN) {
						dwState ^= PS_TAG_OPEN;
						dwState |= PS_TAG_VALUE;
						pCurrentNode = new CSimpleXMLNode(strBuffer);
						pParent->AddChildNode(pCurrentNode);
					}
					else if (dwState & PS_TAG_CLOSE){
						if (pCurrentNode->strNodeName != strBuffer) { //Check if the closing tag matches the opening tag. if not -> bad XML
							bResult = FALSE;
							bAbort = TRUE;
						}
						dwState ^= PS_TAG_CLOSE;
					}

					strBuffer.Empty();
					break;
				}
				case _T('\t'):
				case _T('\n'):
				case _T('\r'):
				case _T(' '): //we find a white space
				{
					if (dwState & PS_TAG_OPEN) {
						//parse attributes here
						pCurrentNode = new CSimpleXMLNode(strBuffer);
						pParent->AddChildNode(pCurrentNode);
						strBuffer.Empty();
						_parseAttributes(strXML, idx, pCurrentNode);
						dwState ^= PS_TAG_OPEN;

						if (strXML[idx - 1] != _T('/') )
							dwState |= PS_TAG_VALUE;
					}
					else if (dwState & PS_TAG_VALUE) {
						strBuffer += c;
					}
					break;
				}
				default:
					strBuffer += c;
			}
		}

		return bResult;
	}

	BOOL CSimpleXML::_parseAttributes(const CString& strXML, int& idx, CSimpleXMLNode* pParent)
	{
		int size = strXML.GetLength();
		TCHAR c;

		enum ATTR_STATUS
		{
			AS_NAME			= 0x01,
			AS_VALUE		= 0x02,
			AS_LARGE_VALUE	= 0x04,
		};

		DWORD dwStatus(AS_NAME);
		BOOL bResult = TRUE;
		BOOL bRun = TRUE;

		CString strAttrName, strAttrVal;

		auto AppenAttribute = [](CSimpleXMLNode* pParent, CString& attrName, CString& attrVal, BOOL bLargeVal) -> 
			void	{	pParent->AddAttribute(attrName, attrVal, bLargeVal);
						attrName.Empty();
						attrVal.Empty();
					};

		while (idx < size && bResult && bRun)
		{
			c = strXML[++idx];

			switch (c)
			{
			case _T('='):
				if (dwStatus & AS_NAME) {
					dwStatus = AS_VALUE;
				}
				else {
					bResult = FALSE;
				}
				break;
			case _T('\t'):
			case _T('\n'):
			case _T(' '):
				if (dwStatus & AS_LARGE_VALUE) {
					strAttrVal += c;
				}
				else if(dwStatus & AS_VALUE) {
					AppenAttribute(pParent, strAttrName, strAttrVal, FALSE);
					dwStatus = AS_NAME;
				}
				break;
			case _T('\''):
			case _T('\"'):
				if (dwStatus & AS_VALUE) {
					if (dwStatus & AS_LARGE_VALUE)
					{
						AppenAttribute(pParent, strAttrName, strAttrVal, TRUE);
						dwStatus = AS_NAME;
						break;
					}
					else if (strAttrVal.IsEmpty()) 
					{
						dwStatus |= AS_LARGE_VALUE;
						break;
					}
				}
				bResult = FALSE;
				break;

			case _T('>'):
				if (dwStatus & AS_VALUE) {
					AppenAttribute(pParent, strAttrName, strAttrVal, FALSE);
				}
				bRun = FALSE;
				break;
			default:
				if (dwStatus & AS_NAME)
				{
					strAttrName += c;
				} 
				else
				{
					strAttrVal += c;
				}
				break;
			}
		}

		return bResult;
	}
}
