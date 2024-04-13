
private int
mget(struct magic_set *ms, struct magic *m, const struct buffer *b,
    const unsigned char *s, size_t nbytes, size_t o, unsigned int cont_level,
    int mode, int text, int flip, uint16_t *indir_count, uint16_t *name_count,
    int *printed_something, int *need_separator, int *returnval,
    int *found_match)
{
	uint32_t offset = ms->offset;
	struct buffer bb;
	intmax_t lhs;
	file_pushbuf_t *pb;
	int rv, oneed_separator, in_type;
	char *rbuf;
	union VALUETYPE *p = &ms->ms_value;
	struct mlist ml;

	if (*indir_count >= ms->indir_max) {
		file_error(ms, 0, "indirect count (%hu) exceeded",
		    *indir_count);
		return -1;
	}

	if (*name_count >= ms->name_max) {
		file_error(ms, 0, "name use count (%hu) exceeded",
		    *name_count);
		return -1;
	}



	if (mcopy(ms, p, m->type, m->flag & INDIR, s, (uint32_t)(offset + o),
	    (uint32_t)nbytes, m) == -1)
		return -1;

	if ((ms->flags & MAGIC_DEBUG) != 0) {
		fprintf(stderr, "mget(type=%d, flag=%#x, offset=%u, o=%"
		    SIZE_T_FORMAT "u, " "nbytes=%" SIZE_T_FORMAT
		    "u, il=%hu, nc=%hu)\n",
		    m->type, m->flag, offset, o, nbytes,
		    *indir_count, *name_count);
		mdebug(offset, (char *)(void *)p, sizeof(union VALUETYPE));
#ifndef COMPILE_ONLY
		file_mdump(m);
#endif
	}

	if (m->flag & INDIR) {
		intmax_t off = m->in_offset;
		const int sgn = m->in_op & FILE_OPSIGNED;
		if (m->in_op & FILE_OPINDIRECT) {
			const union VALUETYPE *q = CAST(const union VALUETYPE *,
			    ((const void *)(s + offset + off)));
			switch (cvt_flip(m->in_type, flip)) {
			case FILE_BYTE:
				if (OFFSET_OOB(nbytes, offset + off, 1))
					return 0;
				off = SEXT(sgn,8,q->b);
				break;
			case FILE_SHORT:
				if (OFFSET_OOB(nbytes, offset + off, 2))
					return 0;
				off = SEXT(sgn,16,q->h);
				break;
			case FILE_BESHORT:
				if (OFFSET_OOB(nbytes, offset + off, 2))
					return 0;
				off = SEXT(sgn,16,BE16(q));
				break;
			case FILE_LESHORT:
				if (OFFSET_OOB(nbytes, offset + off, 2))
					return 0;
				off = SEXT(sgn,16,LE16(q));
				break;
			case FILE_LONG:
				if (OFFSET_OOB(nbytes, offset + off, 4))
					return 0;
				off = SEXT(sgn,32,q->l);
				break;
			case FILE_BELONG:
			case FILE_BEID3:
				if (OFFSET_OOB(nbytes, offset + off, 4))
					return 0;
				off = SEXT(sgn,32,BE32(q));
				break;
			case FILE_LEID3:
			case FILE_LELONG:
				if (OFFSET_OOB(nbytes, offset + off, 4))
					return 0;
				off = SEXT(sgn,32,LE32(q));
				break;
			case FILE_MELONG:
				if (OFFSET_OOB(nbytes, offset + off, 4))
					return 0;
				off = SEXT(sgn,32,ME32(q));
				break;
			case FILE_BEQUAD:
				if (OFFSET_OOB(nbytes, offset + off, 8))
					return 0;
				off = SEXT(sgn,64,BE64(q));
				break;
			case FILE_LEQUAD:
				if (OFFSET_OOB(nbytes, offset + off, 8))
					return 0;
				off = SEXT(sgn,64,LE64(q));
				break;
			default:
				abort();
			}
			if ((ms->flags & MAGIC_DEBUG) != 0)
				fprintf(stderr, "indirect offs=%jd\n", off);
		}
		switch (in_type = cvt_flip(m->in_type, flip)) {
		case FILE_BYTE:
			if (OFFSET_OOB(nbytes, offset, 1))
				return 0;
			offset = do_ops(m, SEXT(sgn,8,p->b), off);
			break;
		case FILE_BESHORT:
			if (OFFSET_OOB(nbytes, offset, 2))
				return 0;
			offset = do_ops(m, SEXT(sgn,16,BE16(p)), off);
			break;
		case FILE_LESHORT:
			if (OFFSET_OOB(nbytes, offset, 2))
				return 0;
			offset = do_ops(m, SEXT(sgn,16,LE16(p)), off);
			break;
		case FILE_SHORT:
			if (OFFSET_OOB(nbytes, offset, 2))
				return 0;
			offset = do_ops(m, SEXT(sgn,16,p->h), off);
			break;
		case FILE_BELONG:
		case FILE_BEID3:
			if (OFFSET_OOB(nbytes, offset, 4))
				return 0;
			lhs = BE32(p);
			if (in_type == FILE_BEID3)
				lhs = cvt_id3(ms, (uint32_t)lhs);
			offset = do_ops(m, SEXT(sgn,32,lhs), off);
			break;
		case FILE_LELONG:
		case FILE_LEID3:
			if (OFFSET_OOB(nbytes, offset, 4))
				return 0;
			lhs = LE32(p);
			if (in_type == FILE_LEID3)
				lhs = cvt_id3(ms, (uint32_t)lhs);
			offset = do_ops(m, SEXT(sgn,32,lhs), off);
			break;
		case FILE_MELONG:
			if (OFFSET_OOB(nbytes, offset, 4))
				return 0;
			offset = do_ops(m, SEXT(sgn,32,ME32(p)), off);
			break;
		case FILE_LONG:
			if (OFFSET_OOB(nbytes, offset, 4))
				return 0;
			offset = do_ops(m, SEXT(sgn,32,p->l), off);
			break;
		case FILE_LEQUAD:
			if (OFFSET_OOB(nbytes, offset, 8))
				return 0;
			offset = do_ops(m, SEXT(sgn,64,LE64(p)), off);
			break;
		case FILE_BEQUAD:
			if (OFFSET_OOB(nbytes, offset, 8))
				return 0;
			offset = do_ops(m, SEXT(sgn,64,BE64(p)), off);
			break;
		default:
			abort();
		}

		if (m->flag & INDIROFFADD) {
			offset += ms->c.li[cont_level-1].off;
			if (offset == 0) {
				if ((ms->flags & MAGIC_DEBUG) != 0)
					fprintf(stderr,
					    "indirect *zero* offset\n");
				return 0;
			}
			if ((ms->flags & MAGIC_DEBUG) != 0)
				fprintf(stderr, "indirect +offs=%u\n", offset);
		}
		if (mcopy(ms, p, m->type, 0, s, offset, nbytes, m) == -1)
			return -1;
		ms->offset = offset;

		if ((ms->flags & MAGIC_DEBUG) != 0) {
			mdebug(offset, (char *)(void *)p,
			    sizeof(union VALUETYPE));
#ifndef COMPILE_ONLY
			file_mdump(m);
#endif
		}
	}

	/* Verify we have enough data to match magic type */
	switch (m->type) {
	case FILE_BYTE:
		if (OFFSET_OOB(nbytes, offset, 1))
			return 0;
		break;

	case FILE_SHORT:
	case FILE_BESHORT:
	case FILE_LESHORT:
		if (OFFSET_OOB(nbytes, offset, 2))
			return 0;
		break;

	case FILE_LONG:
	case FILE_BELONG:
	case FILE_LELONG:
	case FILE_MELONG:
	case FILE_DATE:
	case FILE_BEDATE:
	case FILE_LEDATE:
	case FILE_MEDATE:
	case FILE_LDATE:
	case FILE_BELDATE:
	case FILE_LELDATE:
	case FILE_MELDATE:
	case FILE_FLOAT:
	case FILE_BEFLOAT:
	case FILE_LEFLOAT:
		if (OFFSET_OOB(nbytes, offset, 4))
			return 0;
		break;

	case FILE_DOUBLE:
	case FILE_BEDOUBLE:
	case FILE_LEDOUBLE:
		if (OFFSET_OOB(nbytes, offset, 8))
			return 0;
		break;

	case FILE_STRING:
	case FILE_PSTRING:
	case FILE_SEARCH:
		if (OFFSET_OOB(nbytes, offset, m->vallen))
			return 0;
		break;

	case FILE_REGEX:
		if (nbytes < offset)
			return 0;
		break;

	case FILE_INDIRECT:
		if (m->str_flags & INDIRECT_RELATIVE)
			offset += CAST(uint32_t, o);
		if (offset == 0)
			return 0;

		if (nbytes < offset)
			return 0;

		if ((pb = file_push_buffer(ms)) == NULL)
			return -1;

		(*indir_count)++;
		bb = *b;
		bb.fbuf = s + offset;
		bb.flen = nbytes - offset;
		rv = file_softmagic(ms, &bb,
		    indir_count, name_count, BINTEST, text);

		if ((ms->flags & MAGIC_DEBUG) != 0)
			fprintf(stderr, "indirect @offs=%u[%d]\n", offset, rv);

		rbuf = file_pop_buffer(ms, pb);
		if (rbuf == NULL && ms->event_flags & EVENT_HAD_ERR)
			return -1;

		if (rv == 1) {
			if ((ms->flags & MAGIC_NODESC) == 0 &&
			    file_printf(ms, F(ms, m->desc, "%u"), offset) == -1)
			{
				free(rbuf);
				return -1;
			}
			if (file_printf(ms, "%s", rbuf) == -1) {
				free(rbuf);
				return -1;
			}
		}
		free(rbuf);
		return rv;

	case FILE_USE:
		if (nbytes < offset)
			return 0;
		rbuf = m->value.s;
		if (*rbuf == '^') {
			rbuf++;
			flip = !flip;
		}
		if (file_magicfind(ms, rbuf, &ml) == -1) {
			file_error(ms, 0, "cannot find entry `%s'", rbuf);
			return -1;
		}
		(*name_count)++;
		oneed_separator = *need_separator;
		if (m->flag & NOSPACE)
			*need_separator = 0;
		rv = match(ms, ml.magic, ml.nmagic, b, offset + o,
		    mode, text, flip, indir_count, name_count,
		    printed_something, need_separator, returnval, found_match);
		(*name_count)--;
		if (rv != 1)
		    *need_separator = oneed_separator;
		return rv;

	case FILE_NAME:
		if (ms->flags & MAGIC_NODESC)
			return 1;
		if (file_printf(ms, "%s", m->desc) == -1)
			return -1;
		return 1;
	case FILE_DER:
	case FILE_DEFAULT:	/* nothing to check */
	case FILE_CLEAR:
	default:
		break;
	}
	if (!mconvert(ms, m, flip))
		return 0;