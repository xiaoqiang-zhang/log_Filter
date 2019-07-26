#include "stdafx.h"
#include "loghandle.h"
#include "string.h"
//1�ֽڶ���
	
enum{MAX_CLIENTS_NUM=16, RECORD_FLAG=0xAA, RECORD_FLAG_Number=0x48};
enum{IntL_1 = 1, IntL_2 = 2, IntL_4 = 4};
	
//CReadLogFile
int CReadLogFile::GetIntegerLengthInLog(IntegerLength eIntL)
{
	int iIntSizeInLog = 0;
	if (eIntL == IntL_1)
	{
		iIntSizeInLog = 2;
	}
	else if (eIntL == IntL_2)
	{
		iIntSizeInLog = 4;
	}
	else if (eIntL == IntL_4)
	{
		iIntSizeInLog = 8;
	}
	else
	{

	}
	return iIntSizeInLog;
}
int CReadLogFile::ReadIntegerValueFromLog(CFile& rFile, void* pIntValue, IntegerLength eIntL)
{
	int iIntSizeInLog = GetIntegerLengthInLog(eIntL);   //�õ�2,4,8
	char ch[1] = {0};
	//int sizeRead = fread(ch, 1, 1, &rFile);
	int sizeRead = rFile.Read(ch, 1);
	if (ch[0] != '\n') 
	{
		//fseek(&rFile, -1, SEEK_CUR);
		rFile.Seek(-1, CFile::current);
	}

	char* pszBuf = new char[iIntSizeInLog + 1];     //����һ��ָ�루pszBuf[iIntSizeInLog+1]������ȡ�ļ�����
	memset(pszBuf, 0, iIntSizeInLog + 1);

	//sizeRead = fread(pszBuf, 1, iIntSizeInLog, &rFile);
	sizeRead = rFile.Read(pszBuf, iIntSizeInLog);
	unsigned long lValue = strtoul(pszBuf, NULL, 16);    //��ȡ��������ת��Ϊ16����
	if (eIntL == IntL_1)		          //��������ת����
	{
		*(unsigned char*)pIntValue = (unsigned char)lValue;
	}
	else if (eIntL == IntL_2)
	{
		*(unsigned short*)pIntValue = (unsigned short)lValue;
	}
	else if (eIntL == IntL_4)
	{
		*(unsigned long*)pIntValue = lValue;
	}
	else
	{

	}
	delete[] pszBuf;
	return sizeRead;
}
bool CReadLogFile::IsReplayFileFormatBinary(CFile& rFile)
{
	//����־�ļ��ĵ�һ���ֽ���Ϊ�ж�����
	unsigned char cRecordFlag = 0;
	//int sizeRead = fread(&cRecordFlag, 1, 1, &rFile);
	int sizeRead = rFile.Read(&cRecordFlag, 1);
	if (sizeRead > 0)
	{
		//fseek(&rFile, -1, SEEK_CUR);
		rFile.Seek(-1, CFile::current);
		return cRecordFlag == RECORD_FLAG;
	}
	return false;//Ĭ��Ϊ�¸�ʽ
}
int CReadLogFile::ReadReplayDataFromLog(CFile& rFile, REPLAY_DATA& dataReplay)
{
	int iRetLen = 0;
	int sizeRead = 0;
	if (IsReplayFileFormatBinary(rFile))
	{//�����ƻطŸ�ʽ�����ϵĻطŸ�ʽ
		//sizeRead = fread(&dataReplay, 1, sizeof(REPLAY_DATA), &rFile);
		memset(&dataReplay, 0, sizeof(dataReplay));
		//sizeRead = fread(&dataReplay.m_flag, 1, 1, &rFile);
		//sizeRead = fread(&dataReplay.m_time, 1, 4, &rFile);
		//sizeRead = fread(&dataReplay.m_len, 1, 2, &rFile);
		//sizeRead = fread(&dataReplay.m_data, 1, dataReplay.m_len, &rFile);
		//sizeRead = fread(&dataReplay.m_position, 1, 4, &rFile);
		//CString str = (CString)dataReplay.m_data;
		sizeRead = rFile.Read(&dataReplay.m_flag, 1);
		sizeRead = rFile.Read(&dataReplay.m_time, 4);
		sizeRead = rFile.Read(&dataReplay.m_len, 2);
		sizeRead = rFile.Read(&dataReplay.m_data, dataReplay.m_len);
		sizeRead = rFile.Read(&dataReplay.m_position, 4);
		if (sizeRead <= 0)
		{
			return 0;
		}
		if (dataReplay.m_len > 1000)
		{
			//DebugMsgOut(0,"[IP]:�ط����ݵ����ݶγ���:%d ����",dataReplay.m_len);
			return 0;
		}
		dataReplay.m_position = *(long*)&dataReplay.m_data[dataReplay.m_len];            
		dataReplay.m_data[dataReplay.m_len] = 0;
		iRetLen = sizeof(REPLAY_DATA) - (MAX_REPLAY_DATA_LENGTH-dataReplay.m_len);
	}
	else
	{//�µĻطŸ�ʽ
		memset(&dataReplay, 0, sizeof(dataReplay));
		sizeRead = ReadIntegerValueFromLog(rFile, &dataReplay.m_flag, IntL_1);
		if (sizeRead <= 0)
		{
			return 0;
		}
		iRetLen += sizeRead;
		sizeRead = ReadIntegerValueFromLog(rFile, &dataReplay.m_time, IntL_4);
		if (sizeRead <= 0)
		{
			return 0;
		}
		iRetLen += sizeRead;
		sizeRead = ReadIntegerValueFromLog(rFile, &dataReplay.m_len, IntL_2);
		if (sizeRead <= 0)
		{
			return 0;
		}
		if ((dataReplay.m_len <= 0) || (dataReplay.m_len > MAX_REPLAY_DATA_LENGTH))
		{
			return 0;
		}
		iRetLen += sizeRead;
		//sizeRead = fread(dataReplay.m_data, 1, dataReplay.m_len, &rFile);
		sizeRead = rFile.Read(&dataReplay.m_data, dataReplay.m_len);
		if (sizeRead <= 0)
		{
			return 0;
		}
		iRetLen += sizeRead;
		sizeRead = ReadIntegerValueFromLog(rFile, &dataReplay.m_position, IntL_4);
		if (sizeRead <= 0)
		{
			return 0;
		}
		iRetLen += sizeRead;
	}
	return iRetLen;
}

