void php_filter_validate_ip(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	/* validates an ipv4 or ipv6 IP, based on the flag (4, 6, or both) add a
	 * flag to throw out reserved ranges; multicast ranges... etc. If both
	 * allow_ipv4 and allow_ipv6 flags flag are used, then the first dot or
	 * colon determine the format */

	int            ip[4];
	int            mode;

	if (memchr(Z_STRVAL_P(value), ':', Z_STRLEN_P(value))) {
		mode = FORMAT_IPV6;
	} else if (memchr(Z_STRVAL_P(value), '.', Z_STRLEN_P(value))) {
		mode = FORMAT_IPV4;
	} else {
		RETURN_VALIDATION_FAILED
	}

	if ((flags & FILTER_FLAG_IPV4) && (flags & FILTER_FLAG_IPV6)) {
		/* Both formats are cool */
	} else if ((flags & FILTER_FLAG_IPV4) && mode == FORMAT_IPV6) {
		RETURN_VALIDATION_FAILED
	} else if ((flags & FILTER_FLAG_IPV6) && mode == FORMAT_IPV4) {
		RETURN_VALIDATION_FAILED
	}

	switch (mode) {
		case FORMAT_IPV4:
			if (!_php_filter_validate_ipv4(Z_STRVAL_P(value), Z_STRLEN_P(value), ip)) {
				RETURN_VALIDATION_FAILED
			}

			/* Check flags */
			if (flags & FILTER_FLAG_NO_PRIV_RANGE) {
				if (
					(ip[0] == 10) ||
					(ip[0] == 172 && ip[1] >= 16 && ip[1] <= 31) ||
					(ip[0] == 192 && ip[1] == 168)
				) {
					RETURN_VALIDATION_FAILED
				}
			}

			if (flags & FILTER_FLAG_NO_RES_RANGE) {
				if (
					(ip[0] == 0) ||
					(ip[0] >= 240) ||
					(ip[0] == 127) ||
					(ip[0] == 169 && ip[1] == 254)
				) {
					RETURN_VALIDATION_FAILED
				}
			}
			break;

		case FORMAT_IPV6:
			{
				int res = 0;
				res = _php_filter_validate_ipv6(Z_STRVAL_P(value), Z_STRLEN_P(value));
				if (res < 1) {
					RETURN_VALIDATION_FAILED
				}
				/* Check flags */
				if (flags & FILTER_FLAG_NO_PRIV_RANGE) {
					if (Z_STRLEN_P(value) >=2 && (!strncasecmp("FC", Z_STRVAL_P(value), 2) || !strncasecmp("FD", Z_STRVAL_P(value), 2))) {
						RETURN_VALIDATION_FAILED
					}
				}
				if (flags & FILTER_FLAG_NO_RES_RANGE) {
					switch (Z_STRLEN_P(value)) {
						case 1: case 0:
							break;
						case 2:
							if (!strcmp("::", Z_STRVAL_P(value))) {
								RETURN_VALIDATION_FAILED
							}
							break;
						case 3:
							if (!strcmp("::1", Z_STRVAL_P(value)) || !strcmp("5f:", Z_STRVAL_P(value))) {
								RETURN_VALIDATION_FAILED
							}
							break;
						default:
							if (Z_STRLEN_P(value) >= 5) {
								if (
									!strncasecmp("fe8", Z_STRVAL_P(value), 3) ||
									!strncasecmp("fe9", Z_STRVAL_P(value), 3) ||
									!strncasecmp("fea", Z_STRVAL_P(value), 3) ||
									!strncasecmp("feb", Z_STRVAL_P(value), 3)
								) {
									RETURN_VALIDATION_FAILED
								}
							}
							if (
								(Z_STRLEN_P(value) >= 9 &&  !strncasecmp("2001:0db8", Z_STRVAL_P(value), 9)) ||
								(Z_STRLEN_P(value) >= 2 &&  !strncasecmp("5f", Z_STRVAL_P(value), 2)) ||
								(Z_STRLEN_P(value) >= 4 &&  !strncasecmp("3ff3", Z_STRVAL_P(value), 4)) ||
								(Z_STRLEN_P(value) >= 8 &&  !strncasecmp("2001:001", Z_STRVAL_P(value), 8))
							) {
								RETURN_VALIDATION_FAILED
							}
					}
				}
			}
			break;
	}
}