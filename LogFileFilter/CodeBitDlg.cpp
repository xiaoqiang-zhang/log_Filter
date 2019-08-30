// CodeBitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LogFileFilter.h"
#include "CodeBitDlg.h"
#include "LogHandle.h"
#include "string.h"
#include <fstream>
#include "MainFrm.h"
#include "LogFileFilterDoc.h"
#include "LogFileFilterView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCodeBitDlg dialog

CCodeBitDlg::CCodeBitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCodeBitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCodeBitDlg)

	//}}AFX_DATA_INIT
	AfxInitRichEdit();
	m_Flag_SelectAllOrDeleteSelected_ExpressCodeBits = -1;
	m_Flag_SelectAllOrDeleteSelected_ControlCodeBits = -1;
	//pFileIpdbg = new CFile;
	//pFileCadcom = new CStdioFile;
}

CCodeBitDlg::~CCodeBitDlg()
{
	/*if (pFileIpdbg != NULL)
	{
		pFileIpdbg->Close();
		delete pFileIpdbg;
		pFileIpdbg = NULL;
	}
	if (pFileCadcom != NULL)
	{
		pFileCadcom = NULL;
		delete pFileCadcom;
		pFileCadcom = NULL;
	}*/
}

void CCodeBitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCodeBitDlg)
	DDX_Control(pDX, IDC_EXPCODESELECTLIST, m_ExpressCodeBitsSelectedList);
	DDX_Control(pDX, IDC_EXPCODELIST, m_ExpressCodeBitsList);
	DDX_Control(pDX, IDC_CTRCODESELECTLIST, m_ControlCodeBitsSelectedList);
	DDX_Control(pDX, IDC_CTRCODELIST, m_ControlCodeBitsList);
	DDX_Control(pDX, IDC_QUERYTIMESTOP, m_EndQueryTime_CDateTimeCtrl);
	DDX_Control(pDX, IDC_QUERYTIMESTART, m_StartQueryTime_CDateTimeCtrl);
	DDX_Control(pDX, IDC_LOGTIMESPAN, m_LogTimeWidth);
	DDX_Radio(pDX, IDC_CHOOSECTRCODE, m_Flag_SelectAllOrDeleteSelected_ExpressCodeBits);
	DDX_Radio(pDX, IDC_CHOOSEEXPCODE, m_Flag_SelectAllOrDeleteSelected_ControlCodeBits);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCodeBitDlg, CDialog)
	//{{AFX_MSG_MAP(CCodeBitDlg)
	ON_BN_CLICKED(IDC_CHOOSEEXPCODE, OnSelectAllExpressCodeBits)
	ON_BN_CLICKED(IDC_CHOOSEEXPSOME, OnDeleteSelectedExpressCodeBits)
	ON_BN_CLICKED(IDC_CHOOSECTRCODE, OnSelectAllControlCodeBits)
	ON_BN_CLICKED(IDC_CHOOSECTRSOME, OnDeleteSelectedControlCodeBits)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CTRDECREASE, &CCodeBitDlg::OnClickedDecreaseControlCodeBits)
	ON_BN_CLICKED(IDC_CTRINCREASE, &CCodeBitDlg::OnClickedIncreaseControlCodeBits)
	ON_BN_CLICKED(IDC_EXPDECREASE, &CCodeBitDlg::OnClickedDecreaseExpressCodeBits)
	ON_BN_CLICKED(IDC_EXPINCREASE, &CCodeBitDlg::OnClickedIncreaseExpressCodeBits)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_QUERYTIMESTART, &CCodeBitDlg::OnDatetimechangeQuerytimestart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_QUERYTIMESTOP, &CCodeBitDlg::OnDatetimechangeQuerytimestop)
	ON_BN_CLICKED(IDC_CHECK, &CCodeBitDlg::OnClickedCheck)
	ON_BN_CLICKED(IDC_CANCEL, &CCodeBitDlg::OnClickedCancel)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCodeBitDlg message handlers


