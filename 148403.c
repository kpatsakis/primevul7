int sldns_str2wire_rr_buf(const char* str, uint8_t* rr, size_t* len,
	size_t* dname_len, uint32_t default_ttl, uint8_t* origin,
	size_t origin_len, uint8_t* prev, size_t prev_len)
{
	return sldns_str2wire_rr_buf_internal(str, rr, len, dname_len,
		default_ttl, origin, origin_len, prev, prev_len, 0);
}