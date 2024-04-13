int sldns_str2wire_apl_buf(const char* str, uint8_t* rd, size_t* len)
{
	const char *my_str = str;

	char my_ip_str[64];
	size_t ip_str_len;

	uint16_t family;
	int negation;
	size_t adflength = 0;
	uint8_t data[16+4];
	uint8_t prefix;
	size_t i;

	if(*my_str == '\0') {
		/* empty APL element, no data, no string */
		*len = 0;
		return LDNS_WIREPARSE_ERR_OK;
	}

	/* [!]afi:address/prefix */
	if (strlen(my_str) < 2
			|| strchr(my_str, ':') == NULL
			|| strchr(my_str, '/') == NULL
			|| strchr(my_str, ':') > strchr(my_str, '/')) {
		return LDNS_WIREPARSE_ERR_INVALID_STR;
	}

	if (my_str[0] == '!') {
		negation = 1;
		my_str += 1;
	} else {
		negation = 0;
	}

	family = (uint16_t) atoi(my_str);

	my_str = strchr(my_str, ':') + 1;

	/* need ip addr and only ip addr for inet_pton */
	ip_str_len = (size_t) (strchr(my_str, '/') - my_str);
	if(ip_str_len+1 > sizeof(my_ip_str))
		return LDNS_WIREPARSE_ERR_INVALID_STR;
	(void)strlcpy(my_ip_str, my_str, sizeof(my_ip_str));
	my_ip_str[ip_str_len] = 0;

	if (family == 1) {
		/* ipv4 */
		if(inet_pton(AF_INET, my_ip_str, data+4) == 0)
			return LDNS_WIREPARSE_ERR_INVALID_STR;
		for (i = 0; i < 4; i++) {
			if (data[i+4] != 0) {
				adflength = i + 1;
			}
		}
	} else if (family == 2) {
		/* ipv6 */
		if (inet_pton(AF_INET6, my_ip_str, data+4) == 0)
			return LDNS_WIREPARSE_ERR_INVALID_STR;
		for (i = 0; i < 16; i++) {
			if (data[i+4] != 0) {
				adflength = i + 1;
			}
		}
	} else {
		/* unknown family */
		return LDNS_WIREPARSE_ERR_INVALID_STR;
	}

	my_str = strchr(my_str, '/') + 1;
	prefix = (uint8_t) atoi(my_str);

	sldns_write_uint16(data, family);
	data[2] = prefix;
	data[3] = (uint8_t)adflength;
	if (negation) {
		/* set bit 1 of byte 3 */
		data[3] = data[3] | 0x80;
	}

	if(*len < 4+adflength)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	memmove(rd, data, 4+adflength);
	*len = 4+adflength;
	return LDNS_WIREPARSE_ERR_OK;
}