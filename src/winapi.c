#define _UNICODE
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#define __STDC_LIB_EXT1__

#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <Shlobj.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sqlite3.h>
#include <errno.h>

#define COL_DBLUE	RGB(0x54, 0x8d, 0x9a)
#define COL_LBLUE	RGB(0x7a, 0xa3, 0xae)

#define COL_WHITE	RGB(0xff, 0xff, 0xff)
#define COL_BLACK	RGB(0x00, 0x00, 0x00)

#define COL_LGREEN	RGB(0x4f, 0xd1, 0x73)
#define COL_MGREEN	RGB(0x4f, 0xd1, 0x90)
#define COL_DGREEN	RGB(0x4f, 0xc1, 0xa7)

#include "podcaster.h"
#include "resource.h"


#include "podded_utils.c"
#include "xml_parse.c"
#include "sqlite.c"
#include "podded_curl.c"

#include "podded_lv_rss.c"
#include "podded_lv_podcast.c"


extern inline RECT BTN_LIB_DIM(void)
{
	RECT result = {
		0, 50, 100, 100
	};
	return(result);
}

extern inline RECT BTN_LONLINE_DIM(void)
{
	RECT result = {
		0, 85, -105, 0
	};
	return(result);
}

extern inline RECT BTN_SAVE_DIM(void)
{
	RECT result = {
		0, 115, -105, 0
	};
	return(result);
}

extern inline RECT BTN_DOWNLOAD_DIM(void)
{
	RECT result = {
		0, 145, -105, 0
	};
	return(result);
}

extern inline RECT BTN_SEARCH_DIM(void)
{
	RECT result = {
		0, 55, -160, 0
	};
	return(result);
}

extern inline RECT BTN_DELETE_DIM(void)
{
	RECT result = {
		105, 55, 0, 0
	};
	return(result);
}

extern inline RECT BTN_CHECKALL_DIM(void)
{
	RECT result = {
		310, 55, 0, 0
	};
	return(result);
}

extern inline RECT EDT_URL_DIM(void)
{
	RECT result = {
		415, 55, -580, 0
	};
	return(result);
}

extern inline RECT LV_RSS_DIM(void)
{
	RECT result = {
		105, 85, 0, -125
	};
	return(result);
}

extern inline RECT LV_PODCAST_DIM(void)
{
	RECT result = {
		310, 85, -420, -125
	};
	return(result);
}

extern inline RECT ADD_RECT(RECT a,
		       RECT b)
{
	RECT result = {
		a.left + b.left, 	
		a.top + b.top, 	
		a.right + b.right, 	
		a.bottom + b.bottom, 	
	};
	return(result);
}

#include "podded_winapi_draw.c"

