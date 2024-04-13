int sldns_str2wire_tag_buf(const char* str, uint8_t* rd, size_t* len)
{
	size_t slen = strlen(str);
	const char* ptr;

	if (slen > 255)
		return LDNS_WIREPARSE_ERR_SYNTAX_TAG;
	if(*len < slen+1)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	for (ptr = str; *ptr; ptr++) {
		if(!isalnum((unsigned char)*ptr))
			return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_TAG, ptr-str);
	}
	rd[0] = (uint8_t)slen;
	memmove(rd+1, str, slen);
	*len = slen+1;
	return LDNS_WIREPARSE_ERR_OK;
}