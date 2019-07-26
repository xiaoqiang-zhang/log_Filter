// ColorListBox.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LogFileFilter.h"
#include "ColorListBox.h"

#define LISTBOX_MAX_LEN 200


// CColorListBox

IMPLEMENT_DYNAMIC(CColorListBox, CListBox)

CColorListBox::CColorListBox()
{
	width = 0;
	font1.CreateFontA(16, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_NORMAL, // nWeight 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("����"));
	font2.CreateFontA(16, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_BOLD, // nWeight 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("����"));
}

CColorListBox::~CColorListBox()
{
}


BEGIN_MESSAGE_MAP(CColorListBox, CListBox)

END_MESSAGE_MAP()



// CColorListBox ��Ϣ�������




void CColorListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct/*lpDrawItemStruct*/)
{

	// TODO:  ������Ĵ����Ի���ָ����
	if(lpDrawItemStruct->itemID == -1)
	{
		return;
	}
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC); 
	CRect rect(lpDrawItemStruct->rcItem);
	COLORREF crRedText = RGB(255,0,0);
	COLORREF crBlackText=RGB(0,0,0); 
	CString lpStr;
	
	CFont* pOldFont = pDC->SelectObject(GetFont());
	GetText(lpDrawItemStruct->itemID,lpStr );   //�õ���
	if(lpStr.Find("�������") >= 0)
	{
		pDC->SelectObject(&font2); //����������// 
		pDC->SetTextColor(crRedText);  
	}
	else
	{
		pDC->SelectObject(&font1); //����������//
		pDC->SetTextColor(crBlackText); 
	}
	DrawText(lpDrawItemStruct->hDC,lpStr, -1, &lpDrawItemStruct->rcItem, 
		DT_LEFT | DT_SINGLELINE);// restore DC colorsc
	
}


void CColorListBox::PreSubclassWindow()
{
	// TODO: �ڴ����ר�ô����/����û���
	CListBox::PreSubclassWindow();
}


void CColorListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct/*lpMeasureItemStruct*/)
{
	// TODO:  ������Ĵ�����ȷ��ָ����Ĵ�С
}


BOOL CColorListBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: �ڴ����ר�ô����/����û���
	//dwStyle |= LBS_NOINTEGRALHEIGHT | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS;
	return CListBox::Create(dwStyle, rect, pParentWnd, nID);
}


int CColorListBox::AddString(LPCTSTR s)
{
	int result = CListBox::AddString(s);
	if(result < 0)
		return result;
	updateWidth(s);
	return result;
}


void CColorListBox::updateWidth(LPCTSTR s)
{
	CClientDC dc(this);

	CFont * pFont = CListBox::GetFont();
	dc.SelectObject(pFont);

	CSize sz = dc.GetTextExtent(s, _tcslen(s));
	sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
	if(sz.cx > width)
	{ /* extend */
		width = sz.cx;
		//��֤�ػ�֮����ַ��������ضϣ���ԭ�еĿ����������
		width = width*4/3;
		CListBox::SetHorizontalExtent(width);
	} /* extend */
}
