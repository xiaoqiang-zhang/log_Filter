// PoorSperation.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LogFileFilter.h"
#include "PoorSperation.h"
#include "afxdialogex.h"
#include "LogFile.h"
#include "LogHandle.h"
#include "MainFrm.h"
#include "LogFileFilterDoc.h"
#include "LogFileFilterView.h"


// CPoorSperation �Ի���

IMPLEMENT_DYNAMIC(CPoorSperation, CDialog)

CPoorSperation::CPoorSperation(CWnd* pParent /*=NULL*/)
	: CDialog(CPoorSperation::IDD, pParent)
{
	pFileIpdbg = new CFile;
}

CPoorSperation::~CPoorSperation()
{
	delete pFileIpdbg;
}

void CPoorSperation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGTIMESPANPOOR, m_LogTimeSpanPoor);
	DDX_Control(pDX, IDC_QUERYTIMESTARTPOOR, m_PoorQueryTimeStart);
	DDX_Control(pDX, IDC_QUERYTIMESTOPPOOR, m_PoorQueryTimeStop);
}


BEGIN_MESSAGE_MAP(CPoorSperation, CDialog)
	ON_BN_CLICKED(IDC_CHECKPOOR, &CPoorSperation::OnClickedCheckpoor)
	ON_BN_CLICKED(IDC_CANCELPOOR, &CPoorSperation::OnClickedCancelpoor)
END_MESSAGE_MAP()


// CPoorSperation ��Ϣ�������


BOOL CPoorSperation::OnInitDialog()
{
	CDialog::OnInitDialog();
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pView = (CLogFileFilterView *)pMain->GetActiveView();
	if(pView->strIpdbgFilePath.IsEmpty())
	{
		MessageBox("open file failed");
		return FALSE;
	}
	pFileIpdbg->Open(pView->strIpdbgFilePath, CFile::modeRead | CFile::typeBinary);
	int resSetTime = CLogFileOperation::SetLogTimeWidthAndDefaultQueryTime(*pFileIpdbg, m_LogTimeSpanPoor, m_PoorQueryTimeStart, m_PoorQueryTimeStop);
	if (!resSetTime)
	{
		if (resSetTime == -1)
		{
			MessageBox("Open file failed!");
		}
		else
		{
			MessageBox("Read file error, Please confirm the record!");
		}
		return FALSE;
	}
	pFileIpdbg->SeekToBegin();
	pMain = NULL;
	pView = NULL;
	delete pView;
	delete pMain;
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CPoorSperation::OnClickedCheckpoor()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialog::OnOK();
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pView = (CLogFileFilterView *)pMain->GetActiveView();
	CLogFileOperation::GetQueryTimeWin(m_PoorQueryTimeStart, m_PoorQueryTimeStop, m_TimeStart, m_TimeStop);
	if(m_TimeStart > m_TimeStop)
	{
		MessageBox("��ʼʱ����ڽ���ʱ�䣬����������");
		return;
	}
	pView->m_pListBox->AddString("������·������Ϣ:");
	GetQueryResult();
	pMain = NULL;
	pView = NULL;
	delete pView;
	delete pMain;
}

void CPoorSperation::OnClickedCancelpoor()
{
	CDialog::OnCancel();
}

