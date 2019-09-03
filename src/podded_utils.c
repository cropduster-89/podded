#define BITCLEAR(a, n) _BITCLEAR_U64(&a, n)
extern inline void _BITCLEAR_U64(uint64_t *a,
			         int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a &= ~(1ull << n);
}

#define BITSET(a, n) _BITSET_U64(&a, n)
extern inline void _BITSET_U64(uint64_t *a,
			       int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a |= 1ull << n;
}

#define BITCHECK(a, n) _BITCHECK_U64(a, n)
extern inline bool _BITCHECK_U64(uint64_t a,
			         int32_t n)
{
	assert(n < sizeof(a) * 8);
	return(a & (1ull << n));
}

extern void win32_CorrectFilename(WCHAR *src)
{	
	for(int32_t i = 0; src[i]; ++i) {
		if(src[i] == L':') {
			src[i] = ',';
		} 
	}	
}

/*
*	Removes whitespace from a WCHAR string.
*/
extern void _RemoveSpaces16(WCHAR *src)
{
	int32_t count = 0;
	for(int32_t i = 0; src[i]; ++i) {
		if(src[i] != ' ') {
			src[count++] = src[i];
		}
	}
	src[count] = '\0'; 
}
/*
*	Removes whitespace from a char/utf8 string.
*/
extern void _RemoveSpaces8(char *src)
{
	int32_t count = 0;
	for(int32_t i = 0; src[i]; ++i) {
		if(src[i] != ' ') {
			src[count++] = src[i];
		}
	}
	src[count] = '\0'; 
}

#define RemoveSpaces(a) _Generic((a), \
wchar_t *: _RemoveSpaces16, \
char *	 : _RemoveSpaces8) (a)

/*
*	Removes spaces from a podcast title,
*	and adds local path + .rss to the string for saving.
*
*	2 versions, for utf 8 + 16
*/
extern void _BuildRssPath8(char *title,
			   char *dest)
{
	char titleBuffer[MAX_TITLE_LEN];
	strcpy(titleBuffer, title);
	RemoveSpaces(titleBuffer);
	char pathBuffer[MAX_URL_LEN] = "rss/\0";
	strcat(pathBuffer, titleBuffer);
	strcat(pathBuffer, ".rss");
	strcpy(dest, pathBuffer);
}

extern void _BuildRssPath16(wchar_t *title,
			    wchar_t *dest)
{
	wchar_t titleBuffer[MAX_TITLE_LEN];
	wcscpy(titleBuffer, title);
	RemoveSpaces(titleBuffer);
	wchar_t pathBuffer[MAX_URL_LEN] = L"rss/\0";
	wcscat(pathBuffer, titleBuffer);
	wcscat(pathBuffer, L".rss");
	wcscpy(dest, pathBuffer);
}

#define podded_BuildRssPath(a, b) _Generic((a),\
wchar_t *	: _BuildRssPath16, \
char *		: _BuildRssPath8) (a, b)

#ifdef _WIN32
/*
*	String conversion functions for win32,
*	does not execute copy if the buffer is too small.
*	
*	TODO: work out if the check is required for 8to16,
*	won't the dest always be smaller than the src?
*/
static inline bool win32_Str8To16(char *src,
			          WCHAR *dest,
			          enum string_size_max max)
{
	bool result = false;
	
	int32_t requiredSize = MultiByteToWideChar(CP_UTF8, 
		0, src, -1, dest, 0);
	if(requiredSize < max)	
	{
		MultiByteToWideChar(CP_UTF8, 0, src, max, dest, max);
		result = true;	
	}	
	else
	{
#ifdef DEBUG		
		INVALID_PATH;
#endif
		printf("String conversion failed, buffer too small\n");	
	}	
	return(result);	
}

extern inline bool win32_Str16To8(WCHAR *src,
			          char *dest,
			          enum string_size_max max)
{
	bool result = false;
	
	int32_t requiredSize = WideCharToMultiByte(CP_ACP, 
		WC_NO_BEST_FIT_CHARS, src, -1, dest, 0, 0, 0);
	if(requiredSize < max)	
	{
		WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, src, -1, dest, max, 0, 0);
		result = true;	
	}	
	else
	{
#ifdef DEBUG		
		INVALID_PATH;
#endif
		printf("String conversion failed, buffer too small\n");	
	}
	return(result);	
}
#endif