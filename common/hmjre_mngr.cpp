//	$Id: hmjre_mngr.cpp,v 1.8 2009/09/20 13:49:01 sugiura Exp $
/*
 *	hmjre_mngr.cpp
 *	HmJre_Manager クラスの実装
 */

#include "hmjre_mngr.h"


HmJre_Manager::HmJre_Manager(const StringBuffer& filename)
	:	REDLL_Manager(filename)
	,	m_pLastJre2(NULL)
	,	m_pLastFuzzyData(NULL)
	,	m_bLastIsFuzzy(FALSE)
{
	m_pfnJreGetVersion = (PFN_JREGETVERSION)::GetProcAddress(m_hModuleDll,HMJRE_JREGETVERSION);

	m_pfnJreGetTagPosition			= (PFN_JREGETTAGPOSITION)::GetProcAddress(m_hModuleDll, HMJRE_JREGETTAGPOSITION);
	m_pfnJre2Open					= (PFN_JRE2OPEN)::GetProcAddress(m_hModuleDll, HMJRE_JRE2OPEN);
	m_pfnJre2Close					= (PFN_JRE2CLOSE)::GetProcAddress(m_hModuleDll, HMJRE_JRE2CLOSE);
	m_pfnJre2Compile				= (PFN_JRE2COMPILE)::GetProcAddress(m_hModuleDll, HMJRE_JRE2COMPILE);
	m_pfnJre2GetMatchInfo			= (PFN_JRE2GETMATCHINFO)::GetProcAddress(m_hModuleDll, HMJRE_JRE2GETMATCHINFO);
	m_pfnFuzzy_Open					= (PFN_FUZZY_OPEN)::GetProcAddress(m_hModuleDll, HMJRE_FUZZY_OPEN);
	m_pfnFuzzy_Close				= (PFN_FUZZY_CLOSE)::GetProcAddress(m_hModuleDll, HMJRE_FUZZY_CLOSE);
	m_pfnFuzzy_ConvertTarget		= (PFN_FUZZY_CONVERTTARGET)::GetProcAddress(m_hModuleDll, HMJRE_FUZZY_CONVERTTARGET);
	m_pfnFuzzy_ConvertFindString	= (PFN_FUZZY_CONVERTFINDSTRING)::GetProcAddress(m_hModuleDll, HMJRE_FUZZY_CONVERTFINDSTRING);
	m_pfnFuzzy_FindPos2RealPos		= (PFN_FUZZY_FINDPOS2REALPOS)::GetProcAddress(m_hModuleDll, HMJRE_FUZZY_FINDPOS2REALPOS);
	m_pfnFuzzy_FindArea2RealArea	= (PFN_FUZZY_FINDAREA2REALAREA)::GetProcAddress(m_hModuleDll, HMJRE_FUZZY_FINDAREA2REALAREA);
	m_pfnFuzzy_RealPos2FindPos		= (PFN_FUZZY_REALPOS2FINDPOS)::GetProcAddress(m_hModuleDll, HMJRE_FUZZY_REALPOS2FINDPOS);
	m_pfnFuzzy_GetFuzzyDataInJre	= (PFN_FUZZY_GETFUZZYDATAINJRE)::GetProcAddress(m_hModuleDll, HMJRE_FUZZY_GETFUZZYDATAINJRE);
	m_pfnFuzzy_OptionDialog			= (PFN_FUZZY_OPTIONDIALOG)::GetProcAddress(m_hModuleDll, HMJRE_FUZZY_OPTIONDIALOG);
	m_pfnFindRegular				= (PFN_FINDREGULAR)::GetProcAddress(m_hModuleDll, HMJRE_FINDREGULAR);
	m_pfnFindRegularNoCaseSense		= (PFN_FINDREGULARNOCASESENSE)::GetProcAddress(m_hModuleDll, HMJRE_FINDREGULARNOCASESENSE);
	m_pfnGetLastMatchLength			= (PFN_GETLASTMATCHLENGTH)::GetProcAddress(m_hModuleDll, HMJRE_GETLASTMATCHLENGTH);
	m_pfnEnvChanged					= (PFN_ENVCHANGED)::GetProcAddress(m_hModuleDll, HMJRE_ENVCHANGED);
	m_pfnGetLastMatchTagPosition	= (PFN_GETLASTMATCHTAGPOSITION)::GetProcAddress(m_hModuleDll, HMJRE_GETLASTMATCHTAGPOSITION);
	m_pfnGetLastMatchTagLength		= (PFN_GETLASTMATCHTAGLENGTH)::GetProcAddress(m_hModuleDll, HMJRE_GETLASTMATCHTAGLENGTH);
	m_pfnReplaceRegular				= (PFN_REPLACEREGULAR)::GetProcAddress(m_hModuleDll, HMJRE_REPLACEREGULAR);
	m_pfnReplaceRegularNoCaseSense	= (PFN_REPLACEREGULARNOCASESENSE)::GetProcAddress(m_hModuleDll, HMJRE_REPLACEREGULARNOCASESENSE);

	if (m_pfnJreGetVersion == NULL				||
		m_pfnJre2Open == NULL					||
		m_pfnJre2Close == NULL					||
		m_pfnJre2Compile == NULL				||
		m_pfnJre2GetMatchInfo == NULL			||
		m_pfnJreGetTagPosition == NULL			||
		m_pfnFuzzy_Open == NULL					||
		m_pfnFuzzy_Close == NULL				||
		m_pfnFuzzy_ConvertTarget == NULL		||
		m_pfnFuzzy_ConvertFindString == NULL	||
		m_pfnFuzzy_FindPos2RealPos == NULL		||
		m_pfnFuzzy_FindArea2RealArea == NULL	||
		m_pfnFuzzy_RealPos2FindPos == NULL		||
		m_pfnFuzzy_GetFuzzyDataInJre == NULL	||
		m_pfnFuzzy_OptionDialog == NULL)
	{
		throw DllProcAddressNotFoundError();
	}
}