BOOL CCodeBitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pMyView = (CLogFileFilterView *)pMain->GetActiveView();
	if(pMyView->strIpdbgFilePath.IsEmpty())
	{
		//MessageBox("Please load IPDBG file");
		return FALSE;
	}
	LogFile ipdbgLogFile(pMyView->strIpdbgFilePath);
	unsigned long ulBeginTime = 0, ulEndTime = 0;
	std::string strBeginTime = ipdbgLogFile.GetLogBeaginTime(ulBeginTime);
	std::string strEndTime = ipdbgLogFile.GetLogEndTime(ulEndTime);
	std::string strTimeWidth;
	strTimeWidth.append(strBeginTime);	strTimeWidth.append("--");	strTimeWidth.append(strEndTime);
	m_LogTimeWidth.SetWindowText(strTimeWidth.c_str());
	m_StartQueryTime_CDateTimeCtrl.SetFormat("yyyy-MM-dd HH:mm:ss");	
	m_EndQueryTime_CDateTimeCtrl.SetFormat("yyyy-MM-dd HH:mm:ss");
	CTime t_Start(ulBeginTime), t_Stop(ulEndTime);
	m_StartQueryTime_CDateTimeCtrl.SetTime(&t_Start);
	m_EndQueryTime_CDateTimeCtrl.SetTime(&t_Stop);

	int resDisplayBits = 0;
	if(pMyView->strCadcomFilePath.IsEmpty())
	{
		//pFileIpdbg->SeekToBegin();
		int resDisplayBits = DisplayWithoutCADCOM(*ipdbgLogFile);
		if (resDisplayBits < 0)
		{
			MessageBox("An error occurred while displaying the code bit!");
		}
		m_Flag_LoadCadcomFile = 0;
	}
	else
	{
		//pFileCadcom->Open(pMyView->strCadcomFilePath, CFile::modeRead);
		/*std::string line;
		ifstream cadFile ();
		if (myfile.is_open())
		{
			while ( getline (myfile,line) )
			{

			}
			myfile.close();
		}*/
		ifstream cadFile(pMyView->strCadcomFilePath);
		if (cadFile.is_open())
		{
			DisplayWithCADCOM(*pFileCadcom);
		}
		myfile.close();
		//int resDisplayBits = DisplayWithCADCOM(*pFileCadcom);
		m_Flag_LoadCadcomFile = 1;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCodeBitDlg::OnSelectAllExpressCodeBits()
{
	// TODO: Add your control notification handler code here
	m_Flag_SelectAllOrDeleteSelected_ExpressCodeBits = 0;
	CString str;
	int count = m_ExpressCodeBitsList.GetCount();
	for(int i = 0; i < count; i++)
	{
		m_ExpressCodeBitsList.GetText(i,str);
		m_ExpressCodeBitsSelectedList.AddString(str);
	}
}

void CCodeBitDlg::OnDeleteSelectedExpressCodeBits()
{
	// TODO: Add your control notification handler code here
	m_Flag_SelectAllOrDeleteSelected_ExpressCodeBits = 1;
	m_ExpressCodeBitsSelectedList.ResetContent();
}

void CCodeBitDlg::OnSelectAllControlCodeBits()
{
	// TODO: Add your control notification handler code here
	m_Flag_SelectAllOrDeleteSelected_ControlCodeBits = 0;
	CString str;
	int count = m_ControlCodeBitsList.GetCount();
	for(int i = 0; i < count; i++)
	{
		m_ControlCodeBitsList.GetText(i,str);
		m_ControlCodeBitsSelectedList.AddString(str);
	}
}

void CCodeBitDlg::OnDeleteSelectedControlCodeBits()
{
	// TODO: Add your control notification handler code here
	m_Flag_SelectAllOrDeleteSelected_ControlCodeBits = 1;
	m_ControlCodeBitsSelectedList.ResetContent();
}

int CCodeBitDlg::DisplayWithCADCOM(CStdioFile &rFile)
{
	if (rFile == NULL)
	{
		return -1;
	}
	CString strFindKey;
	bool m_flagMMI = false;
	bool m_flagSYS = false;
	CString CodeBitsLength;
	int numOfCodeBits = 0;
	while(rFile.ReadString(strFindKey))
	{
		if (!m_flagMMI) {
			if(strFindKey.Find("DESTINATION = MMI")>=0)
			{
				m_flagMMI = true;
				continue;
			}
		}
		if (!m_flagSYS) {
			if(strFindKey.Find("SOURCE = MMI")>= 0)
			{
				m_flagSYS = true;
				continue;
			}
		}

		if(strFindKey.Find("MESSAGE = LENGTH") >= 0)
		{
			int nFirst = strlen("MESSAGE = LENGTH(");
			CodeBitsLength = strFindKey.Mid(nFirst, strlen(strFindKey) - nFirst -1);
			numOfCodeBits = _ttoi(CodeBitsLength);
		}
		if(m_flagMMI != 0 && m_flagSYS == 0 && strFindKey.Find(" 1 = ") >= 0)
		{
			for(int i = 0; i < numOfCodeBits; i++)
			{
				m_ExpressCodeBitsList.InsertString(i,strFindKey);
				rFile.ReadString(strFindKey);
			}
			continue;
		}
		if(m_flagSYS != 0 && strFindKey.Find(" 1 =") >= 0)
		{
			for(int j = 0; j < numOfCodeBits; j++)
			{
				m_ControlCodeBitsList.InsertString(j,strFindKey);
				rFile.ReadString(strFindKey);
			}
		}
	}
	return 0;
}

void CCodeBitDlg::OnClickedDecreaseControlCodeBits()
{
	// TODO: �ڴ����ӿؼ�֪ͨ������������
	int nSel = m_ControlCodeBitsSelectedList.GetCurSel();
	m_ControlCodeBitsSelectedList.DeleteString(nSel);
}

void CCodeBitDlg::OnClickedIncreaseControlCodeBits()
{
	// TODO: �ڴ����ӿؼ�֪ͨ������������
	int nSel = m_ControlCodeBitsList.GetCurSel();
	CString strIncreaseControlCodeBits;
	m_ControlCodeBitsList.GetText(nSel,strIncreaseControlCodeBits);
	m_ControlCodeBitsSelectedList.AddString(strIncreaseControlCodeBits);
}

void CCodeBitDlg::OnClickedDecreaseExpressCodeBits()
{
	// TODO: �ڴ����ӿؼ�֪ͨ������������
	int nSel = m_ExpressCodeBitsSelectedList.GetCurSel();
	m_ExpressCodeBitsSelectedList.DeleteString(nSel);
}

void CCodeBitDlg::OnClickedIncreaseExpressCodeBits()
{
	// TODO: �ڴ����ӿؼ�֪ͨ������������
	int nSel = m_ExpressCodeBitsList.GetCurSel();
	CString strIncreaseExpressCodeBits;
	m_ExpressCodeBitsList.GetText(nSel,strIncreaseExpressCodeBits);
	m_ExpressCodeBitsSelectedList.AddString(strIncreaseExpressCodeBits);
}

void CCodeBitDlg::OnDatetimechangeQuerytimestart(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: �ڴ����ӿؼ�֪ͨ������������
	*pResult = 0;
}

void CCodeBitDlg::OnDatetimechangeQuerytimestop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: �ڴ����ӿؼ�֪ͨ������������
	*pResult = 0;
}

