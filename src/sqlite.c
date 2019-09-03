static int db_GetAllRssCallback(void *data,
				int argc,
			        char **argv,
			        char **azColname)
{
	struct rss_list_content *list =
		(struct rss_list_content *)data;		
	int32_t i = list->rssCount++;
	
	char *endPtr;
	list->rssFiles[i].id = strtoimax(argv[0], &endPtr, 10);	
	strcpy(list->rssFiles[i].title, argv[1]); 	
	strcpy(list->rssFiles[i].url, argv[2]); 	

	return(0);	
}

static int db_GetSingleRssCallback(void *data,
				   int argc,
			           char **argv,
			           char **azColname)
{
	struct rss_content *rss = (struct rss_content *)data;	

	char *endPtr;
	rss->id = strtoimax(argv[0], &endPtr, 10);		
	strcpy(rss->title, argv[1]); 	
	strcpy(rss->url, argv[2]); 	

	return(0);	
}

extern bool db_GetAllRss(struct rss_list_content *list)
{
	sqlite3 *dbHandle = 0;	
	sqlite3_open("data/podded_db.db", &dbHandle);	
	if(!dbHandle) {printf("No DB handle\n");return(false);}
	
	const char *sqlString = {
		"SELECT * FROM rss;" 
	};
	
	char *errMsg = NULL;
	bool tableExists = true;
	if(sqlite3_exec(dbHandle, sqlString, db_GetAllRssCallback, 
	   list, &errMsg) != SQLITE_OK) {
		tableExists = false;
		printf("Database error: %s\n", errMsg);
	}
	
	sqlite3_close(dbHandle);
	return(tableExists);		
}

extern bool db_Init(void)
{
	sqlite3 *dbHandle = 0;	
	sqlite3_open("data/podded_db.db", &dbHandle);	
	if(!dbHandle) {printf("No DB handle\n");return(false);}
	
	const char *sqlString = {
		"CREATE TABLE IF NOT EXISTS rss(\
		ID INTEGER PRIMARY KEY, \
		TITLE TEXT NOT NULL, \
		URL TEXT NOT NULL \
		);"	
	};
	
	char *errMsg = NULL;
	bool tableExists = true;
	if(sqlite3_exec(dbHandle, sqlString, 0, 0, &errMsg) != SQLITE_OK) {
		tableExists = false;
		printf("Database error: %s\n", errMsg);
	}
	
	sqlite3_close(dbHandle);
	return(tableExists);		
}

extern bool db_DeleteRssLink(int32_t id)
{
	bool result = false;
	
	sqlite3 *dbHandle = 0;	
	sqlite3_open("data/podded_db.db", &dbHandle);	
	if(!dbHandle) {printf("No DB handle\n");return(false);}
	
	const char *sqlFormatString = {
		"DELETE FROM rss WHERE ID = %d"
	};
	char sqlString[strlen(sqlFormatString) + 8];
	sprintf(sqlString, sqlFormatString, id);
	
	char *err;
	if(sqlite3_exec(dbHandle, sqlString, 0, 0, &err) != SQLITE_OK ) {			
		printf("Sqlite3 error: %s\n", err);
	} else {result = true;}	
	
	sqlite3_close(dbHandle);
	return(result);	
}

extern bool db_GetRssById(int32_t id,
			  struct rss_content *content)
{
	bool result = false;
	
	sqlite3 *dbHandle = 0;	
	sqlite3_open("data/podded_db.db", &dbHandle);	
	if(!dbHandle) {printf("No DB handle\n");return(false);}
	
	const char *sqlFormatString = {
		"SELECT * FROM rss WHERE ID = %d"
	};
	char sqlString[strlen(sqlFormatString) + 8];
	sprintf(sqlString, sqlFormatString, id);
	
	char *err;
	if(sqlite3_exec(dbHandle, sqlString, db_GetSingleRssCallback, 
	   content, &err) != SQLITE_OK ) 
	{			
		printf("Sqlite3 error: %s\n", err);		
	} 
	else {result = true;}	
	
	sqlite3_close(dbHandle);
	return(result);		
}

extern bool db_AddRssLink(char *title,
			  char *url)
{
	bool result = false;
	
	sqlite3 *dbHandle = 0;	
	sqlite3_open("data/podded_db.db", &dbHandle);	
	if(!dbHandle) {printf("No DB handle\n");return(false);}
	
	const char *sqlFormatString = {
		"INSERT INTO rss (TITLE, URL) \
		VALUES ('%s', '%s');"
	};
	
	char sqlString[strlen(sqlFormatString) + (512 * 2)];
	sprintf(sqlString, sqlFormatString, title, url);
	
	char *errMsg = NULL;
	if(sqlite3_exec(dbHandle, sqlString, 0, 0, &errMsg) != SQLITE_OK) {
		result = false;
		printf("Database error: %s\n", errMsg);
	}
	
	sqlite3_close(dbHandle);
	return(result);		
}