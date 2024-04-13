parse_posix_bracket(CClassNode* cc, CClassNode* asc_cc,
		    UChar** src, UChar* end, ScanEnv* env)
{
#define POSIX_BRACKET_CHECK_LIMIT_LENGTH  20
#define POSIX_BRACKET_NAME_MIN_LEN         4

  static const PosixBracketEntryType PBS[] = {
    POSIX_BRACKET_ENTRY_INIT("alnum",  ONIGENC_CTYPE_ALNUM),
    POSIX_BRACKET_ENTRY_INIT("alpha",  ONIGENC_CTYPE_ALPHA),
    POSIX_BRACKET_ENTRY_INIT("blank",  ONIGENC_CTYPE_BLANK),
    POSIX_BRACKET_ENTRY_INIT("cntrl",  ONIGENC_CTYPE_CNTRL),
    POSIX_BRACKET_ENTRY_INIT("digit",  ONIGENC_CTYPE_DIGIT),
    POSIX_BRACKET_ENTRY_INIT("graph",  ONIGENC_CTYPE_GRAPH),
    POSIX_BRACKET_ENTRY_INIT("lower",  ONIGENC_CTYPE_LOWER),
    POSIX_BRACKET_ENTRY_INIT("print",  ONIGENC_CTYPE_PRINT),
    POSIX_BRACKET_ENTRY_INIT("punct",  ONIGENC_CTYPE_PUNCT),
    POSIX_BRACKET_ENTRY_INIT("space",  ONIGENC_CTYPE_SPACE),
    POSIX_BRACKET_ENTRY_INIT("upper",  ONIGENC_CTYPE_UPPER),
    POSIX_BRACKET_ENTRY_INIT("xdigit", ONIGENC_CTYPE_XDIGIT),
    POSIX_BRACKET_ENTRY_INIT("ascii",  ONIGENC_CTYPE_ASCII),
    POSIX_BRACKET_ENTRY_INIT("word",   ONIGENC_CTYPE_WORD),
  };

  const PosixBracketEntryType *pb;
  int not, i, r;
  int ascii_range;
  OnigCodePoint c;
  OnigEncoding enc = env->enc;
  UChar *p = *src;

  if (PPEEK_IS('^')) {
    PINC_S;
    not = 1;
  }
  else
    not = 0;

  if (onigenc_strlen(enc, p, end) < POSIX_BRACKET_NAME_MIN_LEN + 3)
    goto not_posix_bracket;

  ascii_range = IS_ASCII_RANGE(env->option) &&
		  ! IS_POSIX_BRACKET_ALL_RANGE(env->option);
  for (pb = PBS; pb < PBS + numberof(PBS); pb++) {
    if (onigenc_with_ascii_strncmp(enc, p, end, pb->name, pb->len) == 0) {
      p = (UChar* )onigenc_step(enc, p, end, pb->len);
      if (onigenc_with_ascii_strncmp(enc, p, end, (UChar* )":]", 2) != 0)
	return ONIGERR_INVALID_POSIX_BRACKET_TYPE;

      r = add_ctype_to_cc(cc, pb->ctype, not, ascii_range, env);
      if (r != 0) return r;

      if (IS_NOT_NULL(asc_cc)) {
	if (pb->ctype != ONIGENC_CTYPE_WORD &&
	    pb->ctype != ONIGENC_CTYPE_ASCII &&
	    !ascii_range)
	  r = add_ctype_to_cc(asc_cc, pb->ctype, not, ascii_range, env);
	if (r != 0) return r;
      }

      PINC_S; PINC_S;
      *src = p;
      return 0;
    }
  }

 not_posix_bracket:
  c = 0;
  i = 0;
  while (!PEND && ((c = PPEEK) != ':') && c != ']') {
    PINC_S;
    if (++i > POSIX_BRACKET_CHECK_LIMIT_LENGTH) break;
  }
  if (c == ':' && ! PEND) {
    PINC_S;
    if (! PEND) {
      PFETCH_S(c);
      if (c == ']')
	return ONIGERR_INVALID_POSIX_BRACKET_TYPE;
    }
  }

  return 1;  /* 1: is not POSIX bracket, but no error. */
}