void CCodeBitDlg::OnClickedCheck()
{
	CDialog::OnOK();
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pMyView = (CLogFileFilterView *)pMain->GetActiveView();
	int numControlSelected = m_ControlCodeBitsSelectedList.GetCount();
	int numExpressSelected = m_ExpressCodeBitsSelectedList.GetCount();
	if(numControlSelected == 0 && numExpressSelected == 0)
	{
		MessageBox("The query content is empty!");
	}

	m_StartQueryTime_CDateTimeCtrl.GetTime(m_StartQueryTime_CTime);	// ��CDatetimeCtrlת��ΪCTime�࣬���ں���ɸѡ��־��¼
	m_EndQueryTime_CDateTimeCtrl.GetTime(m_EndQueryTime_CTime);		// ��CDatetimeCtrlת��ΪCTime�࣬���ں���ɸѡ��־��¼
	if(m_StartQueryTime_CTime > m_EndQueryTime_CTime)
	{
		MessageBox("Start time cannot be greater than end time.");
		return;
	}

	GetQueryCodeBit(numControlSelected, numExpressSelected);
	pMyView->m_pListBox->AddString("��ѯ��λ��Ϣ��:");
	GetCodeBitMsg(*pFileIpdbg, numControlSelected, numExpressSelected);


	if(pMyView->m_pListBox->GetCount() > 1 &&
		pMyView->m_pListBox->SetTopIndex(pMyView->m_pListBox->GetCount() -1))
	{
		MessageBox("Set scroll bar error!");
	}
	pFileIpdbg->SeekToBegin();	// ��ԭ�ļ�ָ��λ��
	pMain = NULL;
	pMyView = NULL;
	delete pMyView;
	delete pMain;
}

