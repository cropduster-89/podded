extern void curl_Init(void)
{
	curl_global_init(CURL_GLOBAL_ALL); 
}

#define MAX_TRANSFERS 4

/*
*	Callback function to write data.
*
* 	libcurl docs says win32 programs will crash 
*	if using default callback, but it worked fine for me 
*	without this.   
*/
static size_t curl_WriteCallback(char *ptr, 
			         size_t size, 
			         size_t nmemb, 
			         void *userdata)
{
	FILE *fh = (FILE *)userdata;
	size_t result = fwrite(ptr, size, nmemb, fh);
	
	return(result);
}
/*
*	Init a new tranfer to the multi download batch
*/
static void curl_AddTransfer(struct download_batch *batch)
{
	struct download_task *task = 
		&batch->downloads[batch->transfersStarted];	
	FILE *fh = 0;
	errno = fopen_s(&fh, task->path, "wb");		
	
	CURL *curlE = curl_easy_init();
	curl_easy_setopt(curlE, CURLOPT_URL, task->url);
	curl_easy_setopt(curlE, CURLOPT_WRITEDATA, fh);
	curl_easy_setopt(curlE, CURLOPT_PRIVATE, fh);
	curl_easy_setopt(curlE, CURLOPT_WRITEFUNCTION, curl_WriteCallback);
	curl_easy_setopt(curlE, CURLOPT_FOLLOWLOCATION , true);	
	curl_easy_setopt(curlE, CURLOPT_CAINFO, "curl-ca-bundle.crt");
	curl_multi_add_handle(batch->curlM, curlE);
	
	++batch->transfersActive;
	++batch->transfersStarted;		
}
/*
*	Non blocking download function, allows multiple downloads
*	simultaniously on a single thread.
* 
*	NOT NON BLOCKING ENOUGH! maybe a problem with peekmessage,
*	or perhaps best moved to multithreading
*
*	TODO handle errors and timeouts
*/
extern bool curl_MultiDownoadUpdate(struct download_batch *batch)
{	
	curl_multi_perform(batch->curlM, &batch->transfersActive);	
	
	int32_t msgCount = -1;
	CURLMsg *msg = 0; ;
	while((msg = curl_multi_info_read(batch->curlM, &msgCount))) 
	{
		if(msg->msg == CURLMSG_DONE)
		{				
			CURL *curlE = msg->easy_handle;
			char *fhPtr = 0;
			curl_easy_getinfo(curlE, CURLINFO_PRIVATE, &fhPtr);
			FILE *fh = (FILE *)fhPtr;
			fclose(fh);			
			curl_multi_remove_handle(batch->curlM, curlE);
			curl_easy_cleanup(curlE);
			++batch->transfersComplete;
			--batch->transfersActive;			
		}	
		if(batch->transfersActive <= MAX_TRANSFERS && 
		   batch->transfersStarted < batch->transfersCount)
		{			
			curl_AddTransfer(batch);
		}
	}
	if(batch->transfersComplete == batch->transfersCount) {		
		curl_multi_cleanup(batch->curlM);
		return(true);} 
	else {return(false);}
}  
/*
*	Start multi-download batch.
*/
extern void curl_InitMultiDownload(struct download_batch *batch)
{
	batch->curlM = curl_multi_init();
	curl_multi_setopt(batch->curlM, CURLOPT_MAXCONNECTS, MAX_TRANSFERS);
	
	int32_t transfers = (batch->transfersCount < MAX_TRANSFERS) 
		? batch->transfersCount : MAX_TRANSFERS;
	
	for(int32_t i = 0; i < transfers; ++i)
	{
		curl_AddTransfer(batch);
	}	
}

extern bool curl_GetRssFromUrl(char *url, char *outFile)
{
	bool result = false;
		
	CURL *curl = curl_easy_init();
	if(!curl) {printf("!curl_easy_init\n"); return(result);}
	
	FILE *fh = 0;
	errno = fopen_s(&fh, outFile, "wb");
	if(errno) {printf("!fopen_s\n"); fclose(fh); return(result);}
	
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fh);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION , true);	
	curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
	CURLcode cResult = curl_easy_perform(curl);
	if(cResult) {printf("!curl_easy_perform\n");}
	else {result = true;}
	
	fclose(fh);
	curl_easy_cleanup(curl);	
	return(result);
}