HmJre_Manager::~HmJre_Manager()
{
	m_Jre2_Pool.initSequentialGet();
	JRE2*	pJre2;
	while ((pJre2 = m_Jre2_Pool.getNextItem()))
	{
		(*m_pfnJre2Close)(pJre2);
	}

	if (m_pLastFuzzyData != NULL)
	{
		m_pfnFuzzy_Close(m_pLastFuzzyData);
		delete m_pLastFuzzyData;
	}
}

//	return "pos:len"
StringBuffer
HmJre_Manager::match(const StringBuffer& strFind, const StringBuffer& strTarget, int nOffset, int nFlags, int nRegExp)
{
	if (m_pLastFuzzyData != NULL)
	{
		m_pfnFuzzy_Close(m_pLastFuzzyData);
		delete m_pLastFuzzyData;
		m_pLastFuzzyData = NULL;
	}

	if ((nFlags & FUZZYOPTION_NORETURN) != 0)
	{
		int	nBreaks = strTarget.count("\n");
		if (nBreaks <= 0)
		{
			nFlags	&= ~FUZZYOPTION_NORETURN;
		}
		else
		{
			if (nBreaks > 0x100)
			{
				nBreaks = 0x100;
			}

			nFlags	|= (nBreaks - 1) << FUZZYOPTION_NORETURN_SHIFT;
		}
	}

	BOOL	bFuzzy = (nRegExp >= 0);

	int	nPos = -1, nLength = 0;

	if (bFuzzy)
	{
		JREFUZZYDATA*	pFuzzyData = new JREFUZZYDATA;

		memset(pFuzzyData, 0, sizeof(*pFuzzyData));
		pFuzzyData->dwSize	= sizeof(*pFuzzyData);

		BOOL	bRet = m_pfnFuzzy_Open(pFuzzyData, FALSE);
		if (!bRet)
		{
			delete pFuzzyData;
			return nullStr;
		}

		m_pLastFuzzyData	= pFuzzyData;

		pFuzzyData->flags	= nFlags;

		bRet = m_pfnFuzzy_ConvertFindString(pFuzzyData, strFind, nRegExp);
		if (!bRet)
		{
			m_pfnFuzzy_Close(pFuzzyData);
			return nullStr;
		}

		bRet = m_pfnFuzzy_ConvertTarget(pFuzzyData, strTarget);
		if (!bRet)
		{
			m_pfnFuzzy_Close(pFuzzyData);
			return nullStr;
		}

		JRE2*	pJre2 = GetJre2(pFuzzyData->pszFindConved, 1);	//	常に case sense
		if (pJre2 == NULL)
		{
			m_pfnFuzzy_Close(pFuzzyData);
			return nullStr;
		}

		pJre2->nStart	= m_pfnFuzzy_RealPos2FindPos(pFuzzyData, nOffset);

		bRet = m_pfnJre2GetMatchInfo(pJre2, pFuzzyData->pszTargetConved);
		if (bRet)
		{
			m_pLastJre2			= pJre2;
			m_pLastFuzzyData	= pFuzzyData;
			m_bLastIsFuzzy		= TRUE;
			nLength	= pJre2->nLength;
			nPos	= m_pfnFuzzy_FindArea2RealArea(pFuzzyData, pJre2->nPosition, &nLength);
		}
		else
		{
			m_pfnFuzzy_Close(pFuzzyData);
		}
	}
	else
	{
		JRE2*	pJre2 = GetJre2(strFind, 1);	//	常に case sense
		if (pJre2 == NULL)
		{
			return nullStr;
		}

		pJre2->nStart	= nOffset;

		BOOL	bRet = m_pfnJre2GetMatchInfo(pJre2, strTarget);
		if (bRet)
		{
			m_pLastJre2		= pJre2;
			m_bLastIsFuzzy	= FALSE;
			nLength	= pJre2->nLength;
			nPos	= pJre2->nPosition;
		}
	}

	return make_pos_result(nPos, nLength);
}