void CCodeBitDlg::OnClickedCancel()
{
	// TODO: �ڴ����ӿؼ�֪ͨ������������
	CDialog::OnCancel();
}

void CCodeBitDlg::GetQueryCodeBit(const int numControl, const int numExpress)
{	// ��Ҫ��ѯ����λ������map�������Ƿ�������CADCOM�ļ�
	CString strControlCodeBitsName, strExpressCodeBitsName;
	int j = 0, k = 0;
	struct CODEBITINFO CodeBitCtr, CodeBitExp;
	//memset(&CodeBitCtr, 0, sizeof(CODEBITINFO));
	//memset(&CodeBitExp, 0, sizeof(CODEBITINFO));
	if(m_Flag_LoadCadcomFile == 1)
	{
		for(j = 0; j < numControl; j++)
		{
			m_ControlCodeBitsSelectedList.GetText(j, strControlCodeBitsName);
			strControlCodeBitsName.Replace(" ", "");
			CodeBitCtr.CodeBitName = strControlCodeBitsName.Mid(strControlCodeBitsName.Find("=")+1);
			CodeBitCtr.CodeBitStatus = 0;	// ��λ״̬��ʼĬ��Ϊ0
			m_mapRecordControlCodeBitsInfo[_ttoi(strControlCodeBitsName.Left(strControlCodeBitsName.Find("=")+1))] = CodeBitCtr;
		}
		for(k = 0; k < numExpress; k++)
		{
			m_ExpressCodeBitsSelectedList.GetText(k, strExpressCodeBitsName);
			strExpressCodeBitsName.Replace(" ", "");
			CodeBitExp.CodeBitName = strExpressCodeBitsName.Mid(strExpressCodeBitsName.Find("=")+1);
			CodeBitExp.CodeBitStatus = 0;	// ��λ״̬��ʼĬ��Ϊ0
			m_mapRecordExpressCodeBitsInfo[_ttoi(strExpressCodeBitsName.Left(strExpressCodeBitsName.Find("=")+1))] = CodeBitExp;
		}
	}
	else
	{
		for(j = 0; j < numControl; j++)
		{
			m_ControlCodeBitsSelectedList.GetText(j, strControlCodeBitsName);
			CodeBitCtr.CodeBitName = "";
			CodeBitCtr.CodeBitStatus = 0;	// ��λ״̬��ʼĬ��Ϊ0
			m_mapRecordControlCodeBitsInfo[_ttoi(strControlCodeBitsName)] = CodeBitCtr;
		}
		for(k = 0; k < numExpress; k++)
		{
			m_ExpressCodeBitsSelectedList.GetText(k, strExpressCodeBitsName);
			CodeBitExp.CodeBitName = "";
			CodeBitExp.CodeBitStatus = 0;	// ��λ״̬��ʼĬ��Ϊ0
			m_mapRecordExpressCodeBitsInfo[_ttoi(strExpressCodeBitsName)] = CodeBitExp;
		}
	}
}

