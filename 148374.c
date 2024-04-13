uint8_t* sldns_wirerr_get_rdatawl(uint8_t* rr, size_t len, size_t dname_len)
{
	if(len < dname_len+10)
		return NULL;
	return rr+dname_len+8;
}