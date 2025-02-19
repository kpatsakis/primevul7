fetch_token(OnigToken* tok, UChar** src, UChar* end, ScanEnv* env)
{
  int r, num;
  OnigCodePoint c;
  OnigEncoding enc = env->enc;
  const OnigSyntaxType* syn = env->syntax;
  UChar* prev;
  UChar* p = *src;
  PFETCH_READY;

 start:
  if (PEND) {
    tok->type = TK_EOT;
    return tok->type;
  }

  tok->type  = TK_STRING;
  tok->base  = 0;
  tok->backp = p;

  PFETCH(c);
  if (IS_MC_ESC_CODE(c, syn)) {
    if (PEND) return ONIGERR_END_PATTERN_AT_ESCAPE;

    tok->backp = p;
    PFETCH(c);

    tok->u.c = c;
    tok->escaped = 1;
    switch (c) {
    case '*':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_ASTERISK_ZERO_INF)) break;
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 0;
      tok->u.repeat.upper = REPEAT_INFINITE;
      goto greedy_check;
      break;

    case '+':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_PLUS_ONE_INF)) break;
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 1;
      tok->u.repeat.upper = REPEAT_INFINITE;
      goto greedy_check;
      break;

    case '?':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_QMARK_ZERO_ONE)) break;
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 0;
      tok->u.repeat.upper = 1;
    greedy_check:
      if (!PEND && PPEEK_IS('?') &&
	  IS_SYNTAX_OP(syn, ONIG_SYN_OP_QMARK_NON_GREEDY)) {
	PFETCH(c);
	tok->u.repeat.greedy     = 0;
	tok->u.repeat.possessive = 0;
      }
      else {
      possessive_check:
	if (!PEND && PPEEK_IS('+') &&
	    ((IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_PLUS_POSSESSIVE_REPEAT) &&
	      tok->type != TK_INTERVAL)  ||
	     (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_PLUS_POSSESSIVE_INTERVAL) &&
	      tok->type == TK_INTERVAL))) {
	  PFETCH(c);
	  tok->u.repeat.greedy     = 1;
	  tok->u.repeat.possessive = 1;
	}
	else {
	  tok->u.repeat.greedy     = 1;
	  tok->u.repeat.possessive = 0;
	}
      }
      break;

    case '{':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_BRACE_INTERVAL)) break;
      r = fetch_range_quantifier(&p, end, tok, env);
      if (r < 0) return r;  /* error */
      if (r == 0) goto greedy_check;
      else if (r == 2) { /* {n} */
	if (IS_SYNTAX_BV(syn, ONIG_SYN_FIXED_INTERVAL_IS_GREEDY_ONLY))
	  goto possessive_check;

	goto greedy_check;
      }
      /* r == 1 : normal char */
      break;

    case '|':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_VBAR_ALT)) break;
      tok->type = TK_ALT;
      break;

    case '(':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_LPAREN_SUBEXP)) break;
      tok->type = TK_SUBEXP_OPEN;
      break;

    case ')':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_LPAREN_SUBEXP)) break;
      tok->type = TK_SUBEXP_CLOSE;
      break;

    case 'w':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_W_WORD)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_WORD;
      tok->u.prop.not   = 0;
      break;

    case 'W':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_W_WORD)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_WORD;
      tok->u.prop.not   = 1;
      break;

    case 'b':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_B_WORD_BOUND)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor.subtype = ANCHOR_WORD_BOUND;
      tok->u.anchor.ascii_range = IS_ASCII_RANGE(env->option)
		&& ! IS_WORD_BOUND_ALL_RANGE(env->option);
      break;

    case 'B':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_B_WORD_BOUND)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor.subtype = ANCHOR_NOT_WORD_BOUND;
      tok->u.anchor.ascii_range = IS_ASCII_RANGE(env->option)
		&& ! IS_WORD_BOUND_ALL_RANGE(env->option);
      break;

#ifdef USE_WORD_BEGIN_END
    case '<':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_LTGT_WORD_BEGIN_END)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor.subtype = ANCHOR_WORD_BEGIN;
      tok->u.anchor.ascii_range = IS_ASCII_RANGE(env->option);
      break;

    case '>':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_LTGT_WORD_BEGIN_END)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor.subtype = ANCHOR_WORD_END;
      tok->u.anchor.ascii_range = IS_ASCII_RANGE(env->option);
      break;
