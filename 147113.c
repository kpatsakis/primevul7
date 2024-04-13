HTTPrequestURI(ParsedURL *pu, HRequest *hr)
{
    Str tmp = Strnew();
    if (hr->command == HR_COMMAND_CONNECT) {
	Strcat_charp(tmp, pu->host);
	Strcat(tmp, Sprintf(":%d", pu->port));
    }
    else if (hr->flag & HR_FLAG_LOCAL) {
	Strcat_charp(tmp, pu->file);
	if (pu->query) {
	    Strcat_char(tmp, '?');
	    Strcat_charp(tmp, pu->query);
	}
    }
    else {
	char *save_label = pu->label;
	pu->label = NULL;
	Strcat(tmp, _parsedURL2Str(pu, TRUE));
	pu->label = save_label;
    }
    return tmp;
}