static void win32_ResizeAll(HWND window)
{
	RECT clientRect;
	GetClientRect(window, &clientRect);
	
	struct program_state *state = (struct program_state *)
		GetClassLongPtr(window, 0);
	state->headerRect = clientRect;
	state->headerRect.bottom = 50;
	
	state->sideRect = clientRect;
	state->sideRect.top = 50;
	state->sideRect.right = 100;	
	
	RECT dimRect = BTN_LONLINE_DIM();
	RECT relRect = ADD_RECT(clientRect, dimRect);
	SetWindowPos(GetDlgItem(window, BTN_LONLINE), HWND_TOP,
		relRect.right, relRect.top,
		100, 25, 0);
		
	dimRect = BTN_DOWNLOAD_DIM();
	relRect = ADD_RECT(clientRect, dimRect);
	SetWindowPos(GetDlgItem(window, BTN_DOWNLOAD), HWND_TOP,
		relRect.right, relRect.top,
		100, 25, 0);	
		
	dimRect = BTN_CHECKALL_DIM();
	relRect = ADD_RECT(clientRect, dimRect);
	SetWindowPos(GetDlgItem(window, BTN_CHECKALL), HWND_TOP,
		relRect.left, relRect.top,
		100, 25, 0);		
		
	dimRect = BTN_SAVE_DIM();
	relRect = ADD_RECT(clientRect, dimRect);
	SetWindowPos(GetDlgItem(window, BTN_SAVE), HWND_TOP,
		relRect.right, relRect.top,
		100, 25, 0);	
		
	dimRect = EDT_URL_DIM();
	relRect = ADD_RECT(clientRect, dimRect);
	SetWindowPos(GetDlgItem(window, EDT_URL), HWND_TOP,
		relRect.left, relRect.top,
		relRect.right, 25, 0);		
		
	dimRect = BTN_SEARCH_DIM();
	relRect = ADD_RECT(clientRect, dimRect);
	SetWindowPos(GetDlgItem(window, BTN_SEARCH), HWND_TOP,
		relRect.right, relRect.top,
		50, 25, 0);			
		
	dimRect = LV_PODCAST_DIM();
	relRect = ADD_RECT(clientRect, dimRect);
	SetWindowPos(GetDlgItem(window, LV_PODCAST), HWND_TOP,
		relRect.left, relRect.top,
		relRect.right, relRect.bottom, 0);	
		
	dimRect = LV_RSS_DIM();
	relRect = ADD_RECT(clientRect, dimRect);
	SetWindowPos(GetDlgItem(window, LV_RSS), HWND_TOP,
		relRect.left, relRect.top,
		200, relRect.bottom, 0);	
	
	dimRect = BTN_DELETE_DIM();
	relRect = ADD_RECT(clientRect, dimRect);
	SetWindowPos(GetDlgItem(window, BTN_DELETE), HWND_TOP,
		relRect.left, relRect.top,
		50, 25, 0);	
		
	SetWindowPos(GetDlgItem(window, STATUS), HWND_BOTTOM,
		0, 0,
		0, 0, 0);	
}

static void win32_ToolTipDisplay(HWND window,
				 LPNMHDR header,
				 LPNMBCHOTITEM item)
{
	bool isEntering = (item->dwFlags & HICF_ENTERING);
	HWND status = GetDlgItem(window, STATUS);
	WCHAR message[128];
	if(isEntering) {		
		switch(header->idFrom) {
		case BTN_LONLINE: {
			wcscpy(message, L"Open the browser to the " 
				"selected episodes location");
			break;	
		} case BTN_SAVE: {
			wcscpy(message, L"Save this podcast");
			break;
		} case BTN_SEARCH: {
			wcscpy(message, L"Get list of podcasts from URL");
			break;
		} case BTN_DELETE: {
			wcscpy(message, L"Delete Podcast");
			break;
		} default: {INVALID_PATH;}	
		}
		SendMessage(status, SB_SETTEXT, (WPARAM)0, 
			(LPARAM)message);
	} else {
		SendMessage(status, SB_SETTEXT, (WPARAM)0, 
			(LPARAM)L"");
	}
}

extern void win32_AddChannelToState(struct program_state *state,
				    struct podcast_content *content,
				    char *searchUrl)
{
	strcpy(state->title, content->title);
	strcpy(state->rssLink, searchUrl);	
	BITSET(state->flags, STATE_SEARCHDATA);	
}

static void win32_AddContentToListView(HWND window,
				       struct podcast_content *content)
{
	LVITEMW lvitm = {};
	lvitm.mask = LVIF_TEXT;
	WCHAR buffer[512];	
	HWND lv = win32_LvPodcastGetHwnd(window);
	ListView_DeleteAllItems(lv);
	
