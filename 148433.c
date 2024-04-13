int sldns_str2wire_a_buf(const char* str, uint8_t* rd, size_t* len)
{
	struct in_addr address;
	if(inet_pton(AF_INET, (char*)str, &address) != 1)
		return LDNS_WIREPARSE_ERR_SYNTAX_IP4;
	if(*len < sizeof(address))
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	memmove(rd, &address, sizeof(address));
	*len = sizeof(address);
	return LDNS_WIREPARSE_ERR_OK;
}