//CLogFileOperation
int CLogFileOperation::SetLogTimeWidthAndDefaultQueryTime(CFile &rFile, CEdit &m_Editcontrol, CDateTimeCtrl &timestart, CDateTimeCtrl &timestop)
{	// ��ʼʱ��Ĭ��Ϊ��־��һ����¼��ʱ�䣬����ʱ��Ϊ��־���һ����¼��ʱ��
	if (rFile == NULL)
	{
		return -1;
	}
	int NumOfWrap = 0;
	REPLAY_DATA dataReplay;
	CString strLogTimeWidth, strLogFirstRecordTime, strLogLastRecordTime;
	unsigned long ulLogFirstRecord = 0, ulLogLastRecord = 0;
	char chWrap[1] = {0};
	int n = sizeof(chWrap);
	bool flagNewRecord = false;
	if (!CReadLogFile::IsReplayFileFormatBinary(rFile))
	{
		flagNewRecord = true;
	}
	int readSize = CReadLogFile::ReadReplayDataFromLog(rFile, dataReplay);
	if (readSize <= 0)
	{
		return -2;
	}
	
	ulLogFirstRecord = dataReplay.m_time;
	ULONGLONG fileLength = rFile.SeekToEnd();
	rFile.SeekToBegin();
	rFile.Seek(fileLength - 1, CFile::current);
	while (1)
	{
		if (!flagNewRecord)
		{
			rFile.Read(chWrap, 1);
			if(chWrap[0] != '\n')
			{
				rFile.Seek(-2, CFile::current);
				continue;
			}
			NumOfWrap++;
			if (NumOfWrap == 2)
			{
				rFile.Seek(5, CFile::current);
				rFile.Read(&ulLogLastRecord, 4);
				break;
			}
			rFile.Seek(-2, CFile::current);
		}
		else
		{
			rFile.Read(chWrap, sizeof(chWrap));
			if(chWrap[0] != '\n')
			{
				rFile.Seek(-2, CFile::current);
				continue;
			}
			NumOfWrap++;
			if (NumOfWrap == 2)
			{
				rFile.Seek(10, CFile::current);
				char endTimeBuf[8] = {0};
				rFile.Read(endTimeBuf, sizeof(endTimeBuf));
				ulLogLastRecord = atoi(endTimeBuf);
				ulLogLastRecord = strtol(endTimeBuf, NULL, 16);
				break;
			}
			rFile.Seek(-2, CFile::current);
		}
	}
	strLogFirstRecordTime = ConvertFromEpochToHumanReadableDate(ulLogFirstRecord);	//TimeStamp convert to normal date
	strLogLastRecordTime = ConvertFromEpochToHumanReadableDate(ulLogLastRecord);	//TimeStamp convert to normal date
	strLogTimeWidth = strLogFirstRecordTime + "--" + strLogLastRecordTime;	
	m_Editcontrol.SetWindowText(strLogTimeWidth);

	timestart.SetFormat("yyyy-MM-dd HH:mm:ss");	
	timestop.SetFormat("yyyy-MM-dd HH:mm:ss");
	CTime t_Start(ulLogFirstRecord), t_Stop(ulLogLastRecord);
	timestart.SetTime(&t_Start);
	timestop.SetTime(&t_Stop);

	rFile.SeekToBegin();
	return 1;
}

