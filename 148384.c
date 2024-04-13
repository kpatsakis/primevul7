rrinternal_write_typeclassttl(sldns_buffer* strbuf, uint8_t* rr, size_t len,
	size_t dname_len, uint16_t tp, uint16_t cl, uint32_t ttl, int question)
{
	if(question) {
		/* question is : name, type, class */
		if(dname_len + 4 > len)
			return RET_ERR(LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL,
				sldns_buffer_position(strbuf));
		sldns_write_uint16(rr+dname_len, tp);
		sldns_write_uint16(rr+dname_len+2, cl);
		return LDNS_WIREPARSE_ERR_OK;
	}

	/* type(2), class(2), ttl(4), rdatalen(2 (later)) = 10 */
	if(dname_len + 10 > len)
		return RET_ERR(LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL,
			sldns_buffer_position(strbuf));
	sldns_write_uint16(rr+dname_len, tp);
	sldns_write_uint16(rr+dname_len+2, cl);
	sldns_write_uint32(rr+dname_len+4, ttl);
	sldns_write_uint16(rr+dname_len+8, 0); /* rdatalen placeholder */
	return LDNS_WIREPARSE_ERR_OK;
}