int sldns_str2wire_ilnp64_buf(const char* str, uint8_t* rd, size_t* len)
{
	unsigned int a, b, c, d;
	uint16_t shorts[4];
	int l;
	if(*len < sizeof(shorts))
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;

	if (sscanf(str, "%4x:%4x:%4x:%4x%n", &a, &b, &c, &d, &l) != 4 ||
			l != (int)strlen(str) || /* more data to read */
			strpbrk(str, "+-")       /* signed hexes */
			)
		return LDNS_WIREPARSE_ERR_SYNTAX_ILNP64;
	shorts[0] = htons(a);
	shorts[1] = htons(b);
	shorts[2] = htons(c);
	shorts[3] = htons(d);
	memmove(rd, &shorts, sizeof(shorts));
	*len = sizeof(shorts);
	return LDNS_WIREPARSE_ERR_OK;
}