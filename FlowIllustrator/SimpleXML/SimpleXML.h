#pragma once
#include <vector>
#include <memory>

using namespace std;

namespace simplexml
{
	class CSimpleXMLAttribute
	{
	public:
		CSimpleXMLAttribute(const CString& attribName, const CString& attribValue, BOOL bLargeVal = FALSE);

	public:
		CString strAttribName;
		CString strAttribValue;
		BOOL bLargeValue;

	public:
		CString ToString() const;
	};

	class CSimpleXMLNode
	{
	public:
		CSimpleXMLNode(const CString& nodeName, const CString& nodeValue);
		CSimpleXMLNode(const CString& nodeName);

	public:
		CString strNodeName;
		CString strNodeValue;
		vector<shared_ptr<CSimpleXMLNode>> m_children;
		vector<shared_ptr<CSimpleXMLAttribute>> m_attributes;

		void AddAttribute(const CString& attribName, const CString& attribValue, BOOL bLargeVal);
		void AddAttribute(CSimpleXMLAttribute* pNewAttribute);

		void AddChildNode(const CString& nodeName, const CString& nodeValue);
		void AddChildNode(CSimpleXMLNode* pNewChildNode);

		BOOL HasAttributes() const;
		vector<shared_ptr<CSimpleXMLAttribute>>::iterator GetFirstAttribute();
		vector<shared_ptr<CSimpleXMLAttribute>>::iterator GetLastAttribute();
		shared_ptr<CSimpleXMLAttribute> FindAttribute(const CString& strAttributeName);
		shared_ptr<CSimpleXMLAttribute> PopAttribute(const CString& strAttributeName);


		BOOL HasChildren() const;
		vector<shared_ptr<CSimpleXMLNode>>::iterator GetFirstChild();
		vector<shared_ptr<CSimpleXMLNode>>::iterator GetLastChild();
		shared_ptr<CSimpleXMLNode> FindChildNode(const CString& strNodeName);
		shared_ptr<CSimpleXMLNode> PopChildNode(const CString& strNodeName);

		CString ToString() const;

	private:
		vector<shared_ptr<CSimpleXMLNode>>::iterator _findChildNode(const CString& strNodeName);
		vector<shared_ptr<CSimpleXMLAttribute>>::iterator _findAttribute(const CString& strAttributeName);
	};


	class CSimpleXML
	{
	private:
		shared_ptr<CSimpleXMLNode>	pRoot;
		CString docString;

	public:
		CSimpleXML();
		~CSimpleXML();

	public:
		BOOL LoadXML(const CString& strFilePath);
		BOOL SaveXML(const CString& strFilePath);
		BOOL ParseXML(const CString& strXML);

		shared_ptr<CSimpleXMLNode> GetRootNode() const;

		CString ToString() const;

	private:
		BOOL _openFile(__in const CString& strFilePath, __out CString& strFileData);
		BOOL _parseXMLString(const CString& strXML, int start = 0);
		BOOL _parse(const CString& strXML, int& idx, CSimpleXMLNode* pParent);
		BOOL _parseAttributes(const CString& strXML, int& idx, CSimpleXMLNode* pParent);
	};
};