void CLogFileOperation::ChangeToTime(unsigned long mTime, char *timeTmp, int num)
{
	time_t tick = mTime;
	struct tm tm;
	localtime_s(&tm, &tick);
	strftime(timeTmp, num, "%Y-%m-%d %H:%M:%S", &tm);
}

void CLogFileOperation::GetQueryTimeWin(CDateTimeCtrl &TimeStart, CDateTimeCtrl &TimeStop, CTime &startTime, CTime &stopTime)
{
	//��ȡ��ʾ��ʱ��
	TimeStart.GetTime(startTime);
	TimeStop.GetTime(stopTime);
}

void CLogFileOperation::SetFileEmpty(CFile *rFile)
{
	if (rFile != NULL)
	{
		rFile = NULL;
	}
}

BOOL CLogFileOperation::TransformLogFile(CString strFilePath, CFile* pFile, int *wrongLineNum)
{
	if (strFilePath.IsEmpty())
	{
		return FALSE;
	}
	CString logNameNew, strData, strChange;
	CString strFilter = "�ı��ļ�(*.LOG)| *.LOG||";
	logNameNew = strFilePath.Mid(0, (strlen(strFilePath)-strlen(".LOG")));
	logNameNew +=  "_NEW.LOG";
	CFileDialog filedlg(
		false,
		".LOG",
		_T(logNameNew),
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		strFilter,
		NULL);
	if(filedlg.DoModal() == IDOK)
	{
		//FILE *rFileIpdbgChange = fopen(filedlg.GetPathName(), "w+b");
		CFile* rFileIpdbgChange = new CFile;
		rFileIpdbgChange->Open(filedlg.GetPathName(), CFile::modeCreate|CFile::modeReadWrite);
		REPLAY_DATA dataReplay = {0};
		int n = 1;
		int readSize = CReadLogFile::ReadReplayDataFromLog(*pFile, dataReplay);
		if (readSize <= 0)
		{
			return FALSE;
		}
		while(pFile->GetPosition() < pFile->GetLength())
		{
			CTime logTime(dataReplay.m_time);
			strData = (CString)dataReplay.m_data;
			char bufTmpTime[100] = {0};
			CString strTmpTime;
			CLogFileOperation::ChangeToTime(dataReplay.m_time, bufTmpTime, 100);
			strTmpTime.Format("%s", bufTmpTime);
			strChange = strTmpTime + strData;
			//fwrite(strChange, strlen(strChange), 1, rFileIpdbgChange);
			rFileIpdbgChange->Write(strChange, strlen(strChange));
			readSize = CReadLogFile::ReadReplayDataFromLog(*pFile, dataReplay);
			n++;
			if (readSize <= 0)
			{
				return FALSE;
			}
		}
		pFile->SeekToBegin();
		return TRUE;
	}
	return FALSE;
}

CString CLogFileOperation::OpenLogFile(CString strFilter)
{	// ����Ҫ�ж�strFilter�Ƿ�Ϊ��,Ϊ��ֻ�ǲ��ܹ����ļ���Ӱ��ѡȡ�ļ�.
	CFileDialog filedlg(
		true,
		NULL,
		NULL,
		OFN_NOCHANGEDIR,
		strFilter,
		NULL);
	if(filedlg.DoModal() == IDOK)
	{
		//strFilePath = filedlg.GetPathName();
		return filedlg.GetPathName();
	}
	return "";
}

CString CLogFileOperation::ConvertFromEpochToHumanReadableDate(unsigned long logTime)
{
	//CString strHumanReadableDate = "";
	time_t tmLog = logTime; 
	struct tm ts; 
	char buf[80] = {0};
	//time(&tmLog); // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz" 
	localtime_s(&ts, &tmLog);	// ת��Ϊ����ʱ��
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &ts); 
	return (CString)buf; 
}

CString CLogFileOperation::IsFindRestartRecord(CString *strRecord, CString strRecordTime, int* numExp, int* numCtrl)
{
	CString res;
	if (strRecord == NULL || strRecord->Find("[USER]: create queue,USER.queue_handle") < 0)
	{
		return res;
	}
	//res += strRecordTime;
	res = strRecordTime + "ʱ���������";
	*numExp = 0;
	*numCtrl = 0;
	return res;
}