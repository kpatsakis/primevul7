static int _php_filter_validate_ipv6(char *str, size_t str_len) /* {{{ */
{
	int compressed = 0;
	int blocks = 0;
	int n;
	char *ipv4;
	char *end;
	int ip4elm[4];
	char *s = str;

	if (!memchr(str, ':', str_len)) {
		return 0;
	}

	/* check for bundled IPv4 */
	ipv4 = memchr(str, '.', str_len);
	if (ipv4) {
 		while (ipv4 > str && *(ipv4-1) != ':') {
			ipv4--;
		}

		if (!_php_filter_validate_ipv4(ipv4, (str_len - (ipv4 - str)), ip4elm)) {
			return 0;
		}

		str_len = ipv4 - str; /* length excluding ipv4 */
		if (str_len < 2) {
			return 0;
		}

		if (ipv4[-2] != ':') {
			/* don't include : before ipv4 unless it's a :: */
			str_len--;
		}

		blocks = 2;
	}

	end = str + str_len;

	while (str < end) {
		if (*str == ':') {
			if (++str >= end) {
				/* cannot end in : without previous : */
				return 0;
			}
			if (*str == ':') {
				if (compressed) {
					return 0;
				}
				blocks++; /* :: means 1 or more 16-bit 0 blocks */
				compressed = 1;

				if (++str == end) {
					return (blocks <= 8);
				}
			} else if ((str - 1) == s) {
				/* don't allow leading : without another : following */
				return 0;
			}
		}
		n = 0;
		while ((str < end) &&
		       ((*str >= '0' && *str <= '9') ||
		        (*str >= 'a' && *str <= 'f') ||
		        (*str >= 'A' && *str <= 'F'))) {
			n++;
			str++;
		}
		if (n < 1 || n > 4) {
			return 0;
		}
		if (++blocks > 8)
			return 0;
	}
	return ((compressed && blocks <= 8) || blocks == 8);
}