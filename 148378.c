int sldns_str2wire_cert_alg_buf(const char* str, uint8_t* rd, size_t* len)
{
	sldns_lookup_table *lt = sldns_lookup_by_name(sldns_cert_algorithms,
		str);
	if(*len < 2)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	if(lt) {
		sldns_write_uint16(rd, (uint16_t)lt->id);
	} else {
		int s = sldns_str2wire_int16_buf(str, rd, len);
		if(s) return s;
		if(sldns_read_uint16(rd) == 0)
			return LDNS_WIREPARSE_ERR_CERT_BAD_ALGORITHM;
	}
	*len = 2;
	return LDNS_WIREPARSE_ERR_OK;
}