#include "stdafx.h"
#include "loghandle.h"
#include "string.h"
//1字节对齐
	
enum{MAX_CLIENTS_NUM=16, RECORD_FLAG=0xAA, RECORD_FLAG_Number=0x48};
enum{IntL_1 = 1, IntL_2 = 2, IntL_4 = 4};
	
////////////////non member function////////////////////////////
const int GetIntegerLengthInLog(const IntegerLength eIntL)
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
	return iIntSizeInLog;
}

int LogFile::ReadIntegerValueFromLog(void* pIntValue, IntegerLength eIntL)
{
	int iIntSizeInLog = GetIntegerLengthInLog(eIntL);   //得到2,4,8
	char chFlag = NULL;
	int sizeRead = fread(chFlag, 1, 1, &rLogFile._file);
	//int sizeRead = rstreamFile.read(ch, sizof(ch));
	if (chFlag != '\n') 
	{
		fseek(_file, -1, SEEK_CUR);
		//rstreamFile.seekg(-1, CFile::current);
	}

	char* pszBuf = new char[iIntSizeInLog + 1];     //设置一个指针（pszBuf[iIntSizeInLog+1]），读取文件内容
	memset(pszBuf, 0, iIntSizeInLog + 1);

	sizeRead = fread(pszBuf, 1, iIntSizeInLog, _file);
	//sizeRead = rFile.Read(pszBuf, iIntSizeInLog);
	unsigned long lValue = strtoul(pszBuf, NULL, 16);    //读取出的内容转换为16进制
	if (eIntL == IntL_1)		          //数据类型转换？
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
	delete[] pszBuf;
	return sizeRead;
}
////////////////Determine if the file is a binary file////////// 
BOOL LogFile::BinaryFile()
{
	//以日志文件的第一个字节作为判断依据
	char cRecordFlag = NULL;
	int sizeRead = fread(&cRecordFlag, 1, 1, _file);
	//int sizeRead = rstreamFile.Read(&cRecordFlag, 1);
	if (sizeRead > 0)
	{
		fseek(_file, -1, SEEK_CUR);
		return cRecordFlag == RECORD_FLAG;
	}
	return false;//默认为新格式
}
///////////////Read fixed size data/////////////////////////////////
REPLAY_DATA LogFile::ReadDataFromFile()
{
	int iRetLen = 0;
	int sizeRead = 0;
	REPLAY_DATA dataReplay;
	if (BinaryFile())
	{//二进制回放格式，即老的回放格式
		//sizeRead = fread(&dataReplay, 1, sizeof(REPLAY_DATA), &rFile);
		memset(&dataReplay, 0, sizeof(dataReplay));
		sizeRead = fread(&dataReplay.m_flag, 1, 1, &rFile);
		sizeRead = fread(&dataReplay.m_time, 1, 4, &rFile);
		sizeRead = fread(&dataReplay.m_len, 1, 2, &rFile);
		sizeRead = fread(&dataReplay.m_data, 1, dataReplay.m_len, &rFile);
		sizeRead = fread(&dataReplay.m_position, 1, 4, &rFile);	
		if (sizeRead <= 0)
		{
			return 0;
		}
		if (dataReplay.m_len > 1000)
		{
			return 0;
		}
		dataReplay.m_position = *(long*)&dataReplay.m_data[dataReplay.m_len];            
		dataReplay.m_data[dataReplay.m_len] = 0;
		iRetLen = sizeof(REPLAY_DATA) - (MAX_REPLAY_DATA_LENGTH-dataReplay.m_len);
	}
	else
	{//新的回放格式
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
		sizeRead = fread(dataReplay.m_data, 1, dataReplay.m_len, &rFile);
		//sizeRead = rFile.Read(&dataReplay.m_data, dataReplay.m_len);
		if (sizeRead <= 0)
		{
			return 0;
		}
		sizeRead = ReadIntegerValueFromLog(rFile, &dataReplay.m_position, IntL_4);
		if (sizeRead <= 0)
		{
			return 0;
		}
	}
	return dataReplay;
}

