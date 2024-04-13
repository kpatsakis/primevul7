check_time_syntax (struct berval *val,
	int start,
	int *parts,
	struct berval *fraction)
{
	/*
	 * start=0 GeneralizedTime YYYYmmddHH[MM[SS]][(./,)d...](Z|(+/-)HH[MM])
	 * start=1 UTCTime         YYmmddHHMM[SS][Z|(+/-)HHMM]
	 * GeneralizedTime supports leap seconds, UTCTime does not.
	 */
	static const int ceiling[9] = { 100, 100, 12, 31, 24, 60, 60, 24, 60 };
	static const int mdays[2][12] = {
		/* non-leap years */
		{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
		/* leap years */
		{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
	};
	char *p, *e;
	int part, c, c1, c2, tzoffset, leapyear = 0;

	p = val->bv_val;
	e = p + val->bv_len;

#ifdef SUPPORT_OBSOLETE_UTC_SYNTAX
	parts[0] = 20; /* century - any multiple of 4 from 04 to 96 */
#endif
	for (part = start; part < 7 && p < e; part++) {
		c1 = *p;
		if (!ASCII_DIGIT(c1)) {
			break;
		}
		p++;
		if (p == e) {
			return LDAP_INVALID_SYNTAX;
		}
		c = *p++;
		if (!ASCII_DIGIT(c)) {
			return LDAP_INVALID_SYNTAX;
		}
		c += c1 * 10 - '0' * 11;
		if ((part | 1) == 3) {
			--c;
			if (c < 0) {
				return LDAP_INVALID_SYNTAX;
			}
		}
		if (c >= ceiling[part]) {
			if (! (c == 60 && part == 6 && start == 0))
				return LDAP_INVALID_SYNTAX;
		}
		parts[part] = c;
	}
	if (part < 5 + start) {
		return LDAP_INVALID_SYNTAX;
	}
	for (; part < 9; part++) {
		parts[part] = 0;
	}

	/* leapyear check for the Gregorian calendar (year>1581) */
	if (parts[parts[1] == 0 ? 0 : 1] % 4 == 0) {
		leapyear = 1;
	}

	if (parts[3] >= mdays[leapyear][parts[2]]) {
		return LDAP_INVALID_SYNTAX;
	}

	if (start == 0) {
		fraction->bv_val = p;
		fraction->bv_len = 0;
		if (p < e && (*p == '.' || *p == ',')) {
			char *end_num;
			while (++p < e && ASCII_DIGIT(*p)) {
				/* EMTPY */;
			}
			if (p - fraction->bv_val == 1) {
				return LDAP_INVALID_SYNTAX;
			}
			for (end_num = p; end_num[-1] == '0'; --end_num) {
				/* EMPTY */;
			}
			c = end_num - fraction->bv_val;
			if (c != 1) fraction->bv_len = c;
		}
	}

	if (p == e) {
		/* no time zone */
		return start == 0 ? LDAP_INVALID_SYNTAX : LDAP_SUCCESS;
	}

	tzoffset = *p++;
	switch (tzoffset) {
	default:
		return LDAP_INVALID_SYNTAX;
	case 'Z':
		/* UTC */
		break;
	case '+':
	case '-':
		for (part = 7; part < 9 && p < e; part++) {
			c1 = *p;
			if (!ASCII_DIGIT(c1)) {
				break;
			}
			p++;
			if (p == e) {
				return LDAP_INVALID_SYNTAX;
			}
			c2 = *p++;
			if (!ASCII_DIGIT(c2)) {
				return LDAP_INVALID_SYNTAX;
			}
			parts[part] = c1 * 10 + c2 - '0' * 11;
			if (parts[part] >= ceiling[part]) {
				return LDAP_INVALID_SYNTAX;
			}
		}
		if (part < 8 + start) {
			return LDAP_INVALID_SYNTAX;
		}

		if (tzoffset == '-') {
			/* negative offset to UTC, ie west of Greenwich */
			parts[4] += parts[7];
			parts[5] += parts[8];
			/* offset is just hhmm, no seconds */
			for (part = 6; --part >= 0; ) {
				if (part != 3) {
					c = ceiling[part];
				} else {
					c = mdays[leapyear][parts[2]];
				}
				if (parts[part] >= c) {
					if (part == 0) {
						return LDAP_INVALID_SYNTAX;
					}
					parts[part] -= c;
					parts[part - 1]++;
					continue;
				} else if (part != 5) {
					break;
				}
			}
		} else {
			/* positive offset to UTC, ie east of Greenwich */
			parts[4] -= parts[7];
			parts[5] -= parts[8];
			for (part = 6; --part >= 0; ) {
				if (parts[part] < 0) {
					if (part == 0) {
						return LDAP_INVALID_SYNTAX;
					}
					if (part != 3) {
						c = ceiling[part];
					} else {
						/* make first arg to % non-negative */
						c = mdays[leapyear][(parts[2] - 1 + 12) % 12];
					}
					parts[part] += c;
					parts[part - 1]--;
					continue;
				} else if (part != 5) {
					break;
				}
			}
		}
	}

	return p != e ? LDAP_INVALID_SYNTAX : LDAP_SUCCESS;
}