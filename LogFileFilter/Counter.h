#pragma once
#include <map>
using namespace std;

// CCounter �Ի���
class LogFileFilterView;
struct COUNTERINFO
{
	CString counterYearMonthDay;	// ��¼��ʱ�䣨�����գ�
	int counterNum;
};

class CCounter : public CDialog
{
	DECLARE_DYNAMIC(CCounter)

public:
	CCounter(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCounter();

	CTime m_TimeStartCounter, m_TimeStopCounter;
	CString strStationNum, strStationEquipmentType, strStationEquipmentID, 
		strCounterState, strCounterNum;
	CString strJudge, strData;
	CString strCounterResult;
	map<CString,COUNTERINFO> mapCounter;
	map<CString, COUNTERINFO>::iterator iter;
	CFile* pFileIpdbg;

// �Ի�������
	enum { IDD = IDD_COUNTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_LogTimeSpanCounter;
	CDateTimeCtrl m_CounterQueryTimeStart;
	CDateTimeCtrl m_CounterQueryTimeStop;
	afx_msg void OnBnClickedCheckcounter();
	afx_msg void OnBnClickedCancelcounter();
	void GetQueryResult(void);
	void DisplayFileReadCountInOneLine(int numFixedLine, int count);
};
