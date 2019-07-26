// Counter.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LogFileFilter.h"
#include "Counter.h"
#include "stdafx.h"
#include "LogFile.h"
#include "LogHandle.h"
#include "MainFrm.h"
#include "LogFileFilterDoc.h"
#include "LogFileFilterView.h"

// CCounter �Ի���

IMPLEMENT_DYNAMIC(CCounter, CDialog)

CCounter::CCounter(CWnd* pParent /*=NULL*/)
	: CDialog(CCounter::IDD, pParent)
{
	pFileIpdbg = new CFile;
}

CCounter::~CCounter()
{
	delete pFileIpdbg;
}

void CCounter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGTIMESPANCOUNTER, m_LogTimeSpanCounter);
	DDX_Control(pDX, IDC_QUERYTIMESTARTCOUNTER, m_CounterQueryTimeStart);
	DDX_Control(pDX, IDC_QUERYTIMESTOPCOUNTER, m_CounterQueryTimeStop);
}

BEGIN_MESSAGE_MAP(CCounter, CDialog)
	ON_BN_CLICKED(IDC_CHECKCOUNTER, &CCounter::OnBnClickedCheckcounter)
	ON_BN_CLICKED(IDC_CANCELCOUNTER, &CCounter::OnBnClickedCancelcounter)
END_MESSAGE_MAP()


// CCounter ��Ϣ�������



BOOL CCounter::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pView = (CLogFileFilterView *)pMain->GetActiveView();
	if(pView->strIpdbgFilePath.IsEmpty())
	{
		MessageBox("open file failed");
		return FALSE;
	}
	pFileIpdbg->Open(pView->strIpdbgFilePath, CFile::modeRead | CFile::typeBinary);
	int resSetTime = CLogFileOperation::SetLogTimeWidthAndDefaultQueryTime(*pFileIpdbg, m_LogTimeSpanCounter, m_CounterQueryTimeStart, m_CounterQueryTimeStop);
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

void CCounter::OnBnClickedCheckcounter()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialog::OnOK();
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pView = (CLogFileFilterView *)pMain->GetActiveView();
	
	CLogFileOperation::GetQueryTimeWin(m_CounterQueryTimeStart, m_CounterQueryTimeStop, m_TimeStartCounter, m_TimeStopCounter);
	if(m_TimeStartCounter > m_TimeStopCounter)
	{
		MessageBox("��ʼʱ����ڽ���ʱ�䣬����������");
		return;
	}
	pView->m_pListBox->AddString("������������Ϣ:");
	//���ݲ�ѯʱ���ѯ���
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

void CCounter::OnBnClickedCancelcounter()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialog::OnCancel(); 
}

