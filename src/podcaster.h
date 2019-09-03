#ifndef PODCASTER_H
#define PODCASTER_H

#define INVALID_PATH assert(0);
#define ARRAY_COUNT(a) (sizeof(a) / sizeof(a[0]))

enum char_type {
	UTF_8,	
	UTF_16,	
};

enum string_size_max {
	MAX_TITLE_LEN	= 256,
	MAX_URL_LEN	= 512,	
	MAX_PATH_LEN	= 1024,	
};

enum state_flags {
	STATE_SEARCHDATA = 0,
	STATE_LIBWINDOW  = 8,	
	STATE_DLWINDOW   = 16,	
	STATE_ACTIVEDL	 = 24,	
};

struct episode_content {
	char title[256];
	char downloadLink[512];
	char browserLink[512];
	char publishDate[128];	
};

struct podcast_content {
	char title[256];
	uint32_t episodeCount;
	struct episode_content episodes[256];
};

struct rss_content {
	int32_t id;
	char title[256];
	char url[512];
};

struct rss_list_content {
	uint32_t rssCount;
	struct rss_content rssFiles[256];
};

struct download_task {
	char path[MAX_PATH_LEN];
	char url[MAX_URL_LEN];		
};

struct download_batch {
	int32_t transfersCount;
	int32_t transfersComplete;
	int32_t transfersStarted;
	int32_t transfersActive;
	struct download_task *downloads;
	CURLM *curlM;	
};

struct program_state {
	char title[256];
	char rssLink[512];	
	uint64_t flags;
	struct download_batch dlBatch;
	
	HBRUSH blueBrush;
	HBRUSH blueBrushL;
	
	HBRUSH greenBrushL;
	HBRUSH greenBrushM;
	HBRUSH greenBrushD;	
	
	HBRUSH whiteBrush;
	
	HBITMAP logoBmp;
	
	HFONT btnFontBig;
	HFONT btnFontSmall;
	
	RECT headerRect;
	RECT sideRect;
};

#endif