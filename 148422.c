uint16_t sldns_wirerr_get_class(uint8_t* rr, size_t len, size_t dname_len)
{
	if(len < dname_len+4)
		return 0;
	return sldns_read_uint16(rr+dname_len+2);
}