std::string LogFile::GetLogBeaginTime(unsigned long ulBeginTime)
{
	REPLAY_DATA replayData = ReadDataFromFile();
	//std::string strTime;
	if (replayData == NULL)
	{
		fseek(_file, 0, SEEK_SET);
		ulBeginTime = 0;
		return "read first record error!";
	}
	std::string strTime = EpocoConversion(replayData.m_time);	//TimeStamp convert to normal date
	ulBeginTime = replayData.m_time;
	fseek(_file, 0, SEEK_SET);
	return strTime;
}

std::string LogFile::GetLogEndTime(unsigned long ulEndTime)
{
	char chFalg, chWrap = NULL;
	unsigned long ulLastRecordTime = 0;
	std::string strTime
	int wrapCount = 0;
	fseek(_file, 0, SEEK_END);
	if (BinaryFile())
	{
		while(1)
		{
			fread(&chWrap, sizeof(chWrap), 1, _file);
			if(chWrap != '\n')
			{
				fseek(_file, -2, SEEK_CUR);
				continue;
			}
			wrapCount++;
			if (wrapCount == 2)
			{
				fseek(_file, 5, 1, SEEK_CUR);
				fread(&ulLastRecordTime, sizeof(ulLastRecordTime), 1, _file);
				strTime = EpocoConversion(ulLogFirstRecord);
				break;
			}
		}
	}
	else
	{
		while (1)
		{
			fread(&chWrap, sizeof(chWrap), 1， _file);
			if(chWrap != '\n')
			{
				fseek(_file, -2, SEEK_CUR);
				continue;
			}
			wrapCount++;
			if (wrapCount == 2)
			{
				fseek(_file, 10, SEEK_CUR);
				fread(&ulLastRecordTime, sizeof(ulLastRecordTime), 1, _file);
				strTime = EpocoConversion(ulLastRecordTime);
				break;
			}
		}
	}
	ulEndTime = ulLastRecordTime;
	rewind(_file);	//将文件指针指向文件头
	//strLogFirstRecordTime = ConvertFromEpochToHumanReadableDate(ulLogFirstRecord);	//TimeStamp convert to normal date
	//strLogLastRecordTime = ConvertFromEpochToHumanReadableDate(ulLogLastRecord);	//TimeStamp convert to normal date
	//strLogTimeWidth = strLogFirstRecordTime + "--" + strLogLastRecordTime;	
	//m_Editcontrol.SetWindowText(strLogTimeWidth);
	//timestart.SetFormat("yyyy-MM-dd HH:mm:ss");	
	//timestop.SetFormat("yyyy-MM-dd HH:mm:ss");
	//CTime t_Start(ulLogFirstRecord), t_Stop(ulLogLastRecord);
	//timestart.SetTime(&t_Start);
	//timestop.SetTime(&t_Stop);
	return strTime
}

/*int LogFile::SetLogTimeWidthAndDefaultQueryTime(LogFile &logFile, CEdit &m_Editcontrol, CDateTimeCtrl &timestart, CDateTimeCtrl &timestop)
{	// 开始时间默认为日志第一条记录的时间，结束时间为日志最后一条记录的时间
	if (logFile == NULL)
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
	if (!GetFileISBinary(rFile))
	{
		flagNewRecord = true;
	}
	int readSize = logFile.GetReplayDataFromLog(rFile, dataReplay);
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
}*/

void CLogFileOperation::ChangeToTime(unsigned long mTime, char *timeTmp, int num)
{
	time_t tick = mTime;
	struct tm tm;
	localtime_s(&tm, &tick);
	strftime(timeTmp, num, "%Y-%m-%d %H:%M:%S", &tm);
}

void CLogFileOperation::GetQueryTimeWin(CDateTimeCtrl &TimeStart, CDateTimeCtrl &TimeStop, CTime &startTime, CTime &stopTime)
{
	//获取显示的时间
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
	CString strFilter = "文本文件(*.LOG)| *.LOG||";
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
{	// 不需要判断strFilter是否为空,为空只是不能过滤文件不影响选取文件.
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
	localtime_s(&ts, &tmLog);	// 转换为本地时区
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
	res = strRecordTime + "时，软件重启";
	*numExp = 0;
	*numCtrl = 0;
	return res;
}