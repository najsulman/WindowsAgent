//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// RequestAbstract.cpp: implementation of the CRequestAbstract class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RequestAbstract.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRequestAbstract::CRequestAbstract()
{
	m_pLogger = getOcsLogger();
	m_pDeviceid = getOcsDeviceID();
	m_bFinal = FALSE;
    m_pRawMessage = NULL;

	/****
	 *
	 *	XML beginning generation
	 *
	 ****/
	m_pRequestXmlNode = m_cmXml.AddElem( _T( "REQUEST"));
	m_cmXml.AddChildElem( _T( "DEVICEID"), m_pDeviceid->getDeviceID());
	m_pContentXmlNode = m_cmXml.AddChildElem( _T( "CONTENT")); 
}

CRequestAbstract::~CRequestAbstract()
{
	if (m_pRawMessage != NULL)
		delete m_pRawMessage;
}

/**
 *	Return the compressed sent message length
 */
DWORD CRequestAbstract::getRawMessageLength()
{
	if (m_pRawMessage == NULL)
		return 0;
	else
		return m_pRawMessage->GetSize();
}

/**
 *	Removes any binary data from the xml markup
 */
void CRequestAbstract::cleanXml()
{
/*	CStringA bef = m_cmXml.GetDoc();
	
	for (int i=0;i<bef.GetLength();i++)
	{	
		UCHAR cut = (UCHAR)bef.GetAt(i);
		if( (cut<32 || cut>244) && cut!=10 && cut!=13 )
			bef.SetAt(i,'x');		
	}
	m_cmXml.SetDoc( bef);
*/
}

/**
 *	Final operations before a request can be sent
 */
BOOL CRequestAbstract::final()
{
	try
	{
		if(!m_bFinal)
		{
			cleanXml();
/* TO FIX
			m_pLogger->log( LOG_PRIORITY_TRACE, GetUnicodeFromUTF8( getMessage()));
*/
			m_pRawMessage = CZip::deflate( getMessage());
			m_bFinal = TRUE;
			return (m_pRawMessage != NULL);
		}
		else
			return FALSE;
	}
	catch (CException *pEx)
	{
		m_pLogger->log( LOG_PRIORITY_ERROR, _T( "REQUEST => Generic deflate error"));
		pEx->Delete();
		return FALSE;
	}
}

/**
 *	Set the query name and optional type
 */
BOOL CRequestAbstract::setQuery( LPCTSTR lpstrQuery, LPCTSTR lpstrType)
{
	m_cmXml.ResetPos( m_pRequestXmlNode); 

	if (!m_cmXml.FindFirstElem( _T("QUERY")))
	{
		if (!m_cmXml.AddChildElem( _T( "QUERY"), lpstrQuery))
			return FALSE;
		if (lpstrType != NULL)
			m_cmXml.AddChildElem( _T( "TYPE"), lpstrType);
	}
	return TRUE;
}

/**
 *	Return a pointer to our global Markup
 */
CMarkup* CRequestAbstract::getXmlPointer()
{
	m_cmXml.ResetPos();
	return &m_cmXml;
};

/**
 *	Return a pointer to our request
 */
CMarkup* CRequestAbstract::getXmlPointerRequest()
{
	m_cmXml.ResetPos( m_pRequestXmlNode);
	return &m_cmXml;
};

/**
 *	Return a pointer to our request content
 */
CMarkup* CRequestAbstract::getXmlPointerContent()
{
	m_cmXml.ResetPos( m_pContentXmlNode);
	return &m_cmXml;
};