#endif

    case 's':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_S_WHITE_SPACE)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_SPACE;
      tok->u.prop.not   = 0;
      break;

    case 'S':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_S_WHITE_SPACE)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_SPACE;
      tok->u.prop.not   = 1;
      break;

    case 'd':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_D_DIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_DIGIT;
      tok->u.prop.not   = 0;
      break;

    case 'D':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_D_DIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_DIGIT;
      tok->u.prop.not   = 1;
      break;

    case 'h':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_H_XDIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_XDIGIT;
      tok->u.prop.not   = 0;
      break;

    case 'H':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_H_XDIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_XDIGIT;
      tok->u.prop.not   = 1;
      break;

    case 'A':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_AZ_BUF_ANCHOR)) break;
    begin_buf:
      tok->type = TK_ANCHOR;
      tok->u.anchor.subtype = ANCHOR_BEGIN_BUF;
      break;

    case 'Z':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_AZ_BUF_ANCHOR)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor.subtype = ANCHOR_SEMI_END_BUF;
      break;

    case 'z':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_AZ_BUF_ANCHOR)) break;
    end_buf:
      tok->type = TK_ANCHOR;
      tok->u.anchor.subtype = ANCHOR_END_BUF;
      break;

    case 'G':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_CAPITAL_G_BEGIN_ANCHOR)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor.subtype = ANCHOR_BEGIN_POSITION;
      break;

    case '`':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_GNU_BUF_ANCHOR)) break;
      goto begin_buf;
      break;

    case '\'':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_GNU_BUF_ANCHOR)) break;
      goto end_buf;
      break;

    case 'x':
      if (PEND) break;

      prev = p;
      if (PPEEK_IS('{') && IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_X_BRACE_HEX8)) {
	PINC;
	num = scan_unsigned_hexadecimal_number(&p, end, 0, 8, enc);
	if (num < 0) return ONIGERR_TOO_BIG_WIDE_CHAR_VALUE;
	if (!PEND) {
	  if (ONIGENC_IS_CODE_XDIGIT(enc, PPEEK))
	    return ONIGERR_TOO_LONG_WIDE_CHAR_VALUE;
	}

	if ((p > prev + enclen(enc, prev, end)) && !PEND && PPEEK_IS('}')) {
	  PINC;
	  tok->type   = TK_CODE_POINT;
	  tok->u.code = (OnigCodePoint )num;
	}
	else {
	  /* can't read nothing or invalid format */
	  p = prev;
	}
      }
      else if (IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_X_HEX2)) {
	num = scan_unsigned_hexadecimal_number(&p, end, 0, 2, enc);
	if (num < 0) return ONIGERR_TOO_BIG_NUMBER;
	if (p == prev) {  /* can't read nothing. */
	  num = 0; /* but, it's not error */
	}
	tok->type = TK_RAW_BYTE;
	tok->base = 16;
	tok->u.c  = num;
      }
      break;

    case 'u':
      if (PEND) break;

      prev = p;
      if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_U_HEX4)) {
	num = scan_unsigned_hexadecimal_number(&p, end, 4, 4, enc);
	if (num < -1) return ONIGERR_TOO_SHORT_DIGITS;
	else if (num < 0) return ONIGERR_TOO_BIG_NUMBER;
	if (p == prev) {  /* can't read nothing. */
	  num = 0; /* but, it's not error */
	}
	tok->type   = TK_CODE_POINT;
	tok->base   = 16;
	tok->u.code = (OnigCodePoint )num;
      }
      break;

    case 'o':
      if (PEND) break;

      prev = p;
      if (PPEEK_IS('{') && IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_O_BRACE_OCTAL)) {
	PINC;
	num = scan_unsigned_octal_number(&p, end, 11, enc);
	if (num < 0) return ONIGERR_TOO_BIG_WIDE_CHAR_VALUE;
	if (!PEND) {
	  OnigCodePoint c = PPEEK;
	  if (ONIGENC_IS_CODE_DIGIT(enc, c) && c < '8')
	    return ONIGERR_TOO_LONG_WIDE_CHAR_VALUE;
	}

	if ((p > prev + enclen(enc, prev, end)) && !PEND && PPEEK_IS('}')) {
	  PINC;
	  tok->type   = TK_CODE_POINT;
	  tok->u.code = (OnigCodePoint )num;
	}
	else {
	  /* can't read nothing or invalid format */
	  p = prev;
	}
      }
      break;

    case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      PUNFETCH;
      prev = p;
      num = onig_scan_unsigned_number(&p, end, enc);
      if (num < 0 || num > ONIG_MAX_BACKREF_NUM) {
	goto skip_backref;
      }

      if (IS_SYNTAX_OP(syn, ONIG_SYN_OP_DECIMAL_BACKREF) &&
	  (num <= env->num_mem || num <= 9)) { /* This spec. from GNU regex */
	if (IS_SYNTAX_BV(syn, ONIG_SYN_STRICT_CHECK_BACKREF)) {
	  if (num > env->num_mem || IS_NULL(SCANENV_MEM_NODES(env)[num]))
	    return ONIGERR_INVALID_BACKREF;
	}

	tok->type = TK_BACKREF;
	tok->u.backref.num     = 1;
	tok->u.backref.ref1    = num;
	tok->u.backref.by_name = 0;
#ifdef USE_BACKREF_WITH_LEVEL
	tok->u.backref.exist_level = 0;
#endif
	break;
      }

    skip_backref:
      if (c == '8' || c == '9') {
	/* normal char */
	p = prev; PINC;
	break;
      }

      p = prev;
      /* fall through */
    case '0':
      if (IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_OCTAL3)) {
	prev = p;
	num = scan_unsigned_octal_number(&p, end, (c == '0' ? 2:3), enc);
	if (num < 0) return ONIGERR_TOO_BIG_NUMBER;
	if (p == prev) {  /* can't read nothing. */
	  num = 0; /* but, it's not error */
	}
	tok->type = TK_RAW_BYTE;
	tok->base = 8;
	tok->u.c  = num;
      }
      else if (c != '0') {
	PINC;
      }
      break;

