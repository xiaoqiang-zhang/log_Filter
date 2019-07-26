// TrainNumWin.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "LogFileFilter.h"
#include "TrainNumWin.h"
#include "afxdialogex.h"
#include "LogHandle.h"
#include "MainFrm.h"
#include "LogFileFilterDoc.h"
#include "LogFileFilterView.h"


// CTrainNumWin �Ի���

IMPLEMENT_DYNAMIC(CTrainNumWin, CDialog)

CTrainNumWin::CTrainNumWin(CWnd* pParent /*=NULL*/)
	: CDialog(CTrainNumWin::IDD, pParent)
{
	pFileIpdbg = new CFile;
}

CTrainNumWin::~CTrainNumWin()
{
	delete pFileIpdbg;
}

void CTrainNumWin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGTIMESPANWIN, m_LogTimeSpanWin);
	DDX_Control(pDX, IDC_QUERYTIMESTARTWIN, m_WinQueryTimeStart);
	DDX_Control(pDX, IDC_QUERYTIMESTOPWIN, m_WinQueryTimeStop);
}


BEGIN_MESSAGE_MAP(CTrainNumWin, CDialog)
	//ON_COMMAND(ID_TRAINNUMWIN, &CTrainNumWin::OnTrainnumwin)
	ON_BN_CLICKED(IDC_CHECK, &CTrainNumWin::OnClickedCheck)
	ON_COMMAND(IDC_CANCEL, &CTrainNumWin::OnCancel)
END_MESSAGE_MAP()


// CTrainNumWin ��Ϣ�������


BOOL CTrainNumWin::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pView = (CLogFileFilterView *)pMain->GetActiveView();
	if(pView->strIpdbgFilePath.IsEmpty())
	{
		MessageBox("open file failed");
	}
	pFileIpdbg->Open(pView->strIpdbgFilePath, CFile::modeRead | CFile::typeBinary);
	int resSetTime = CLogFileOperation::SetLogTimeWidthAndDefaultQueryTime(*pFileIpdbg, m_LogTimeSpanWin, m_WinQueryTimeStart, m_WinQueryTimeStop);
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

void CTrainNumWin::OnClickedCheck()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialog::OnOK();
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pView = (CLogFileFilterView *)pMain->GetActiveView();
	
	CLogFileOperation::GetQueryTimeWin(m_WinQueryTimeStart, m_WinQueryTimeStop, m_TimeStartWin, m_TimeStopWin);
	if(m_TimeStartWin > m_TimeStopWin)
	{
		MessageBox("��ʼʱ����ڽ���ʱ�䣬����������");
		return;
	}
	pView->m_pListBox->AddString("�������δ���Ϣ:");
	GetQueryResult();
	if(pView->m_pListBox->GetCount() > 1 && 
		pView->m_pListBox->SetTopIndex(pView->m_pListBox->GetCount() -1))
	{
		MessageBox("Set scroll bar error!");
	}
	pMain = NULL;
	pView = NULL;
	delete pView;
	delete pMain;
}


void CTrainNumWin::OnCancel()
{
	// TODO: �ڴ���������������
	CDialog::OnCancel(); 
}


