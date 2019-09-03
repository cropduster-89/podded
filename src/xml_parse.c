enum {
	XML_CONTENT,
	XML_ATTRIBUTE,
};

static void xml_CopyStr(xmlNode *node,
			char *dest,
			int32_t type)
{
	char *src = 0;
	if(type == XML_CONTENT) {
		
		src = (char *)xmlNodeGetContent(node);
	} 
	else 
	{
		src = (char *)xmlGetProp(node, (xmlChar *)"url");
	}	
	strcpy(dest, src);
	xmlFree(src);
}
/*
*	
*
*/
static void xml_GetElements(xmlNode *node,
			    bool isItem,
			    struct podcast_content *content)
{
	for(xmlNode *current = node; current; current = current->next) {
		
		if(current->type != XML_ELEMENT_NODE) {goto escape;}
		
		if(isItem && !strcmp((char *)current->name, "item")) 
		{	
			content->episodeCount++;
		} 
		else if(!isItem && !strcmp((char *)current->name, "item")) 
		{
			isItem = true;
		} 
		
		if(!isItem && !strcmp((char *)current->name, "title")) 
		{
			xml_CopyStr(current, content->title, XML_CONTENT);			
		} 
		else if(isItem && !strcmp((char *)current->name, "title")) 
		{			
			xml_CopyStr(current, content->episodes
			[content->episodeCount].title, XML_CONTENT);			
		} 
		
		if(isItem && !strcmp((char *)current->name, "link")) 
		{
			xml_CopyStr(current, content->episodes
			[content->episodeCount].browserLink, XML_CONTENT);
		} 
		else if(isItem && !strcmp((char *)current->name, "enclosure")) 
		{
			xml_CopyStr(current, content->episodes
			[content->episodeCount].downloadLink, XML_ATTRIBUTE);
		} 
		else if(isItem && !strcmp((char *)current->name, "guid")) 	
		{
			xml_CopyStr(current, content->episodes
			[content->episodeCount].browserLink, XML_CONTENT);
		} 
		else if(isItem && !strcmp((char *)current->name, "pubDate")) 	
		{
			xml_CopyStr(current, content->episodes
			[content->episodeCount].publishDate, XML_CONTENT);
		} 			
escape:		xml_GetElements(current->children, isItem, content);	
	}
}

extern bool xml_ParseDocument(char *fileName,
			      struct podcast_content *content)
{
	xmlDoc *doc = xmlReadFile(fileName, NULL, 0);
	xmlNode *root = xmlDocGetRootElement(doc);
	
	xml_GetElements(root, false, content);
	
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return(true);
}