void CCodeBitDlg::GetCodeBitMsg(CFile& rFile, const int numControl, const int numExpress)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pMyView = (CLogFileFilterView *)pMain->GetActiveView();
	REPLAY_DATA dataReplay;
	map<int,CODEBITINFO> mapExpressCodeBitsTemp, mapControlCodeBitsTemp;
	map<int,CODEBITINFO>::iterator iterExpressTemp, iterControlTemp;	// �����ж�ÿ����־����λ�Ƿ��б仯
	int NumOfReadExpressCodeBits = 0, NumOfReadControlCodeBits = 0;	// ��¼��ȡ���Ʊ�ʾ��λ�Ĵ���
	int NumOfRowsRead = 0;	// ��¼������һ��
	int resGetBitValue = 0;
	int numFixedLine = pMyView->m_pListBox->GetCount();
	CString strJudge, strFindRestartRecord;
	while(rFile.GetPosition() < rFile.GetLength())
	{
		int readSize = CReadLogFile::ReadReplayDataFromLog(rFile, dataReplay);
		if (readSize <= 0)
		{
			MessageBox("Read log record error! Please Confirm that the record is complete");
			return;
		}
		DisplayFileReadCountInOneLine(numFixedLine, NumOfRowsRead++);
		CTime logTime(dataReplay.m_time);
		strJudge = (CString)dataReplay.m_data;
		CString strRecordTime = CLogFileOperation::ConvertFromEpochToHumanReadableDate(dataReplay.m_time);
		if(logTime < m_StartQueryTime_CTime)
		{
			continue;
		}
		if (logTime >m_EndQueryTime_CTime)
		{
			break;
		}
		strFindRestartRecord = CLogFileOperation::IsFindRestartRecord(&strJudge, strRecordTime,
			&NumOfReadExpressCodeBits, &NumOfReadControlCodeBits);	// ��ѯ��������¼ʱ���ö�ȡ����ʾ������λ�Ĵ���
		if(!strFindRestartRecord.IsEmpty())
		{
			pMyView->m_pListBox->AddString(strFindRestartRecord);
		}
		if (strJudge.Find("[RPYBLK]") >= 0)
		{
			if (numExpress > 0)
			{
				NumOfReadExpressCodeBits++;
				resGetBitValue = GetBitValue(dataReplay.m_data, m_mapRecordExpressCodeBitsInfo, m_iterExpressCodeBits, &mapExpressCodeBitsTemp,
					iterExpressTemp, NumOfReadExpressCodeBits, strRecordTime);

				if (resGetBitValue < 0)
				{
					MessageBox("The code bits is empty, please confirm this record is complete!");
					return;
				}
			   }
		}
		else if (strJudge.Find("[RPYCTL]")>= 0)
		{
			if (numControl > 0)
			{
				NumOfReadControlCodeBits++;
				resGetBitValue = GetBitValue(dataReplay.m_data, m_mapRecordControlCodeBitsInfo, m_iterControlCodeBits, &mapControlCodeBitsTemp,
					iterControlTemp, NumOfReadControlCodeBits, strRecordTime);
				if (resGetBitValue < 0)
				{
					MessageBox("The code bits is empty, please confirm this record is complete!");
					return;
				}
			}
		}
	}


	rFile.SeekToBegin();
	pMain = NULL;
	pMyView = NULL;
	delete pMyView;
	delete pMain;
}

