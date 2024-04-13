moffset(struct magic_set *ms, struct magic *m, const struct buffer *b,
    int32_t *op)
{
	size_t nbytes = b->flen;
	int32_t o;

  	switch (m->type) {
  	case FILE_BYTE:
		o = CAST(int32_t, (ms->offset + sizeof(char)));
		break;

  	case FILE_SHORT:
  	case FILE_BESHORT:
  	case FILE_LESHORT:
		o = CAST(int32_t, (ms->offset + sizeof(short)));
		break;

  	case FILE_LONG:
  	case FILE_BELONG:
  	case FILE_LELONG:
  	case FILE_MELONG:
		o = CAST(int32_t, (ms->offset + sizeof(int32_t)));
		break;

  	case FILE_QUAD:
  	case FILE_BEQUAD:
  	case FILE_LEQUAD:
		o = CAST(int32_t, (ms->offset + sizeof(int64_t)));
		break;

  	case FILE_STRING:
  	case FILE_PSTRING:
  	case FILE_BESTRING16:
  	case FILE_LESTRING16:
		if (m->reln == '=' || m->reln == '!') {
			o = ms->offset + m->vallen;
		} else {
			union VALUETYPE *p = &ms->ms_value;

			if (*m->value.s == '\0')
				p->s[strcspn(p->s, "\r\n")] = '\0';
			o = CAST(uint32_t, (ms->offset + strlen(p->s)));
			if (m->type == FILE_PSTRING)
				o += (uint32_t)file_pstring_length_size(m);
		}
		break;

	case FILE_DATE:
	case FILE_BEDATE:
	case FILE_LEDATE:
	case FILE_MEDATE:
		o = CAST(int32_t, (ms->offset + sizeof(uint32_t)));
		break;

	case FILE_LDATE:
	case FILE_BELDATE:
	case FILE_LELDATE:
	case FILE_MELDATE:
		o = CAST(int32_t, (ms->offset + sizeof(uint32_t)));
		break;

	case FILE_QDATE:
	case FILE_BEQDATE:
	case FILE_LEQDATE:
		o = CAST(int32_t, (ms->offset + sizeof(uint64_t)));
		break;

	case FILE_QLDATE:
	case FILE_BEQLDATE:
	case FILE_LEQLDATE:
		o = CAST(int32_t, (ms->offset + sizeof(uint64_t)));
		break;

  	case FILE_FLOAT:
  	case FILE_BEFLOAT:
  	case FILE_LEFLOAT:
		o = CAST(int32_t, (ms->offset + sizeof(float)));
		break;

  	case FILE_DOUBLE:
  	case FILE_BEDOUBLE:
  	case FILE_LEDOUBLE:
		o = CAST(int32_t, (ms->offset + sizeof(double)));
		break;

	case FILE_REGEX:
		if ((m->str_flags & REGEX_OFFSET_START) != 0)
			o = CAST(int32_t, ms->search.offset);
		else
			o = CAST(int32_t,
			    (ms->search.offset + ms->search.rm_len));
		break;

	case FILE_SEARCH:
		if ((m->str_flags & REGEX_OFFSET_START) != 0)
			o = CAST(int32_t, ms->search.offset);
		else
			o = CAST(int32_t, (ms->search.offset + m->vallen));
		break;

	case FILE_CLEAR:
	case FILE_DEFAULT:
	case FILE_INDIRECT:
		o = ms->offset;
		break;

	case FILE_DER:
		{
			o = der_offs(ms, m, nbytes);
			if (o == -1 || (size_t)o > nbytes) {
				if ((ms->flags & MAGIC_DEBUG) != 0) {
					(void)fprintf(stderr,
					    "Bad DER offset %d nbytes=%"
					    SIZE_T_FORMAT "u", o, nbytes);
				}
				*op = 0;
				return 0;
			}
			break;
		}

	default:
		o = 0;
		break;
	}

	if ((size_t)o > nbytes) {
#if 0
		file_error(ms, 0, "Offset out of range %" SIZE_T_FORMAT
		    "u > %" SIZE_T_FORMAT "u", (size_t)o, nbytes);
#endif
		return -1;
	}
	*op = o;
	return 1;
}