void CCounter::GetQueryResult(void)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pMyView = (CLogFileFilterView *)pMain->GetActiveView();
	REPLAY_DATA dataReplay;
	COUNTERINFO counterDateAndNum;
	char strType[4] = {0};
	char strTypeMap[4] = {0};
	int mapNum = 0;
	int nNum = 0;
	int NumOfRowsRead = 0;
	int NumFixedLine = pMyView->m_pListBox->GetCount();
	while(pFileIpdbg->GetPosition() < pFileIpdbg->GetLength())
	{
		int readSize = CReadLogFile::ReadReplayDataFromLog(*pFileIpdbg, dataReplay);
		if (readSize <= 0)
		{
			MessageBox("Read log record error! Please Confirm that the record is complete");
			return;
		}
		DisplayFileReadCountInOneLine(NumFixedLine, NumOfRowsRead++);
		CTime logTime(dataReplay.m_time);
		strJudge = (CString)dataReplay.m_data;
		char strTmp[100], bufCounterDate[100];
		CString strPoorType, strTmpCount, strCounterDate;
		int nFirstWin = strJudge.Find("[RPYCNT]");
		CString strStationContentTmp;
		static unsigned long lastRecordTime = 0;
		static int acceptableErrorNum = 0;
		static CString strLastRecord;
		CString strRecordTime = CLogFileOperation::ConvertFromEpochToHumanReadableDate(dataReplay.m_time);
		/************************�����Զ�����������Դ���**********************/
		if (dataReplay.m_time - lastRecordTime >= 5)
		{	// ��һ����¼��lastRecordTime��ֵ��������ӡ
			//lastRecordTime = dataReplay.m_time;
			pMyView->m_pListBox->AddString(strLastRecord);
			pMyView->m_pListBox->AddString(strJudge);
		}
		lastRecordTime = dataReplay.m_time;
		strLastRecord = strJudge;
		/**********************************************/
		if(logTime < m_TimeStartCounter)
		{
			pMyView->m_pListBox->UpdateWindow();
			continue;
		}
		if ((logTime > m_TimeStopCounter))
		{
			break;
		}
		if(strJudge.Find("[USER]: create queue,USER.queue_handle") >= 0)
		{
			if(!mapCounter.empty())
			{
				mapCounter.clear();
			}
			strCounterResult = strRecordTime + "ʱ���������";
			pMyView->m_pListBox->AddString(strCounterResult);
		}
		if (nFirstWin < 0)
		{
			pMyView->m_pListBox->UpdateWindow();
			continue;
		}
		strData = strJudge.Mid(nFirstWin + strlen("[RPYCNT]"), 
			dataReplay.m_len - nFirstWin - strlen("[RPYCNT]"));
		strStationNum = strData.Left(4);
		strStationEquipmentType = strData.Mid(4, 4);
		strStationEquipmentID = strData.Mid(8, 4);
		strCounterState = strData.Mid(12, 4);
		strCounterNum = strData.Mid(16, 4);
		sprintf_s(bufCounterDate, sizeof(bufCounterDate), "%d-%d-%d", logTime.GetYear(), logTime.GetMonth(), logTime.GetDay());
		strCounterDate.Format("%s", bufCounterDate);
		sprintf_s(strTmp, sizeof(strTmp), "%d-%d-%d", strtol(strStationNum, NULL, 16), 
			strtol(strStationEquipmentType, NULL, 16), strtol(strStationEquipmentID, NULL, 16));
		// ��ѯmapCounter�Ƿ���ڵ�ǰ�豸�ļ�¼���еĻ����ش�Ÿ��豸��¼�ĵ�ַ��û�еĻ�����end()
		iter = mapCounter.find((CString)strTmp);	
		mapNum = strtol(strCounterNum, NULL, 16);
		if(mapCounter.empty())
		{
			//strStationContentTmp = "";
			counterDateAndNum.counterYearMonthDay = strCounterDate;
			counterDateAndNum.counterNum = mapNum;
			mapCounter[(CString)strTmp] = counterDateAndNum;
		}
		else
		{
			if(iter != mapCounter.end())
			{	// ��ǰһ�μ�����Ϣ���Ա�
				if (!iter->second.counterYearMonthDay.Compare(strCounterDate))
				{
					if(mapNum != iter->second.counterNum)
					{
						if (mapNum > iter->second.counterNum)
						{
							strTmpCount.Format("%d", (mapNum - iter->second.counterNum));
							strTmpCount = "(+" + strTmpCount + ")";
						}
						else
						{
							strTmpCount.Format("%d", (iter->second.counterNum - mapNum));
							strTmpCount = "(-" + strTmpCount + ")";
						}
						iter->second.counterNum = mapNum;
					}
					else if(mapNum == iter->second.counterNum)
					{
						continue;
					}
				}
				else
				{
					iter->second.counterYearMonthDay = strCounterDate;
					iter->second.counterNum = mapNum;
				}	
			}
			else
			{	// map��Ϊ�գ���û�и��豸,ֱ����map����Ӹü�¼
				counterDateAndNum.counterYearMonthDay = strCounterDate;
				counterDateAndNum.counterNum = mapNum;
				mapCounter[(CString)strTmp] = counterDateAndNum;
			}
		}
		strCounterNum.Format("%d", mapNum);
		strCounterResult = strRecordTime + " �豸(" + strTmp + ")״̬(" + strCounterState + 
			") ����ֵ" +  strCounterNum +  strTmpCount;
		pMyView->m_pListBox->AddString(strCounterResult);
		nNum++;
		pMyView->m_pListBox->UpdateWindow();
	}
	if(nNum == 0)
	{
		CString strNoQueryRes = m_TimeStartCounter.Format("%Y-%m-%d %H:%M:%S") + "--"+ 
			m_TimeStopCounter.Format("%Y-%m-%d %H:%M:%S") + " �޼�������Ϣ";
		pMyView->m_pListBox->AddString(strNoQueryRes);
	}
	if(pMyView->m_pListBox->GetCount() > 1 && 
		pMyView->m_pListBox->SetTopIndex(pMyView->m_pListBox->GetCount() -1))
	{
		MessageBox("Set scroll bar error!");
	}
	pFileIpdbg->SeekToBegin();
	pMain = NULL;
	pMyView = NULL;
	delete pMyView;
	delete pMain;
}

void CCounter::DisplayFileReadCountInOneLine(int numFixedLine, int count)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pView = (CLogFileFilterView *)pMain->GetActiveView();
	CString strFileCount;
	strFileCount.Format("��ѯ��������Ϣ��:%d", count); 
	pView->m_pListBox->DeleteString(numFixedLine-1);
	pView->m_pListBox->InsertString(numFixedLine-1, strFileCount);
	pMain = NULL;
	pView = NULL;
	delete pView;
	delete pMain;
}