#ifdef USE_NAMED_GROUP
    case 'k':
      if (!PEND && IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_K_NAMED_BACKREF)) {
	PFETCH(c);
	if (c == '<' || c == '\'') {
	  r = fetch_named_backref_token(c, tok, &p, end, env);
	  if (r < 0) return r;
	}
	else {
	  PUNFETCH;
	  onig_syntax_warn(env, "invalid back reference");
	}
      }
      break;
#endif

#if defined(USE_SUBEXP_CALL) || defined(USE_NAMED_GROUP)
    case 'g':
# ifdef USE_NAMED_GROUP
      if (!PEND && IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_G_BRACE_BACKREF)) {
	PFETCH(c);
	if (c == '{') {
	  r = fetch_named_backref_token(c, tok, &p, end, env);
	  if (r < 0) return r;
	}
	else
	  PUNFETCH;
      }
# endif
# ifdef USE_SUBEXP_CALL
      if (!PEND && IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_G_SUBEXP_CALL)) {
	PFETCH(c);
	if (c == '<' || c == '\'') {
	  int gnum = -1, rel = 0;
	  UChar* name_end;
	  OnigCodePoint cnext;

	  cnext = PPEEK;
	  if (cnext == '0') {
	    PINC;
	    if (PPEEK_IS(get_name_end_code_point(c))) {  /* \g<0>, \g'0' */
	      PINC;
	      name_end = p;
	      gnum = 0;
	    }
	  }
	  else if (cnext == '+') {
	    PINC;
	    rel = 1;
	  }
	  prev = p;
	  if (gnum < 0) {
	    r = fetch_name((OnigCodePoint )c, &p, end, &name_end, env, &gnum, 1);
	    if (r < 0) return r;
	  }

	  tok->type = TK_CALL;
	  tok->u.call.name     = prev;
	  tok->u.call.name_end = name_end;
	  tok->u.call.gnum     = gnum;
	  tok->u.call.rel      = rel;
	}
	else {
	  onig_syntax_warn(env, "invalid subexp call");
	  PUNFETCH;
	}
      }
# endif
      break;
