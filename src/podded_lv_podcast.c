extern inline HWND win32_LvPodcastGetHwnd(HWND window)
{
	return(GetDlgItem(window, LV_PODCAST));
}

extern int32_t win32_LvPodcastGetItemCount(HWND window)
{
	HWND lv = win32_LvPodcastGetHwnd(window);
	return(ListView_GetItemCount(lv));
}

extern int32_t win32_LvPodcastGetCheckedItemCount(HWND window)
{
	HWND lv = win32_LvPodcastGetHwnd(window);
	int32_t itemCount = win32_LvPodcastGetItemCount(window);
	
	int32_t checkCount = 0;
	for(int32_t i = 0; i < itemCount; ++i) 
	{
		if(!ListView_GetCheckState(lv, i)) {continue;}		
		++checkCount;			
	}	
	return(checkCount);
}

extern void win32_LvPodcastGetCheckAll(HWND window)
{
	HWND lv = win32_LvPodcastGetHwnd(window);
	int32_t itemCount = win32_LvPodcastGetItemCount(window);
	for(int32_t i = 0; i < itemCount; ++i) 
	{		
		ListView_SetCheckState(lv, i, TRUE);	
	}
}

extern void win32_LvPodcastInitGetCheckedItem(HWND window,
					      WCHAR *pathbuffer,
					      struct download_batch *batch)
{		
	HWND lv = win32_LvPodcastGetHwnd(window);
	int32_t itemCount = win32_LvPodcastGetItemCount(window);
	if(!itemCount) {return;}		
	
	int32_t checkCount = 0;
	for(int32_t i = 0; i < itemCount; ++i) 
	{		
		if(ListView_GetCheckState(lv, i)) 
		{
			WCHAR destPath[MAX_PATH_LEN];
			wcscpy(destPath, pathbuffer);
			
			WCHAR url[MAX_URL_LEN];
			ListView_GetItemText(lv, i, 3, url, MAX_URL_LEN);
			win32_Str16To8(url, batch->downloads[checkCount].url,
				MAX_URL_LEN);
			
			WCHAR title[MAX_TITLE_LEN];			
			ListView_GetItemText(lv, i, 0, title, MAX_TITLE_LEN);	
			
			win32_CorrectFilename(title);			
			wcscat(destPath, L"\\");
			wcscat(destPath, title);
			wcscat(destPath, L".mp3");
			win32_Str16To8(destPath, 
				batch->downloads[checkCount].path,
				MAX_PATH_LEN);					
			++checkCount;	
		}					
	}
}

extern void win32_LvPodcastInit(HWND window)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	
	HWND lvPodcast = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL,
		WS_CHILD|LVS_REPORT|WS_VISIBLE, 0, 0, 0, 0,			
		window, (HMENU)LV_PODCAST, hInstance, NULL);
	ListView_SetExtendedListViewStyle(lvPodcast, LVS_EX_FULLROWSELECT|
		LVS_EX_CHECKBOXES|LVS_EX_ONECLICKACTIVATE);	
		
	LVCOLUMNW pcCols 	= {};
	pcCols.mask 		= LVCF_TEXT|LVCF_WIDTH;
	pcCols.fmt		= LVCFMT_LEFT;
	pcCols.cx		= 195;
	pcCols.iSubItem		= 0;
	pcCols.pszText		= L"Title";
	ListView_InsertColumn(lvPodcast, 0, &pcCols);	
	pcCols.iSubItem		= 1;
	pcCols.pszText		= L"Date";
	ListView_InsertColumn(lvPodcast, 1, &pcCols);
	pcCols.iSubItem		= 2;
	pcCols.pszText		= L"Online Link";
	ListView_InsertColumn(lvPodcast, 2, &pcCols);	
	pcCols.iSubItem		= 3;
	pcCols.pszText		= L"Download Link";
	ListView_InsertColumn(lvPodcast, 3, &pcCols);
}