int CCodeBitDlg::DisplayWithoutCADCOM(CFile &rFileNonCadcom)
{	// 以数字形式表示码位
	if (rFileNonCadcom == NULL)
	{
		return -1;
	}
	REPLAY_DATA dataReplay;
	CString strJudge, strExp, strCtr,
		strExpressCodeBits, strControlCodeBits;
	int cnt;
	int m_flagExp = 0;
	int m_flagCtr = 0;
	while(rFileNonCadcom.GetLength() >= sizeof(dataReplay))
	{
		CReadLogFile::ReadReplayDataFromLog(rFileNonCadcom, dataReplay);
		strJudge = (CString)dataReplay.m_data;
		int m_FirstRecordOfExpressCodeBits = strJudge.Find("[RPYBLK]");
		int m_FirstRecordOfControlCodeBits = strJudge.Find("[RPYCTL]");
		if(m_FirstRecordOfExpressCodeBits >= 0 && m_flagExp == 0)
		{	// ֻ��ȡһ��
			strExpressCodeBits = strJudge.Mid(m_FirstRecordOfExpressCodeBits + strlen("[RPYBLK]") + 12,
				dataReplay.m_len - m_FirstRecordOfExpressCodeBits - strlen("[RPYBLK]") -12);//12Ϊվ��+VRDƫ��+VRD�������ֽ�
			cnt = ((strlen(strExpressCodeBits) - 1) -1) * 4;
			for(int i = 0; i < cnt; i++)
			{
				strExp.Format("%d", i + 1);
				m_ExpressCodeBitsList.InsertString(i, strExp);
			}
			m_flagExp = 1;
		}
		if(m_FirstRecordOfControlCodeBits >= 0 && m_flagCtr == 0)
		{// ֻ��ȡһ��
			strControlCodeBits = strJudge.Mid(m_FirstRecordOfControlCodeBits + strlen("[RPYCTL]") + 12,
				dataReplay.m_len - m_FirstRecordOfControlCodeBits - strlen("[RPYCTL]") -12);
			cnt = ((strlen(strControlCodeBits) - 1) - 1) * 4;
			for(int j = 0; j < cnt; j++)
			{
				strCtr.Format("%d", j + 1);
				m_ControlCodeBitsList.InsertString(j, strCtr);
			}
			m_flagCtr = 1;
		}
		if (m_flagCtr == 1 && m_flagExp == 1)
		{
			break;
		}
	}
	return 0;
}

void CCodeBitDlg::DisplayFileReadCountInOneLine(int numFixedLine, int count)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pView = (CLogFileFilterView *)pMain->GetActiveView();
	CString strFileCount;
	strFileCount.Format("��ѯ��λ��Ϣ��:%d", count);
	pView->m_pListBox->DeleteString(numFixedLine-1);
	pView->m_pListBox->InsertString(numFixedLine-1, strFileCount);
	pMain = NULL;
	pView = NULL;
	delete pView;
	delete pMain;
}

// 16�����ַ�ת����������
int CCodeBitDlg::HexStrToInt(char strdata, int bufSize, char *BinaryArray)
{
	if (strdata == NULL || bufSize < 0 || BinaryArray == NULL)
	{
		return -1;
	}
	if (strdata == 'F'){strcpy_s(BinaryArray, bufSize, "1111");}
	else if (strdata == 'E'){strcpy_s(BinaryArray, bufSize, "1110");}
	else if (strdata == 'D'){strcpy_s(BinaryArray, bufSize, "1101");}
	else if (strdata == 'C'){strcpy_s(BinaryArray, bufSize, "1100");}
	else if (strdata == 'B'){strcpy_s(BinaryArray, bufSize, "1011");}
	else if (strdata == 'A'){strcpy_s(BinaryArray, bufSize, "1010");}
	else if (strdata == '9'){strcpy_s(BinaryArray, bufSize, "1001");}
	else if (strdata == '8'){strcpy_s(BinaryArray, bufSize, "1000");}
	else if (strdata == '7'){strcpy_s(BinaryArray, bufSize, "0111");}
	else if (strdata == '6'){strcpy_s(BinaryArray, bufSize, "0110");}
	else if (strdata == '5'){strcpy_s(BinaryArray, bufSize, "0101");}
	else if (strdata == '4'){strcpy_s(BinaryArray, bufSize, "0100");}
	else if (strdata == '3'){strcpy_s(BinaryArray, bufSize, "0011");}
	else if (strdata == '2'){strcpy_s(BinaryArray, bufSize, "0010");}
	else if (strdata == '1'){strcpy_s(BinaryArray, bufSize, "0001");}
	else{strcpy_s(BinaryArray, bufSize, "0000");}
	return 0;
}