#endif

    case 'Q':
      if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_CAPITAL_Q_QUOTE)) {
	tok->type = TK_QUOTE_OPEN;
      }
      break;

    case 'p':
    case 'P':
      if (PPEEK_IS('{') &&
	  IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_P_BRACE_CHAR_PROPERTY)) {
	PINC;
	tok->type = TK_CHAR_PROPERTY;
	tok->u.prop.not = (c == 'P' ? 1 : 0);

	if (!PEND && IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_P_BRACE_CIRCUMFLEX_NOT)) {
	  PFETCH(c);
	  if (c == '^') {
	    tok->u.prop.not = (tok->u.prop.not == 0 ? 1 : 0);
	  }
	  else
	    PUNFETCH;
	}
      }
      else {
	onig_syntax_warn(env, "invalid Unicode Property \\%c", c);
      }
      break;

    case 'R':
      if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_CAPITAL_R_LINEBREAK)) {
	tok->type = TK_LINEBREAK;
      }
      break;

    case 'X':
      if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_CAPITAL_X_EXTENDED_GRAPHEME_CLUSTER)) {
	tok->type = TK_EXTENDED_GRAPHEME_CLUSTER;
      }
      break;

    case 'K':
      if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_CAPITAL_K_KEEP)) {
	tok->type = TK_KEEP;
      }
      break;

    default:
      {
	OnigCodePoint c2;

	PUNFETCH;
	num = fetch_escaped_value(&p, end, env, &c2);
	if (num < 0) return num;
	/* set_raw: */
	if ((OnigCodePoint)tok->u.c != c2) {
	  tok->type = TK_CODE_POINT;
	  tok->u.code = (OnigCodePoint )c2;
	}
	else { /* string */
	  p = tok->backp + enclen(enc, tok->backp, end);
	}
      }
      break;
    }
  }
  else {
    tok->u.c = c;
    tok->escaped = 0;

#ifdef USE_VARIABLE_META_CHARS
    if ((c != ONIG_INEFFECTIVE_META_CHAR) &&
	IS_SYNTAX_OP(syn, ONIG_SYN_OP_VARIABLE_META_CHARACTERS)) {
      if (c == MC_ANYCHAR(syn))
	goto any_char;
      else if (c == MC_ANYTIME(syn))
	goto anytime;
      else if (c == MC_ZERO_OR_ONE_TIME(syn))
	goto zero_or_one_time;
      else if (c == MC_ONE_OR_MORE_TIME(syn))
	goto one_or_more_time;
      else if (c == MC_ANYCHAR_ANYTIME(syn)) {
	tok->type = TK_ANYCHAR_ANYTIME;
	goto out;
      }
    }
#endif

    switch (c) {
    case '.':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_DOT_ANYCHAR)) break;
#ifdef USE_VARIABLE_META_CHARS
    any_char:
#endif
      tok->type = TK_ANYCHAR;
      break;

    case '*':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ASTERISK_ZERO_INF)) break;
#ifdef USE_VARIABLE_META_CHARS
    anytime:
#endif
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 0;
      tok->u.repeat.upper = REPEAT_INFINITE;
      goto greedy_check;
      break;

    case '+':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_PLUS_ONE_INF)) break;
#ifdef USE_VARIABLE_META_CHARS
    one_or_more_time:
#endif
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 1;
      tok->u.repeat.upper = REPEAT_INFINITE;
      goto greedy_check;
      break;

    case '?':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_QMARK_ZERO_ONE)) break;
#ifdef USE_VARIABLE_META_CHARS
    zero_or_one_time:
#endif
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 0;
      tok->u.repeat.upper = 1;
      goto greedy_check;
      break;

    case '{':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_BRACE_INTERVAL)) break;
      r = fetch_range_quantifier(&p, end, tok, env);
      if (r < 0) return r;  /* error */
      if (r == 0) goto greedy_check;
      else if (r == 2) { /* {n} */
	if (IS_SYNTAX_BV(syn, ONIG_SYN_FIXED_INTERVAL_IS_GREEDY_ONLY))
	  goto possessive_check;

	goto greedy_check;
      }
      /* r == 1 : normal char */
      break;

    case '|':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_VBAR_ALT)) break;
      tok->type = TK_ALT;
      break;

    case '(':
      if (PPEEK_IS('?') &&
	  IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_QMARK_GROUP_EFFECT)) {
	PINC;
	if (PPEEK_IS('#')) {
	  PFETCH(c);
	  while (1) {
	    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
	    PFETCH(c);
	    if (c == MC_ESC(syn)) {
	      if (!PEND) PFETCH(c);
	    }
	    else {
	      if (c == ')') break;
	    }
	  }
	  goto start;
	}
#ifdef USE_PERL_SUBEXP_CALL
	/* (?&name), (?n), (?R), (?0), (?+n), (?-n) */
	c = PPEEK;
	if ((c == '&' || c == 'R' || ONIGENC_IS_CODE_DIGIT(enc, c)) &&
	    IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_SUBEXP_CALL)) {
	  /* (?&name), (?n), (?R), (?0) */
	  int gnum;
	  UChar *name;
	  UChar *name_end;

	  if (c == 'R' || c == '0') {
	    PINC;   /* skip 'R' / '0' */
	    if (!PPEEK_IS(')')) return ONIGERR_INVALID_GROUP_NAME;
	    PINC;   /* skip ')' */
	    name_end = name = p;
	    gnum = 0;
	  }
	  else {
	    int numref = 1;
	    if (c == '&') {     /* (?&name) */
	      PINC;
	      numref = 0;       /* don't allow number name */
	    }
	    name = p;
	    r = fetch_name((OnigCodePoint )'(', &p, end, &name_end, env, &gnum, numref);
	    if (r < 0) return r;
	  }

	  tok->type = TK_CALL;
	  tok->u.call.name     = name;
	  tok->u.call.name_end = name_end;
	  tok->u.call.gnum     = gnum;
	  tok->u.call.rel      = 0;
	  break;
	}
	else if ((c == '-' || c == '+') &&
	    IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_SUBEXP_CALL)) {
	  /* (?+n), (?-n) */
	  int gnum;
	  UChar *name;
	  UChar *name_end;
	  OnigCodePoint cnext;
	  PFETCH_READY;

	  PINC;     /* skip '-' / '+' */
	  cnext = PPEEK;
	  if (ONIGENC_IS_CODE_DIGIT(enc, cnext)) {
	    if (c == '-') PUNFETCH;
	    name = p;
	    r = fetch_name((OnigCodePoint )'(', &p, end, &name_end, env, &gnum, 1);
	    if (r < 0) return r;

	    tok->type = TK_CALL;
	    tok->u.call.name     = name;
	    tok->u.call.name_end = name_end;
	    tok->u.call.gnum     = gnum;
	    tok->u.call.rel      = 1;
	    break;
	  }
	}