void CTrainNumWin::GetQueryResult(void)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pMyView = (CLogFileFilterView *)pMain->GetActiveView();
	//pMyView->pfileIPDBG->Open(pMyView->strIpdbgFilePath, CFile::modeRead | CFile::typeBinary);
	REPLAY_DATA dataReplay;
	int nNum = 0;
	//while(!feof(pView->pfileIPDBG))
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
		char bufTmpTime[100] = {0};
		int nFirstWin = strJudge.Find("[RPYTID]");
		CString strTmpTime, strStationContentTmp;
		CLogFileOperation::ChangeToTime(dataReplay.m_time, bufTmpTime, 100);
		strTmpTime.Format("%s", bufTmpTime);
		char strTmp[100];
		if(logTime < m_TimeStartWin)
		{	// ��־ʱ��С�ڲ�ѯʱ��ʱ������������¼ȥ��ѯ����ļ�¼
			continue;
		}
		if (logTime > m_TimeStopWin)
		{	// ��־ʱ����ڲ�ѯʱ��ʱ������whileѭ��
			break;
		}
		if(strJudge.Find("[USER]: create queue,USER.queue_handle") >= 0)
		{
			if(!mapStationNumWin.empty())
			{
				mapStationNumWin.clear();
			}
			strQueryResult = strTmpTime + "ʱ���������";
			pMyView->m_pListBox->AddString(strQueryResult);
		}
		if(nFirstWin >= 0)
		{
			strData = strJudge.Mid(nFirstWin + strlen("[RPYTID]"), 
				dataReplay.m_len - nFirstWin - strlen("[RPYTID]"));
			strStationNum = strData.Left(4);//4�ֽ�վ��
			strStationEquipmentType = strData.Mid(12, 4);//����4�ֽ�0��4�ֽ�Ԥ��,��ȡ4�ֽ�վ���豸����
			strStationEquipmentID = strData.Mid(16, 4);//4�ֽ�վ���豸ID
			strStationEquipmentStatus = strData.Mid(20, 8);//8�ֽ�վ���豸״̬
			strStationContent = strData.Mid(strData.Find('[')+1, strData.Find(']')-strData.Find('[')-1);//���δ�����
			CLogFileOperation::ChangeToTime(dataReplay.m_time, bufTmpTime, 100);
			strTmpTime.Format("%s", bufTmpTime);
			sprintf_s(strTmp, sizeof(strTmp), "%d-%d-%d", strtol(strStationNum, NULL, 16), 
				strtol(strStationEquipmentType, NULL, 16), strtol(strStationEquipmentID, NULL, 16));
			//mapStationNumWin�Ƿ��и��豸�ļ�¼
			iter = mapStationNumWin.find((CString)strTmp);
			int ret = strStationContent.Replace("\r\n", "");
			if(mapStationNumWin.empty())
			{
				strStationContentTmp = "�������δ�:";
				mapStationNumWin[(CString)strTmp] = strStationContent;
			}
			else
			{	// map��Ϊ��
				if(iter != mapStationNumWin.end())
				{	// mapStationNumWin���и��豸�ļ�¼����ʱͨ�����δ����������жϵ�ǰΪ�޸ĳ��δ����������δ�������ɾ�����δ�
					if ((iter->second.IsEmpty()) &&
 						!strStationContent.IsEmpty())//���δ����ݴ�[]==>[ͣ����������]
					{
						strStationContentTmp = "�������δ�:";
						mapStationNumWin[(CString)strTmp]  = strStationContent;
					}
					else if ((!iter->second.IsEmpty()) &&
						strStationContent.IsEmpty())//���δ����ݴ�[ͣ����������]==>[])
					{
						strStationContentTmp = "ɾ�����δ�";
						mapStationNumWin[(CString)strTmp] = strStationContent;
					}
					else if((!iter->second.IsEmpty()) &&
						(!strStationContent.IsEmpty()) &&
						(iter->second.Compare(strStationContent) != 0))
					{
						strStationContentTmp = "�޸ĳ��δ�:";
						mapStationNumWin[(CString)strTmp] = strStationContent;
					}
					else if (!iter->second.Compare(strStationContent))
					{	// ���豸����Ϣ�ظ���ӡʱ������������¼����ʾ
						continue;
					}
				}
				else
				{	// map��Ϊ�գ���û�и��豸,ֱ����map����Ӹü�¼
					strStationContentTmp = "�������δ�:";
					mapStationNumWin[(CString)strTmp] = strStationContent;
				}
			}
			strQueryResult = strTmpTime + " �豸(" + (CString)strTmp + ")" +
				strStationContentTmp + strStationContent;
			pMyView->m_pListBox->AddString(strQueryResult);
			nNum++;
		}
	}
	if(nNum == 0)
	{
		CString strNoQueryReult = m_TimeStartWin.Format("%Y-%m-%d %H:%M:%S") + "--" + 
			m_TimeStopWin.Format("%Y-%m-%d %H:%M:%S") + " �޳��δ���Ϣ";
		pMyView->m_pListBox->AddString(strNoQueryReult);
	}
	if(pMyView->m_pListBox->GetCount() > 1 && 
		pMyView->m_pListBox->SetTopIndex(pMyView->m_pListBox->GetCount() -1))
	{
		MessageBox("Set scroll bar error!");
	}
	pMain = NULL;
	pMyView = NULL;
	delete pMyView;
	delete pMain;
}