//	return "pos:len"
StringBuffer
HmJre_Manager::getTagPosition(int nTagNumber)
{
	if (m_pLastJre2 == NULL || nTagNumber < 1 || nTagNumber > 9)
	{
		return nullStr;
	}

	int	nLength = 0;
	int	nPos = m_pfnJreGetTagPosition(m_pLastJre2, (char)('0' + nTagNumber), &nLength);

	if (nPos >= 0 && m_pLastFuzzyData != NULL && m_bLastIsFuzzy)
	{
		nPos = m_pfnFuzzy_FindArea2RealArea(m_pLastFuzzyData, nPos, &nLength);
	}

	return make_pos_result(nPos, nLength);
}

StringBuffer
HmJre_Manager::getMatchString(const StringBuffer& strTarget, const StringBuffer& strMatchResult)
{
	int	nPos, nLength;

	get_pos_and_length_from_pos_result(strMatchResult, nPos, nLength);

	if (nPos == -1 || nLength == 0)
	{
		return nullStr;
	}

	return strTarget.extract(nPos, nLength);
}

int
HmJre_Manager::findRegular(const StringBuffer& strRE, const StringBuffer& strTarget, int nOffset)
{
	if (m_pfnFindRegular == NULL)
	{
		return -3;
	}

	return m_pfnFindRegular(strRE, strTarget, nOffset);
}

int
HmJre_Manager::findRegularNoCaseSense(const StringBuffer& strRE, const StringBuffer& strTarget, int nOffset)
{
	if (m_pfnFindRegularNoCaseSense == NULL)
	{
		return -3;
	}

	return m_pfnFindRegularNoCaseSense(strRE, strTarget, nOffset);
}

int
HmJre_Manager::getLastMatchLength()
{
	if (m_pfnGetLastMatchLength == NULL)
	{
		return -1;
	}

	return m_pfnGetLastMatchLength();
}

int
HmJre_Manager::envChanged()
{
	if (m_pfnEnvChanged == NULL)
	{
		return -1;
	}

	return m_pfnEnvChanged();
}

int
HmJre_Manager::getLastMatchTagPosition(int nTagNumber)
{
	if (m_pfnGetLastMatchTagPosition == NULL)
	{
		return -1;
	}

	return m_pfnGetLastMatchTagPosition(nTagNumber);
}

int
HmJre_Manager::getLastMatchTagLength(int nTagNumber)
{
	if (m_pfnGetLastMatchTagLength == NULL)
	{
		return -1;
	}

	return m_pfnGetLastMatchTagLength(nTagNumber);
}

int
HmJre_Manager::optionDialog(int nHwnd, int nDisableFlags)
{
	return m_pfnFuzzy_OptionDialog((HWND)nHwnd, nDisableFlags);
}

const StringBuffer&
HmJre_Manager::jreGetVersion()
{
	if (m_strVersion.length() == 0)
	{
		WORD wVer = (*m_pfnJreGetVersion)();
		char buf[80];
		wsprintf(buf, "%01x.%02x", (wVer >> 8), (wVer & 0xFF));
		m_strVersion = buf;
	}

	return m_strVersion;
}

JRE2*
HmJre_Manager::GetJre2(const StringBuffer& strRE, int nCaseSense)
{
	JRE2*	pJre2 = m_htblJre2.getValue(strRE);
	if (pJre2 == NULL)
	{
		pJre2	= new JRE2;

		memset(pJre2, 0, sizeof(*pJre2));
		pJre2->dwSize	= sizeof(*pJre2);

		BOOL	bRet = m_pfnJre2Open(pJre2);
		if (!bRet)
		{
			delete pJre2;
			return NULL;
		}

		pJre2->wTranslate	= -1;	//	下記でコンパイルさせるため

		m_htblJre2.setValue(strRE, pJre2);
		m_Jre2_Pool.addItem(pJre2);
	}

	if (pJre2->wTranslate != (nCaseSense == 0))
	{
		pJre2->wTranslate	= (nCaseSense == 0);

		BOOL	bRet = m_pfnJre2Compile(pJre2, strRE);
		if (!bRet)
		{
			return NULL;
		}
	}

	return pJre2;
}

StringBuffer
HmJre_Manager::replaceRegular(const StringBuffer& strPattern, const StringBuffer& strTarget, int nStart, const StringBuffer& strReplace, int fReplaceAll)
{
	if (m_pfnReplaceRegular == NULL)
	{
		return nullStr;
	}

	return m_pfnReplaceRegular(strPattern, strTarget, nStart, strReplace, fReplaceAll);
}

StringBuffer
HmJre_Manager::replaceRegularNoCaseSense(const StringBuffer& strPattern, const StringBuffer& strTarget, int nStart, const StringBuffer& strReplace, int fReplaceAll)
{
	if (m_pfnReplaceRegularNoCaseSense == NULL)
	{
		return nullStr;
	}

	return m_pfnReplaceRegularNoCaseSense(strPattern, strTarget, nStart, strReplace, fReplaceAll);
}

