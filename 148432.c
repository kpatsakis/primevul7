int sldns_str2wire_dname_buf(const char* str, uint8_t* buf, size_t* len)
{
	return sldns_str2wire_dname_buf_rel(str, buf, len, NULL);
}