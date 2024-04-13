int sldns_str2wire_eui48_buf(const char* str, uint8_t* rd, size_t* len)
{
	unsigned int a, b, c, d, e, f;
	int l;

	if(*len < 6)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	if (sscanf(str, "%2x-%2x-%2x-%2x-%2x-%2x%n",
			&a, &b, &c, &d, &e, &f, &l) != 6 ||
			l != (int)strlen(str))
		return LDNS_WIREPARSE_ERR_SYNTAX_EUI48;
	rd[0] = a;
	rd[1] = b;
	rd[2] = c;
	rd[3] = d;
	rd[4] = e;
	rd[5] = f;
	*len = 6;
	return LDNS_WIREPARSE_ERR_OK;
}