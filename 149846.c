update_string_node_case_fold(regex_t* reg, Node *node)
{
  UChar *p, *end, buf[ONIGENC_MBC_CASE_FOLD_MAXLEN];
  UChar *sbuf, *ebuf, *sp;
  int r, i, len;
  OnigDistance sbuf_size;
  StrNode* sn = NSTR(node);

  end = sn->end;
  sbuf_size = (end - sn->s) * 2;
  sbuf = (UChar* )xmalloc(sbuf_size);
  CHECK_NULL_RETURN_MEMERR(sbuf);
  ebuf = sbuf + sbuf_size;

  sp = sbuf;
  p = sn->s;
  while (p < end) {
    len = ONIGENC_MBC_CASE_FOLD(reg->enc, reg->case_fold_flag, &p, end, buf);
    for (i = 0; i < len; i++) {
      if (sp >= ebuf) {
	UChar* p = (UChar* )xrealloc(sbuf, sbuf_size * 2);
	if (IS_NULL(p)) {
	  xfree(sbuf);
	  return ONIGERR_MEMORY;
	}
	sbuf = p;
	sp = sbuf + sbuf_size;
	sbuf_size *= 2;
	ebuf = sbuf + sbuf_size;
      }

      *sp++ = buf[i];
    }
  }

  r = onig_node_str_set(node, sbuf, sp);

  xfree(sbuf);
  return r;
}