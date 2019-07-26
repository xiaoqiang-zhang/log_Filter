#include <map>
using namespace std; 
#pragma once


// CTrainNumWin �Ի���
class LogFileFilterView;

class CTrainNumWin : public CDialog
{
	DECLARE_DYNAMIC(CTrainNumWin)

public:
	CTrainNumWin(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTrainNumWin();

	//CString startTime, stopTime;
	CTime m_TimeStartWin, m_TimeStopWin;
	CString strStationNum;
	CString strStationEquipmentType;
	CString strStationEquipmentID;
	CString strStationEquipmentStatus;
	CString strStationContent;
	CString strQueryResult;
	CString strJudge, strData;
	map<CString,CString> mapStationNumWin;
	map<CString, CString>::iterator iter;

	CFile* pFileIpdbg;
	

// �Ի�������
	enum { IDD = IDD_TRAINNUMWIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_LogTimeSpanWin;
	
	virtual BOOL OnInitDialog();
	CDateTimeCtrl m_WinQueryTimeStart;
	CDateTimeCtrl m_WinQueryTimeStop;
	afx_msg void OnClickedCheck();
	afx_msg void OnCancel();
	void GetQueryResult(void);
};
