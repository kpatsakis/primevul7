 */
private int
mconvert(struct magic_set *ms, struct magic *m, int flip)
{
	union VALUETYPE *p = &ms->ms_value;

	switch (cvt_flip(m->type, flip)) {
	case FILE_BYTE:
		if (cvt_8(p, m) == -1)
			goto out;
		return 1;
	case FILE_SHORT:
		if (cvt_16(p, m) == -1)
			goto out;
		return 1;
	case FILE_LONG:
	case FILE_DATE:
	case FILE_LDATE:
		if (cvt_32(p, m) == -1)
			goto out;
		return 1;
	case FILE_QUAD:
	case FILE_QDATE:
	case FILE_QLDATE:
	case FILE_QWDATE:
		if (cvt_64(p, m) == -1)
			goto out;
		return 1;
	case FILE_STRING:
	case FILE_BESTRING16:
	case FILE_LESTRING16: {
		/* Null terminate and eat *trailing* return */
		p->s[sizeof(p->s) - 1] = '\0';
		return 1;
	}
	case FILE_PSTRING: {
		size_t sz = file_pstring_length_size(m);
		char *ptr1 = p->s, *ptr2 = ptr1 + sz;
		size_t len = file_pstring_get_length(m, ptr1);
		sz = sizeof(p->s) - sz; /* maximum length of string */
		if (len >= sz) {
			/*
			 * The size of the pascal string length (sz)
			 * is 1, 2, or 4. We need at least 1 byte for NUL
			 * termination, but we've already truncated the
			 * string by p->s, so we need to deduct sz.
			 * Because we can use one of the bytes of the length
			 * after we shifted as NUL termination.
			 */
			len = sz;
		}
		while (len--)
			*ptr1++ = *ptr2++;
		*ptr1 = '\0';
		return 1;
	}
	case FILE_BESHORT:
		p->h = (short)BE16(p);
		if (cvt_16(p, m) == -1)
			goto out;
		return 1;
	case FILE_BELONG:
	case FILE_BEDATE:
	case FILE_BELDATE:
		p->l = (int32_t)BE32(p);
		if (cvt_32(p, m) == -1)
			goto out;
		return 1;
	case FILE_BEQUAD:
	case FILE_BEQDATE:
	case FILE_BEQLDATE:
	case FILE_BEQWDATE:
		p->q = (uint64_t)BE64(p);
		if (cvt_64(p, m) == -1)
			goto out;
		return 1;
	case FILE_LESHORT:
		p->h = (short)LE16(p);
		if (cvt_16(p, m) == -1)
			goto out;
		return 1;
	case FILE_LELONG:
	case FILE_LEDATE:
	case FILE_LELDATE:
		p->l = (int32_t)LE32(p);
		if (cvt_32(p, m) == -1)
			goto out;
		return 1;
	case FILE_LEQUAD:
	case FILE_LEQDATE:
	case FILE_LEQLDATE:
	case FILE_LEQWDATE:
		p->q = (uint64_t)LE64(p);
		if (cvt_64(p, m) == -1)
			goto out;
		return 1;
	case FILE_MELONG:
	case FILE_MEDATE:
	case FILE_MELDATE:
		p->l = (int32_t)ME32(p);
		if (cvt_32(p, m) == -1)
			goto out;
		return 1;
	case FILE_FLOAT:
		if (cvt_float(p, m) == -1)
			goto out;
		return 1;
	case FILE_BEFLOAT:
		p->l = BE32(p);
		if (cvt_float(p, m) == -1)
			goto out;
		return 1;
	case FILE_LEFLOAT:
		p->l = LE32(p);
		if (cvt_float(p, m) == -1)
			goto out;
		return 1;
	case FILE_DOUBLE:
		if (cvt_double(p, m) == -1)
			goto out;
		return 1;
	case FILE_BEDOUBLE:
		p->q = BE64(p);
		if (cvt_double(p, m) == -1)
			goto out;
		return 1;
	case FILE_LEDOUBLE:
		p->q = LE64(p);
		if (cvt_double(p, m) == -1)
			goto out;
		return 1;
	case FILE_REGEX:
	case FILE_SEARCH:
	case FILE_DEFAULT:
	case FILE_CLEAR:
	case FILE_NAME:
	case FILE_USE:
	case FILE_DER:
		return 1;
	default:
		file_magerror(ms, "invalid type %d in mconvert()", m->type);
		return 0;
	}
out:
	file_magerror(ms, "zerodivide in mconvert()");