	for(int32_t i = 0; i <= content->episodeCount; ++i) 
	{
		lvitm.iItem = i;
		
		lvitm.iSubItem = 0;		
		win32_Str8To16(content->episodes[i].title, 
			buffer, MAX_TITLE_LEN);
		lvitm.pszText = buffer;
		ListView_InsertItem(lv, &lvitm);
		
		lvitm.iSubItem = 1;
		win32_Str8To16(content->episodes[i].publishDate, 
			buffer, MAX_TITLE_LEN);		
		lvitm.pszText = buffer;
		ListView_SetItem(lv, &lvitm);
		
		lvitm.iSubItem = 2;
		win32_Str8To16(content->episodes[i].browserLink, 
			buffer, MAX_URL_LEN);			
		lvitm.pszText = buffer;
		ListView_SetItem(lv, &lvitm);
		
		lvitm.iSubItem = 3;
		win32_Str8To16(content->episodes[i].downloadLink, 
			buffer, MAX_URL_LEN);			
		lvitm.pszText = buffer;
		ListView_SetItem(lv, &lvitm);
	}
}

/*
*	Aquires all saved rss entries from the database 
*	and adds them to the list view
*/
static void win32_UpdateRssListview(HWND window)
{
	struct rss_list_content *list =
		calloc(1, sizeof(struct rss_list_content));
		
	if(db_GetAllRss(list)) 
	{
		LVITEMW lvitm = {};
		lvitm.mask = LVIF_TEXT;
		WCHAR nameBuffer[512];			
		WCHAR idBuffer[8];	
		HWND lv = GetDlgItem(window, LV_RSS);
		ListView_DeleteAllItems(lv);	
	
		for(int32_t i = 0; i < list->rssCount; ++i) 
		{
			lvitm.iItem = i;	
			lvitm.iSubItem = 0;	
			wsprintf(idBuffer, L"%d", list->rssFiles[i].id);		
			lvitm.pszText = idBuffer;
			ListView_InsertItem(lv, &lvitm);
			
			lvitm.iSubItem = 1;		
			MultiByteToWideChar(CP_UTF8, 0, list->rssFiles[i].title, 
				512, nameBuffer, 512);
			lvitm.pszText = nameBuffer;	
			ListView_SetItem(lv, &lvitm);
		}	
	}	
	free(list);
}

/*
*	Gets the id stored in list view,
*	converts it from utf16 to utf8,
*	then deletes the corresponding record,
*	and saved rss file.
*/
static void win32_DeleteRssLink(HWND window)
{
	int32_t id = win32_LvRssGetId(window);
	db_DeleteRssLink(id);
	WCHAR path[MAX_URL_LEN];
	win32_LvRssGetPath(window, path);	
	DeleteFileW(path);
	
	win32_UpdateRssListview(window);
	Button_Enable(GetDlgItem(window, BTN_DELETE), FALSE);
}

/*
*	
*/
static void win32_SaveRssLink(HWND window)
{
	struct program_state *state = (struct program_state *)
		GetClassLongPtr(window, 0);
	db_AddRssLink(state->title, state->rssLink);	
	win32_UpdateRssListview(window);
	WIN32_FIND_DATAW fileData;
	HANDLE fileHandle;
	fileHandle = FindFirstFileW(L"transient.rss", &fileData);
	if(fileHandle != INVALID_HANDLE_VALUE) 
	{
		char buffer[MAX_URL_LEN];
		podded_BuildRssPath(state->title, buffer);
		WCHAR path[MAX_URL_LEN];
		win32_Str8To16(buffer, path, MAX_URL_LEN);
		MoveFileExW(L"transient.rss", path, MOVEFILE_REPLACE_EXISTING|
			MOVEFILE_COPY_ALLOWED);	
		CloseHandle(fileHandle);	
	} 
	else {printf("fail\n");}
}
/*
*	Destination of 'listen online' button (BTN_LONLINE)
*
*	Gets browser link fromt the list view,
*	then opens the link in default browser.
*/
static void win32_OpenBrowserLink(HWND window)
{
	HWND listView = GetDlgItem(window, LV_PODCAST);
	INT id = ListView_GetNextItem(listView, -1, LVNI_SELECTED);
	WCHAR url[512];	
	ListView_GetItemText(listView, id, 2, url, 512);				
	ShellExecuteW(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL);
}