void CPoorSperation::GetQueryResult(void)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pMyView = (CLogFileFilterView *)pMain->GetActiveView();
	//pMyView->pfileIPDBG->Open(pMyView->strIpdbgFilePath, CFile::modeRead | CFile::typeBinary);
	REPLAY_DATA dataReplay;
	struct BADSHUNTINFO BadShuntInfoTmp;
	char strType[4] = {0}, strTypeMap[4] = {0}, bufConfirmInfo[10] = {0};
	int nNum = 0;
	CString strQueryRes, strConfirmInfo, strPoorType;
	while(pFileIpdbg->GetPosition() < pFileIpdbg->GetLength())
	{
		int readSize = CReadLogFile::ReadReplayDataFromLog(*pFileIpdbg, dataReplay);
		if (readSize <= 0)
		{
			MessageBox("Read log record error! Please Confirm that the record is complete");
			return;
		}
		CTime logTime(dataReplay.m_time);
		strJudge = (CString)dataReplay.m_data;
		int nFirstWin = strJudge.Find("[RPYBAD]");
		char bufTmpTime[100] = {0};
		CString strTmpTime, strStationContentTmp;
		CLogFileOperation::ChangeToTime(dataReplay.m_time, bufTmpTime, 100);
		strTmpTime.Format("%s", bufTmpTime);
		char strTmp[100] = {0};
		BOOL m_BADflag = FALSE;
		if(logTime < m_TimeStart){continue;}
		if (logTime > m_TimeStop){break;}
		if(strJudge.Find("[USER]: create queue,USER.queue_handle") >= 0)
		{
			if(!mapPoorSperation.empty())
			{
				mapPoorSperation.clear();
			}
			nNum = 0;
			CString strResatrtRes = strTmpTime + "ʱ���������";
			pMyView->m_pListBox->AddString(strResatrtRes);
		}
		if(nFirstWin >= 0)
		{
			nNum++;
			strData = strJudge.Mid(nFirstWin + strlen("[RPYTID]"), 
				dataReplay.m_len - nFirstWin - strlen("[RPYTID]"));
			strStationNum = strData.Left(4);	// 4�ֽ�վ��
			strStationEquipmentType = strData.Mid(4, 4);	// 4�ֽ�վ���豸����
			strStationEquipmentID = strData.Mid(8, 4);	// 4�ֽ�վ���豸ID
			strPoorSperationType = strData.Mid(12, 4);	// 4�ֽڷ�·��������
			strPoorSperationConfirm = strData.Mid(16, 4);	// 4�ֽ�ȷ����Ϣ
			CLogFileOperation::ChangeToTime(dataReplay.m_time, bufTmpTime, 100);
			strTmpTime.Format("%s", bufTmpTime);
			sprintf_s(strTmp, sizeof(strTmp), "%d-%d-%d", strtol(strStationNum, NULL, 16), 
			strtol(strStationEquipmentType, NULL, 16), strtol(strStationEquipmentID, NULL, 16));
			//mapPoorSperation���Ƿ��и��豸�ļ�¼
			iter = mapPoorSperation.find((CString)strTmp);
			//int ret = strPoorSperationConfirm.Replace("\r\n", "");
			mapNum = strtol(strPoorSperationType, NULL, 16);
			if(mapPoorSperation.empty())
			{
				strStationContentTmp = "������·����";
				BadShuntInfoTmp.badShuntType = mapNum;
				BadShuntInfoTmp.badShuntConfirm = GetBadShuntConfirmInfo(strPoorSperationConfirm);
				mapPoorSperation[(CString)strTmp] = BadShuntInfoTmp;
				strPoorType = JudgeBADType(mapNum);
				if(strPoorType.IsEmpty())
				{
					MessageBox("ʶ���¼��������ʧ�ܣ�");
				}
				strQueryRes = strTmpTime + " �豸(" + (CString)strTmp + ")" +
					strStationContentTmp + strPoorType + BadShuntInfoTmp.badShuntConfirm;
			}
			else
			{	// mapPoorSperation���ڸ��豸�ļ�¼ʱ
				if(iter != mapPoorSperation.end())
				{
					if (iter->second.badShuntType != 0 && !iter->second.badShuntConfirm.IsEmpty() && mapNum == 0)
					{
						strStationContentTmp = "ɾ����·����";	// BADSHUNTINFO��Ϊ�գ�����·��������Ϊ�㣬��Ϊɾ����·����
						iter->second.badShuntType = mapNum;
						iter->second.badShuntConfirm = GetBadShuntConfirmInfo(strPoorSperationConfirm);	// badShuntConfirm��Ϊ��
					}
					else if (iter->second.badShuntType != 0 && mapNum != 0 &&
						iter->second.badShuntType != mapNum)	// ��Ϊ�㣬�Ҳ����
					{
						strStationContentTmp = "�����·����";
						iter->second.badShuntType = mapNum;
						iter->second.badShuntConfirm = GetBadShuntConfirmInfo(strPoorSperationConfirm);
					}
					else if (iter->second.badShuntType != 0 && mapNum != 0 && iter->second.badShuntType == mapNum && 
						iter->second.badShuntConfirm.Compare(GetBadShuntConfirmInfo(strPoorSperationConfirm)))
					{	// ȷ����Ϣ��ͬ����ȷ����Ϣ���¸�ֵ
						iter->second.badShuntConfirm = GetBadShuntConfirmInfo(strPoorSperationConfirm);
					}
					else if (iter->second.badShuntType == 0 && !iter->second.badShuntConfirm.IsEmpty()
						&& iter->second.badShuntConfirm.IsEmpty() && mapNum != 0)
					{	// ���ڸ��豸����֮ǰ�ķ�·������Ϣ��ɾ�����������˸��豸�ļ�¼,��ʱ��Ϊ��������·����
						strStationContentTmp = "������·����";
						iter->second.badShuntType = mapNum;
						iter->second.badShuntConfirm = GetBadShuntConfirmInfo(strPoorSperationConfirm);
					}
					else if (iter->second.badShuntType == mapNum && iter->second.badShuntConfirm.Compare(GetBadShuntConfirmInfo(strPoorSperationConfirm)))
					{	// �ظ����ͷ�·������Ϣʱ,����������¼����ʾ
						continue;
					}
					strPoorType = JudgeBADType(mapNum);
					strQueryRes = strTmpTime + " �豸(" + (CString)strTmp + ")" +
						strStationContentTmp + strPoorType + iter->second.badShuntConfirm;

				}
				else
				{	// map��Ϊ�գ���û�и��豸,��map����Ӹ��豸����Ϣ
					strStationContentTmp = "������·����";
					BadShuntInfoTmp.badShuntType = mapNum;
					BadShuntInfoTmp.badShuntConfirm = GetBadShuntConfirmInfo(strPoorSperationConfirm);
					mapPoorSperation[(CString)strTmp] = BadShuntInfoTmp;
					/*if(!JudgeBADType(mapNum, strPoorType))
					{
						MessageBox("ʶ���¼��������ʧ�ܣ�");
					}*/
					strPoorType = JudgeBADType(mapNum);
					strQueryRes = strTmpTime + " �豸(" + (CString)strTmp + ")" +
						strStationContentTmp + strPoorType + BadShuntInfoTmp.badShuntConfirm;
				}
			}
			if (!strStationContentTmp.IsEmpty())
			{
				pMyView->m_pListBox->AddString(strQueryRes);
			}
		}
	}
	if(nNum == 0)
	{
		CString strNoQueryRes = m_TimeStart.Format("%Y-%m-%d %H:%M:%S") + "--" +
			m_TimeStop.Format("%Y-%m-%d %H:%M:%S") + " �޷�·������Ϣ";
		pMyView->m_pListBox->AddString(strNoQueryRes);
	}
	if(pMyView->m_pListBox->GetCount() > 1 && 
		pMyView->m_pListBox->SetTopIndex(pMyView->m_pListBox->GetCount() -1))
	{
		MessageBox("Set scroll bar error!");
	}
	//fseek(pMyView->pfileIPDBG, 0, SEEK_SET);
	//pMyView->pfileIPDBG->Close();
	pFileIpdbg->SeekToBegin();
	pMain = NULL;
	pMyView = NULL;
	delete pMyView;
	delete pMain;
}
CString CPoorSperation::JudgeBADType(int num)
{
	CString str;
	if (num <= 0)
	{
		str.Empty();
	}
	if(num >= 8)
	{
		num -= 8;
		str += "(��λ)";
	}
	if(num >= 4)
	{
		num -= 4;
		str += "(��λ)";
	}
	if(num >= 2)
	{
		num -= 2;
		str += "(��ǰ)";
	}
	if(num >= 1)
	{
		num -= 1;
		str += "(����)";
	}
	return str;
}

CString CPoorSperation::GetBadShuntConfirmInfo(CString strBANDShuntconfirmInfo)
{
	if (strBANDShuntconfirmInfo.IsEmpty())
	{
		//return false;
	}
	CString str;
	if(strtol(strBANDShuntconfirmInfo, NULL, 16) == 0)
	{
		str = "(δȷ��)";
	}
	else if(strtol(strBANDShuntconfirmInfo, NULL, 16) == 1)
	{
		str = "(��ȷ��)";
	}
	return str;
}