#endif /* USE_PERL_SUBEXP_CALL */
#ifdef USE_CAPITAL_P_NAMED_GROUP
	if (PPEEK_IS('P') &&
	    IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_CAPITAL_P_NAMED_GROUP)) {
	  int gnum;
	  UChar *name;
	  UChar *name_end;
	  PFETCH_READY;

	  PINC;     /* skip 'P' */
	  if (PEND) return ONIGERR_UNDEFINED_GROUP_OPTION;
	  PFETCH(c);
	  if (c == '=') {       /* (?P=name): backref */
	    r = fetch_named_backref_token((OnigCodePoint )'(', tok, &p, end, env);
	    if (r < 0) return r;
	    break;
	  }
	  else if (c == '>') {  /* (?P>name): subexp call */
	    name = p;
	    r = fetch_name((OnigCodePoint )'(', &p, end, &name_end, env, &gnum, 0);
	    if (r < 0) return r;

	    tok->type = TK_CALL;
	    tok->u.call.name     = name;
	    tok->u.call.name_end = name_end;
	    tok->u.call.gnum     = gnum;
	    tok->u.call.rel      = 0;
	    break;
	  }
	}
#endif /* USE_CAPITAL_P_NAMED_GROUP */
	PUNFETCH;
      }

      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_LPAREN_SUBEXP)) break;
      tok->type = TK_SUBEXP_OPEN;
      break;

    case ')':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_LPAREN_SUBEXP)) break;
      tok->type = TK_SUBEXP_CLOSE;
      break;

    case '^':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_LINE_ANCHOR)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor.subtype = (IS_SINGLELINE(env->option)
			       ? ANCHOR_BEGIN_BUF : ANCHOR_BEGIN_LINE);
      break;

    case '$':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_LINE_ANCHOR)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor.subtype = (IS_SINGLELINE(env->option)
			       ? ANCHOR_SEMI_END_BUF : ANCHOR_END_LINE);
      break;

    case '[':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_BRACKET_CC)) break;
      tok->type = TK_CC_OPEN;
      break;

    case ']':
      if (*src > env->pattern)   /* /].../ is allowed. */
	CLOSE_BRACKET_WITHOUT_ESC_WARN(env, (UChar* )"]");
      break;

    case '#':
      if (IS_EXTEND(env->option)) {
	while (!PEND) {
	  PFETCH(c);
	  if (ONIGENC_IS_CODE_NEWLINE(enc, c))
	    break;
	}
	goto start;
	break;
      }
      break;

    case ' ': case '\t': case '\n': case '\r': case '\f':
      if (IS_EXTEND(env->option))
	goto start;
      break;

    default:
      /* string */
      break;
    }
  }

#ifdef USE_VARIABLE_META_CHARS
 out:
#endif
  *src = p;
  return tok->type;
}