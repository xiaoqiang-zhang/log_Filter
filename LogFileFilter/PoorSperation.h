#include <map>
using namespace std; 

#pragma once


// CPoorSperation �Ի���
class LogFileFilterView;
struct BADSHUNTINFO
{
	int badShuntType;
	CString badShuntConfirm;
};

class CPoorSperation : public CDialog
{
	DECLARE_DYNAMIC(CPoorSperation)

public:
	CTime m_TimeStart, m_TimeStop;
	CString strStationNum, strStationEquipmentType, strStationEquipmentID, 
		strPoorSperationType, strPoorSperationConfirm;
	CString strJudge, strData;
	map<CString,BADSHUNTINFO> mapPoorSperation;
	map<CString, BADSHUNTINFO>::iterator iter;
	int mapNum;

	CFile* pFileIpdbg;

	CPoorSperation(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPoorSperation();

// �Ի�������
	enum { IDD = IDD_POORSPERATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_LogTimeSpanPoor;
	CDateTimeCtrl m_PoorQueryTimeStart;
	CDateTimeCtrl m_PoorQueryTimeStop;
	afx_msg void OnClickedCheckpoor();
	afx_msg void OnClickedCancelpoor();
	void GetQueryResult(void);
	CString JudgeBADType(int num);
	CString GetBadShuntConfirmInfo(CString strBANDShuntconfirmInfo);
};