static void win32_GetEpisodesFromRssSaved(HWND window)
{		
	char pathBuffer[MAX_URL_LEN];
	win32_LvRssGetPath(window, pathBuffer);
	
	int32_t id = win32_LvRssGetId(window);
	struct rss_content rss = {};	
	db_GetRssById(id, &rss);
	
	if(curl_GetRssFromUrl(rss.url, pathBuffer)) 
	{
		struct podcast_content *content =
			calloc(1, sizeof(struct podcast_content));
		
		xml_ParseDocument(pathBuffer, content);		
		
		struct program_state *state = (struct program_state *)
			GetClassLongPtr(window, 0);
		BITSET(state->flags, STATE_SEARCHDATA);			
		win32_AddContentToListView(window, content);		
		
		free(content);	
	}
}
/*
*	Destination of 'go' button (BTN_SEARCH)
*
*	Gets url from the edit control, downlads the
* 	corresponding rss, parses it, outputs its 
*	content to the list view, and adds the title
* 	and url to program_state in case the user
* 	wants to save it. 
*/
static void win32_GetEpisodesFromRssOnline(HWND window)
{	
	HWND edit = GetDlgItem(window, EDT_URL);
	if(!Edit_GetTextLength(edit)) {return;} 
	WCHAR urlBuffer[256];
	Edit_GetText(edit, urlBuffer, -1);
	char urlBufferUtf8[512];
	WideCharToMultiByte(CP_UTF8, 0, urlBuffer, -1, 
		urlBufferUtf8, 512, NULL, NULL);
	if(curl_GetRssFromUrl(urlBufferUtf8, "transient.rss")) 
	{
		struct podcast_content *content =
			calloc(1, sizeof(struct podcast_content));
		
		xml_ParseDocument("transient.rss", content);		
		
		struct program_state *state = (struct program_state *)
			GetClassLongPtr(window, 0);
		win32_AddChannelToState(state, content, urlBufferUtf8);	
		Button_Enable(GetDlgItem(window, BTN_SAVE), TRUE);	
		win32_AddContentToListView(window, content);		
		
		free(content);	
	}
}

/*
*	Populatues a struct of destinations and 
*	urls for slected episodes for download.
*/
static void win32_DownloadCheckedEpisodes(HWND window)
{	
	struct program_state *state = (struct program_state *)
		GetClassLongPtr(window, 0);
	//	Check if download already active
	// 	TODO: add to exsisting job
	if(BITCHECK(state->flags, STATE_ACTIVEDL)) {
		return;
	}	
	
	BROWSEINFOW bi = {};
	WCHAR destPath[MAX_PATH_LEN];
	bi.hwndOwner 		= window;	
	bi.pszDisplayName	= destPath;
	bi.lpszTitle		= L"Select a distination";
	bi.ulFlags		= BIF_USENEWUI|BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST lpItem = SHBrowseForFolderW(&bi);	
	SHGetPathFromIDList(lpItem, destPath);
	
	state->dlBatch.transfersCount = win32_LvPodcastGetCheckedItemCount(window);
	if(!state->dlBatch.transfersCount) {return;}
	
	state->dlBatch.downloads = calloc(1, 
		sizeof(struct download_task) * state->dlBatch.transfersCount);		
	win32_LvPodcastInitGetCheckedItem(window, destPath, &state->dlBatch);	

	curl_InitMultiDownload(&state->dlBatch);

	BITSET(state->flags, STATE_ACTIVEDL);	
}

static void win32_ProcessButton(HWND window,
				WORD id)
{
	if(id == BTN_SEARCH) 		{win32_GetEpisodesFromRssOnline(window);} 
	else if(id == BTN_LONLINE)  	{win32_OpenBrowserLink(window);} 
	else if(id == BTN_SAVE) 	{win32_SaveRssLink(window);} 
	else if(id == BTN_DELETE)   	{win32_DeleteRssLink(window);}	
	else if(id == BTN_DOWNLOAD)   	{win32_DownloadCheckedEpisodes(window);}	
	else if(id == BTN_CHECKALL)   	{win32_LvPodcastGetCheckAll(window);}	
}

