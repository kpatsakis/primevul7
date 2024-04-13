getarg(
	const char *str,
	int code,
	arg_v *argp
	)
{
	u_long ul;

	switch (code & ~OPT) {
	case NTP_STR:
		argp->string = str;
		break;

	case NTP_ADD:
		if (!getnetnum(str, &argp->netnum, NULL, 0))
			return 0;
		break;

	case NTP_UINT:
		if ('&' == str[0]) {
			if (!atouint(&str[1], &ul)) {
				fprintf(stderr,
					"***Association index `%s' invalid/undecodable\n",
					str);
				return 0;
			}
			if (0 == numassoc) {
				dogetassoc(stdout);
				if (0 == numassoc) {
					fprintf(stderr,
						"***No associations found, `%s' unknown\n",
						str);
					return 0;
				}
			}
			ul = min(ul, numassoc);
			argp->uval = assoc_cache[ul - 1].assid;
			break;
		}
		if (!atouint(str, &argp->uval)) {
			fprintf(stderr, "***Illegal unsigned value %s\n",
				str);
			return 0;
		}
		break;

	case NTP_INT:
		if (!atoint(str, &argp->ival)) {
			fprintf(stderr, "***Illegal integer value %s\n",
				str);
			return 0;
		}
		break;

	case IP_VERSION:
		if (!strcmp("-6", str)) {
			argp->ival = 6;
		} else if (!strcmp("-4", str)) {
			argp->ival = 4;
		} else {
			fprintf(stderr, "***Version must be either 4 or 6\n");
			return 0;
		}
		break;
	}

	return 1;
}