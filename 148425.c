uint32_t sldns_wirerr_get_ttl(uint8_t* rr, size_t len, size_t dname_len)
{
	if(len < dname_len+8)
		return 0;
	return sldns_read_uint32(rr+dname_len+4);
}