LRESULT CALLBACK WindowProc(HWND window,
			    UINT msg,
			    WPARAM wParam,
			    LPARAM lParam)
{
	LRESULT result = 0;
	
	switch(msg) {	
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	} case WM_CREATE:{			
		
		assert(db_Init());
		
		struct program_state *state = 
			calloc(1, sizeof(struct program_state));
		SetClassLongPtrW(window, 0, (ULONG_PTR)state);	
		
		HINSTANCE hInstance = GetModuleHandle(NULL);
		
		CoInitialize(NULL);
		INITCOMMONCONTROLSEX commCtrl;
		commCtrl.dwSize = sizeof(INITCOMMONCONTROLSEX);
		commCtrl.dwICC = ICC_TAB_CLASSES|
			ICC_STANDARD_CLASSES|ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&commCtrl);
		
		state->blueBrush = CreateSolidBrush(COL_DBLUE);
		state->blueBrushL = CreateSolidBrush(COL_LBLUE);
		state->whiteBrush = CreateSolidBrush(COL_WHITE);
		
		state->greenBrushL = CreateSolidBrush(COL_LGREEN);
		state->greenBrushM = CreateSolidBrush(COL_MGREEN);
		state->greenBrushD = CreateSolidBrush(COL_DGREEN);
		
		state->logoBmp = LoadBitmapW(hInstance, MAKEINTRESOURCE(BMP_LOGO));
		assert(state->logoBmp);
		
		HDC dc = GetDC(window);
		int32_t btnFontBigDim = -MulDiv(11, GetDeviceCaps(dc, LOGPIXELSY), 72);
		int32_t btnFontBigSmall = -MulDiv(8, GetDeviceCaps(dc, LOGPIXELSY), 72);
		ReleaseDC(window, dc);
		
		state->btnFontBig = CreateFontW(btnFontBigDim, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, L"Arial Bold");
		state->btnFontSmall = CreateFontW(btnFontBigSmall, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, L"Arial");	
			
		RECT clientRect;
		GetClientRect(window, &clientRect);		
		
		HWND btn1 = CreateWindowExW(0, WC_BUTTON, NULL,
			WS_CHILD|BS_PUSHBUTTON|WS_VISIBLE, 
			0, 50,
			100, 100, 
			window, (HMENU)BTN_LIB, hInstance, NULL);
		SendMessage(btn1, WM_SETFONT, (WPARAM)state->btnFontBig, TRUE);	
			
		HWND btn2 = CreateWindowExW(0, WC_BUTTON, L"Btn2",
			WS_CHILD|BS_PUSHBUTTON|WS_VISIBLE, 
			clientRect.left, clientRect.top + 150,
			100, 100, 
			window, (HMENU)BTN_DLVIEW, hInstance, NULL);	
		SendMessage(btn2, WM_SETFONT, (WPARAM)state->btnFontBig, TRUE);		

		HWND listenBtn = CreateWindowExW(0, WC_BUTTON, L"Listen Online",
			WS_CHILD|BS_PUSHBUTTON|WS_VISIBLE, 
			clientRect.right - 120, clientRect.top + 20,
			100, 25, 
			window, (HMENU)BTN_LONLINE, hInstance, NULL);	
		Button_Enable(listenBtn, FALSE);	
		SendMessage(listenBtn, WM_SETFONT, (WPARAM)state->btnFontSmall, TRUE);

		HWND btn4 = CreateWindowExW(0, WC_BUTTON, NULL,
			WS_CHILD|BS_PUSHBUTTON|WS_VISIBLE, 
			clientRect.right - 120, clientRect.top + 55,
			100, 25, 
			window, (HMENU)BTN_SAVE, hInstance, NULL);
		Button_Enable(btn4, FALSE);
		SendMessage(btn4, WM_SETFONT, (WPARAM)state->btnFontSmall, TRUE);

		HWND btn5 = CreateWindowExW(0, WC_BUTTON, NULL,
			WS_CHILD|BS_PUSHBUTTON|WS_VISIBLE, 
			0, 0, 0, 0,
			window, (HMENU)BTN_DOWNLOAD, hInstance, NULL);	
		SendMessage(btn5, WM_SETFONT, (WPARAM)state->btnFontSmall, TRUE);	
				
		win32_LvRssInit(window);
		win32_LvPodcastInit(window);
		
		HWND deleteRss = CreateWindowExW(0, WC_BUTTON, L"Delete Podcast",
			WS_CHILD|BS_PUSHBUTTON|WS_VISIBLE, 
			0, 0, 0, 0,
			window, (HMENU)BTN_DELETE, hInstance, NULL);
		Button_Enable(deleteRss, FALSE);
		SendMessage(deleteRss, WM_SETFONT, (WPARAM)state->btnFontSmall, TRUE);		

		HWND checkAll = CreateWindowExW(0, WC_BUTTON, L"Check all",
			WS_CHILD|BS_PUSHBUTTON|WS_VISIBLE, 
			0, 0, 0, 0,
			window, (HMENU)BTN_CHECKALL, hInstance, NULL);		
		SendMessage(checkAll, WM_SETFONT, (WPARAM)state->btnFontSmall, TRUE);		
						
		HWND status = CreateWindowExW(0, STATUSCLASSNAME, NULL,
			WS_CHILD|SBARS_SIZEGRIP|WS_VISIBLE, 
			0, 0, 0, 0, 
			window, (HMENU)STATUS, hInstance, NULL);	
		INT borders[] = {clientRect.right * 0.7f, -1};	
		SendMessage(status, SB_SETPARTS, (WPARAM)2, (LPARAM)borders);
		SendMessage(status, SB_SETMINHEIGHT, (WPARAM)35, 0);
		SendMessage(status, SB_SETBKCOLOR , 0, (LPARAM)COL_DBLUE);
		SendMessage(status, WM_SETFONT, (WPARAM)state->btnFontSmall, TRUE);	
		
		CreateWindowExW(WS_EX_CLIENTEDGE, WC_EDIT, NULL,
			WS_CHILD|ES_AUTOHSCROLL|WS_VISIBLE, 
			0, 0, 0, 0, 
			window, (HMENU)EDT_URL, hInstance, NULL);
			
		HWND btnGO = CreateWindowExW(0, WC_BUTTON, L"Go",
			WS_CHILD|BS_PUSHBUTTON|WS_VISIBLE|SBT_OWNERDRAW , 
			clientRect.left + 560, clientRect.top + 20,
			50, 25, 
			window, (HMENU)BTN_SEARCH, hInstance, NULL);
		SendMessage(btnGO, WM_SETFONT, (WPARAM)state->btnFontSmall, TRUE);		
		
		win32_UpdateRssListview(window);
		
		break;
	} case WM_SIZE: {		
		win32_ResizeAll(window);
		break;
	} case WM_PAINT: {
		win32_Draw(window);
		break;
	} case WM_COMMAND: {		
		/*	Check handled button ids, and send them forwards	*/
		if(LOWORD(wParam) >= BTN_LONLINE && LOWORD(wParam) <= BTN_CHECKALL) 
		{
			win32_ProcessButton(window, LOWORD(wParam));
		}		
		break;
	} case WM_NOTIFY: {	
		//	Handle custom draw events
		if(((LPNMHDR)lParam)->code == NM_CUSTOMDRAW) 
		{
			switch(((LPNMHDR)lParam)->idFrom) {			
			case BTN_DLVIEW: 
			case BTN_LIB: {
				result = win32_DrawSqrBtn(window, (LPNMCUSTOMDRAW)lParam);
				break;
			} case BTN_SEARCH:
			case BTN_LONLINE:
			case BTN_DOWNLOAD:
			case BTN_CHECKALL:
			case BTN_SAVE:
			case BTN_DELETE: {
				result = win32_DrawRndBtn(window, (LPNMCUSTOMDRAW)lParam);
				break;
			} case LV_PODCAST:	
			case LV_RSS: {
				result = win32_DrawLv((LPNMLVCUSTOMDRAW)lParam);
				break;
			}	
			}
		}
		/*	Handles the tooltips for various buttons		*/
 		if(((LPNMHDR)lParam)->idFrom >= BTN_LONLINE && ((LPNMHDR)lParam)->idFrom <= BTN_DELETE) 
		{
			if(((LPNMHDR)lParam)->code == BCN_HOTITEMCHANGE) 
			{
				win32_ToolTipDisplay(window, (LPNMHDR)lParam, 
					(LPNMBCHOTITEM)lParam);				
			}		
		} 
		/*	Listview notifications.					*/	
		else if(((LPNMHDR)lParam)->idFrom == LV_PODCAST) 
		{
			if(((LPNMHDR)lParam)->code == LVN_ITEMACTIVATE)
			{
				Button_Enable(GetDlgItem(window, BTN_LONLINE), TRUE);					
			} 			
		} 
		else if(((LPNMHDR)lParam)->idFrom == LV_RSS) 
		{
			if(((LPNMHDR)lParam)->code == LVN_ITEMACTIVATE ) 
			{
				Button_Enable(GetDlgItem(window, BTN_DELETE), TRUE);					
			}
			else if(((LPNMHDR)lParam)->code == NM_DBLCLK) 
			{
				win32_GetEpisodesFromRssSaved(window);				
			}
		}
		break;	
	} default: {
		result = DefWindowProcW(window, msg, wParam, lParam);
		break;
	}	
	}
	return(result);
}

