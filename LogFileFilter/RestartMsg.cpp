#include "stdafx.h"
#include "RestartMsg.h"


void strRestartMsgstr(CString strTime, map<CString, CString> m_map, CString strOutput)
{
	if(!m_map.empty())
	{
		m_map.clear();
	}
	strOutput += strTime + "ʱ���������";
}

void strRestartMsgint(CString strTime, map<CString, int> m_map, CString strOutput)
{
	if(!m_map.empty())
	{
		m_map.clear();
	}
	strOutput = strTime + "ʱ���������";
}