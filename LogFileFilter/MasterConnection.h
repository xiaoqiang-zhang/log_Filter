#pragma once


// CMasterConnection �Ի���
class LogFileFilterView;

class CMasterConnection : public CDialog
{
	DECLARE_DYNAMIC(CMasterConnection)

public:
	CMasterConnection(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMasterConnection();

	CTime m_TimeStartMaster, m_TimeStopMaster;


// �Ի�������
	enum { IDD = IDD_MASTERCONNECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_LogTimeSpanMaster;
	CDateTimeCtrl m_MasterQueryTimeStart;
	CDateTimeCtrl m_MasterQueryTimeStop;
	CFile* pFileIpdbg;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckmaster();
	afx_msg void OnBnClickedCancelmaster();
	void GetQueryResult(void);
};
