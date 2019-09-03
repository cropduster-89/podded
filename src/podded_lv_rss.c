extern inline HWND win32_LvRssGetHwnd(HWND window)
{
	return(GetDlgItem(window, LV_RSS));
}

extern inline void win32_LvRssGetIdStr(HWND lv,
				       WCHAR *dest)
{
	INT rowId = ListView_GetNextItem(lv, -1, LVNI_SELECTED);
	ListView_GetItemText(lv, rowId, 0, dest, 8);	
}

extern inline void win32_LvRssGetTitleStr(HWND lv,
				          WCHAR *dest)
{
	INT rowId = ListView_GetNextItem(lv, -1, LVNI_SELECTED);
	ListView_GetItemText(lv, rowId, 1, dest, MAX_URL_LEN);	
}
/*
*	Construct local file path from the title
*	contained in the list view.
*
*	Opentional UTF8 return version
*/
extern inline void _LvRssGetPathW(HWND window,
				  WCHAR *dest)
{
	HWND lv = win32_LvRssGetHwnd(window);	
	WCHAR title[MAX_TITLE_LEN];	
	win32_LvRssGetTitleStr(lv, title);	
	podded_BuildRssPath(title, dest);	
}

extern inline void _LvRssGetPathA(HWND window,
				  char *dest)
{
	HWND lv = win32_LvRssGetHwnd(window);	
	WCHAR title[MAX_TITLE_LEN];	
	win32_LvRssGetTitleStr(lv, title);	
	WCHAR path[MAX_URL_LEN];	
	podded_BuildRssPath(title, path);	
	win32_Str16To8(path, dest, MAX_URL_LEN);
}

#define win32_LvRssGetPath(a, b) _Generic((b), \
wchar_t *	:_LvRssGetPathW, \
char *		:_LvRssGetPathA) (a, b)

/*
*	Get the id contained within the listview,
*	c
*/
extern inline int32_t win32_LvRssGetId(HWND window)
{
	HWND lv = win32_LvRssGetHwnd(window);	
	WCHAR idStrW[8];	
	win32_LvRssGetIdStr(lv, idStrW);	
	char idStr[8];
	win32_Str16To8(idStrW, idStr, 8);
	char *endPtr;
	int32_t id = strtoimax(idStr, &endPtr, 10);	
	
	return(id);
}

extern void win32_LvRssInit(HWND window)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	
	HWND lvRss = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL,
		WS_CHILD|LVS_REPORT|WS_VISIBLE|LVS_SINGLESEL, 
		0, 0, 0, 0,		
		window, (HMENU)LV_RSS, hInstance, NULL);
	ListView_SetExtendedListViewStyle(lvRss, 
		LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE);		
	
	LVCOLUMNW rssCols 	= {};
	rssCols.mask 		= LVCF_TEXT|LVCF_WIDTH;
	rssCols.fmt		= LVCFMT_LEFT;
	
	rssCols.cx		= 0;
	rssCols.iSubItem	= 0;
	rssCols.pszText		= L"Id";
	ListView_InsertColumn(lvRss, 0, &rssCols);
	
	rssCols.cx		= 195;
	rssCols.iSubItem	= 1;
	rssCols.pszText		= L"Title";
	ListView_InsertColumn(lvRss, 1, &rssCols);		
}