int CCodeBitDlg::GetBitValue(char* Ndata, map<int, CODEBITINFO> Nmap, map<int, CODEBITINFO>::iterator Niterator,
	map<int, CODEBITINFO> *mapTmp, map<int, CODEBITINFO>::iterator iterTmp, int num, CString strTime)
{
	if (Ndata == NULL)
	{
		return -1;
	}
	//count++;	// �ж��Ƿ���һ�β�ѯ(1)��û�в�ѯ(0)
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CLogFileFilterView *pView = (CLogFileFilterView *)pMain->GetActiveView();
	unsigned int numInteger = 0, numRemainder = 0;
	char tempBuf[10] = {0};
	CString strTmp, strIsTimeRecord, strBitChange;	// �����ӶԱ�ʾ��λ��ʱ��¼���ж�
	strIsTimeRecord.Format("%s", Ndata);
	for(Niterator = Nmap.begin(); Niterator != Nmap.end(); Niterator++)
	{	// ������λ�ı���ȥ�ҵ���λ�м�¼��λ��
		unsigned long time1 = GetCurrentTime();
		numInteger = (Niterator->first -1)/8;
		numRemainder = (Niterator->first - 1)%8;
		if(numRemainder > 3)
		{
			//ת��Ϊ����������
			if (strIsTimeRecord.Find("[TIMER_RECORD]") >= 0)
			{
				HexStrToInt(Ndata[numInteger*2 + 12 + strlen("[TIMER_RECORD][RPYBLK]")], sizeof(tempBuf), tempBuf);
				Niterator->second.CodeBitStatus = tempBuf[3 - (numRemainder - 4)];
			}
			else
			{
 				HexStrToInt(Ndata[numInteger*2 + 12 + strlen("[RPYBLK]")], sizeof(tempBuf), tempBuf);
				Niterator->second.CodeBitStatus = tempBuf[3 - (numRemainder - 4)];
			}
		}
		else if(numRemainder <= 3)
		{
			//ת��Ϊ����������
			if (strIsTimeRecord.Find("[TIMER_RECORD]") >= 0)
			{
				HexStrToInt(Ndata[numInteger*2 + 1 + 12 + strlen("[TIMER_RECORD][RPYBLK]")], sizeof(tempBuf), tempBuf);
				Niterator->second.CodeBitStatus = tempBuf[3 - numRemainder];
			}
			else
			{
				HexStrToInt(Ndata[numInteger*2 + 1 + 12 + strlen("[RPYBLK]")], sizeof(tempBuf), tempBuf);
				Niterator->second.CodeBitStatus = tempBuf[3 - numRemainder];
			}
		}
		if (m_Flag_LoadCadcomFile != 1)
		{
			Niterator->second.CodeBitName.Format("%d", Niterator->first);
		}
		strTmp += Niterator->second.CodeBitName + '(' + Niterator->second.CodeBitStatus + ") ";
		unsigned long time2 = GetCurrentTime();
		if (time2 - time1 > 10)
		{
			TRACE("%d, %d\n", (time2 - time1), numRemainder);
		}
	}
	if (num == 1)
	{//��ѯ��һ����ʾ��λʱ��ֱ�Ӵ�ӡ����ѯ��������¼ʱ(mapΪ��)
		strTmp = strTime + " " + strTmp;
		pView->m_pListBox->AddString(strTmp);
	}
	else
	{
		for(Niterator = Nmap.begin(), iterTmp = mapTmp->begin();
			Niterator != Nmap.end(), iterTmp != mapTmp->end();
			Niterator++, iterTmp++)
		{//����map,ֻ��ʾ�仯����λ��û�仯����λ����ʾ
			if(Niterator->second.CodeBitStatus != iterTmp->second.CodeBitStatus)
			{
				strBitChange += Niterator->second.CodeBitName + '(' + Niterator->second.CodeBitStatus + ") ";
			}
		}
		if (!strBitChange.IsEmpty())
		{
			strBitChange = strTime + " " + strBitChange;
			pView->m_pListBox->AddString(strBitChange);
		}
	}
	pView->m_pListBox->UpdateWindow();
	*mapTmp = Nmap;
	return 0;
}
