extern LRESULT win32_DrawLv(LPNMLVCUSTOMDRAW lvcd)
{
	LRESULT result = 0;
	
	if(lvcd->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		result = CDRF_NOTIFYITEMDRAW;
	} 
	else if(lvcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		result = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if(lvcd->nmcd.dwDrawStage == 
		(CDDS_ITEMPREPAINT|CDDS_SUBITEM))
	{
		COLORREF textCol;
		COLORREF backCol;
		if(lvcd->nmcd.dwItemSpec % 2 == 0)
		{
			textCol = COL_WHITE;
			backCol = COL_LBLUE;
		} else {
			textCol = COL_BLACK;
			backCol = COL_WHITE;
		}
		lvcd->clrText = textCol;	
		lvcd->clrTextBk = backCol;	
		
		result = CDRF_DODEFAULT;
	}
	return(result);
}

extern LRESULT win32_DrawSqrBtn(HWND window,
			        LPNMCUSTOMDRAW nmcd)
{
	LRESULT result = 0;
	
	if(nmcd->dwDrawStage == CDDS_PREPAINT) 
	{		
		struct program_state *state = (struct program_state *)
			GetClassLongPtr(window, 0);
		
		HBRUSH *brush;	
		SetTextColor(nmcd->hdc, COL_WHITE);	
		if(nmcd->uItemState & CDIS_SELECTED) 
		{
			brush = &state->greenBrushL;			
		} 
		else if(nmcd->uItemState & CDIS_HOT) 
		{
			brush = &state->greenBrushM;			
		} 
		else 
		{			
			brush = &state->greenBrushD;
		}	
			
		FillRect(nmcd->hdc, &nmcd->rc, *brush);
		
		SetBkMode(nmcd->hdc, TRANSPARENT);
		WCHAR text[64];
		LoadStringW(GetModuleHandle(NULL), nmcd->hdr.idFrom + 400, text, 64); 
		DrawText(nmcd->hdc, text, -1, &nmcd->rc, 
			DT_SINGLELINE|DT_VCENTER|DT_CENTER);	
		result = (CDRF_SKIPDEFAULT|CDRF_SKIPPOSTPAINT);
	}	
	return(result);
}

extern LRESULT win32_DrawRndBtn(HWND window,
			        LPNMCUSTOMDRAW nmcd)
{
	LRESULT result = 0;
	
	if(nmcd->dwDrawStage == CDDS_PREPAINT) 
	{		
		
		if(nmcd->uItemState & CDIS_DISABLED) 
		{
			SetDCBrushColor(nmcd -> hdc, COL_WHITE);			
			SetDCPenColor(nmcd -> hdc, COL_LBLUE);	
			SetTextColor(nmcd->hdc, COL_LBLUE);	
		} 
		else if(nmcd->uItemState & CDIS_HOT) 
		{
			SetDCBrushColor(nmcd -> hdc, COL_LBLUE);			
			SetDCPenColor(nmcd -> hdc, COL_LBLUE);	
			SetTextColor(nmcd->hdc, COL_WHITE);		
		} 
		else 
		{			
			SetDCBrushColor(nmcd -> hdc, COL_DBLUE);
			SetDCPenColor(nmcd -> hdc, COL_DBLUE);	
			SetTextColor(nmcd->hdc, COL_WHITE);		
		}	
		SelectObject(nmcd -> hdc, GetStockObject(DC_BRUSH));	
		SelectObject(nmcd -> hdc, GetStockObject(DC_PEN));	
		
		FillRect(nmcd->hdc, &nmcd->rc, GetStockObject(WHITE_BRUSH));
		RoundRect(nmcd->hdc, nmcd->rc.left, 
                        nmcd->rc.top, 
                        nmcd->rc.right, 
                        nmcd->rc.bottom, 10, 10);
		
		SetBkMode(nmcd->hdc, TRANSPARENT);
		WCHAR text[64];
		LoadStringW(GetModuleHandle(NULL), nmcd->hdr.idFrom + 400, text, 64); 
		DrawText(nmcd->hdc, text, -1, &nmcd->rc, 
			DT_SINGLELINE|DT_VCENTER|DT_CENTER);	
		result = (CDRF_SKIPDEFAULT|CDRF_SKIPPOSTPAINT);
	}	
	return(result);
}

extern void win32_Draw(HWND window)
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(window, &ps);
	struct program_state *state = (struct program_state *)
		GetClassLongPtr(window, 0);
	FillRect(dc, &state->headerRect, state->blueBrush);	
	FillRect(dc, &state->sideRect, state->greenBrushD);	
	
	HDC hdcMem = CreateCompatibleDC(dc);
	HBITMAP bmp = SelectObject(hdcMem, state->logoBmp);			
	BITMAP bm;
	GetObject(state->logoBmp, sizeof(bm), &bm);
	
	BitBlt(dc, 15, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, bmp);
	DeleteDC(hdcMem);
	
	EndPaint(window, &ps);
}