int CALLBACK WinMain(HINSTANCE hInstance, 
		     HINSTANCE hPrevInstance, 
		     LPSTR lpCmdLine, 
		     int nCmdShow)
{
	WNDCLASSEXW wc 		= {};
	wc.cbSize 		= sizeof(WNDCLASSEXW);
	wc.style 		= CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc  	= WindowProc;
	wc.cbClsExtra 		= sizeof(ULONG_PTR);
	wc.hInstance		= hInstance;
	wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName	= L"MainClass";
	if(!RegisterClassExW(&wc)) {printf("!RegisterClassExW\n"); return(1);}
		
	HWND window = CreateWindowExW(0, wc.lpszClassName, L"Podded", 
		WS_OVERLAPPEDWINDOW|WS_VISIBLE, 
		CW_USEDEFAULT, CW_USEDEFAULT, 1000, 500, 
		NULL, NULL, hInstance, NULL);
	if(!window) {printf("!CreateWindowExW\n"); return(1);}	
	
	curl_Init();
	LIBXML_TEST_VERSION
	
	struct program_state *state = (struct program_state *)
		GetClassLongPtr(window, 0);
	
	BOOL msgFlag;	
	MSG msg;	
	while(1) {		
		if(BITCHECK(state->flags, STATE_ACTIVEDL)) 
		{					
			PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE);
			if(msg.message == WM_QUIT) {break;}		
			if(curl_MultiDownoadUpdate(&state->dlBatch)) 
			{			
				free(state->dlBatch.downloads);	
				ZeroMemory(&state->dlBatch, 
					sizeof(struct download_batch));	
				BITCLEAR(state->flags, STATE_ACTIVEDL);
			}
		} 
		else {
			msgFlag = GetMessageW(&msg, NULL, 0, 0);
			if(!msgFlag) {break;}
		}	
		TranslateMessage(&msg);			
		DispatchMessage(&msg);					
	}	
	return(0);
}
	