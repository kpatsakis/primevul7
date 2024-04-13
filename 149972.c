parse_exp(Node** np, OnigToken* tok, int term,
	  UChar** src, UChar* end, ScanEnv* env)
{
  int r, len, group = 0;
  Node* qn;
  Node** targetp;

  *np = NULL;
  if (tok->type == (enum TokenSyms )term)
    goto end_of_token;

  switch (tok->type) {
  case TK_ALT:
  case TK_EOT:
  end_of_token:
    *np = node_new_empty();
    return tok->type;
    break;

  case TK_SUBEXP_OPEN:
    r = parse_enclose(np, tok, TK_SUBEXP_CLOSE, src, end, env);
    if (r < 0) return r;
    if (r == 1) group = 1;
    else if (r == 2) { /* option only */
      Node* target;
      OnigOptionType prev = env->option;

      env->option = NENCLOSE(*np)->option;
      r = fetch_token(tok, src, end, env);
      if (r < 0) {
	env->option = prev;
	return r;
      }
      r = parse_subexp(&target, tok, term, src, end, env);
      env->option = prev;
      if (r < 0) {
	onig_node_free(target);
	return r;
      }
      NENCLOSE(*np)->target = target;
      return tok->type;
    }
    break;

  case TK_SUBEXP_CLOSE:
    if (! IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_UNMATCHED_CLOSE_SUBEXP))
      return ONIGERR_UNMATCHED_CLOSE_PARENTHESIS;

    if (tok->escaped) goto tk_raw_byte;
    else goto tk_byte;
    break;

  case TK_LINEBREAK:
    r = node_linebreak(np, env);
    if (r < 0) return r;
    break;

  case TK_EXTENDED_GRAPHEME_CLUSTER:
    r = node_extended_grapheme_cluster(np, env);
    if (r < 0) return r;
    break;

  case TK_KEEP:
    *np = onig_node_new_anchor(ANCHOR_KEEP);
    CHECK_NULL_RETURN_MEMERR(*np);
    break;

  case TK_STRING:
  tk_byte:
    {
      *np = node_new_str(tok->backp, *src);
      CHECK_NULL_RETURN_MEMERR(*np);

    string_loop:
      while (1) {
	r = fetch_token(tok, src, end, env);
	if (r < 0) return r;
	if (r == TK_STRING) {
	  r = onig_node_str_cat(*np, tok->backp, *src);
	}
#ifndef NUMBERED_CHAR_IS_NOT_CASE_AMBIG
	else if (r == TK_CODE_POINT) {
	  r = node_str_cat_codepoint(*np, env->enc, tok->u.code);
	}
#endif
	else {
	  break;
	}
	if (r < 0) return r;
      }

    string_end:
      targetp = np;
      goto repeat;
    }
    break;

  case TK_RAW_BYTE:
  tk_raw_byte:
    {
      *np = node_new_str_raw_char((UChar )tok->u.c);
      CHECK_NULL_RETURN_MEMERR(*np);
      len = 1;
      while (1) {
	if (len >= ONIGENC_MBC_MINLEN(env->enc)) {
	  if (len == enclen(env->enc, NSTR(*np)->s, NSTR(*np)->end)) {
	    r = fetch_token(tok, src, end, env);
	    NSTRING_CLEAR_RAW(*np);
	    goto string_end;
	  }
	}

	r = fetch_token(tok, src, end, env);
	if (r < 0) return r;
	if (r != TK_RAW_BYTE) {
	  /* Don't use this, it is wrong for little endian encodings. */
#ifdef USE_PAD_TO_SHORT_BYTE_CHAR
	  int rem;
	  if (len < ONIGENC_MBC_MINLEN(env->enc)) {
	    rem = ONIGENC_MBC_MINLEN(env->enc) - len;
	    (void )node_str_head_pad(NSTR(*np), rem, (UChar )0);
	    if (len + rem == enclen(env->enc, NSTR(*np)->s)) {
	      NSTRING_CLEAR_RAW(*np);
	      goto string_end;
	    }
	  }
#endif
	  return ONIGERR_TOO_SHORT_MULTI_BYTE_STRING;
	}

	r = node_str_cat_char(*np, (UChar )tok->u.c);
	if (r < 0) return r;

	len++;
      }
    }
    break;

  case TK_CODE_POINT:
    {
      *np = node_new_empty();
      CHECK_NULL_RETURN_MEMERR(*np);
      r = node_str_cat_codepoint(*np, env->enc, tok->u.code);
      if (r != 0) return r;
#ifdef NUMBERED_CHAR_IS_NOT_CASE_AMBIG
      NSTRING_SET_RAW(*np);
#else
      goto string_loop;
#endif
    }
    break;

  case TK_QUOTE_OPEN:
    {
      OnigCodePoint end_op[2];
      UChar *qstart, *qend, *nextp;

      end_op[0] = (OnigCodePoint )MC_ESC(env->syntax);
      end_op[1] = (OnigCodePoint )'E';
      qstart = *src;
      qend = find_str_position(end_op, 2, qstart, end, &nextp, env->enc);
      if (IS_NULL(qend)) {
	nextp = qend = end;
      }
      *np = node_new_str(qstart, qend);
      CHECK_NULL_RETURN_MEMERR(*np);
      *src = nextp;
    }
    break;

  case TK_CHAR_TYPE:
    {
      switch (tok->u.prop.ctype) {
      case ONIGENC_CTYPE_WORD:
	*np = node_new_ctype(tok->u.prop.ctype, tok->u.prop.not,
			     IS_ASCII_RANGE(env->option));
	CHECK_NULL_RETURN_MEMERR(*np);
	break;

      case ONIGENC_CTYPE_SPACE:
      case ONIGENC_CTYPE_DIGIT:
      case ONIGENC_CTYPE_XDIGIT:
	{
	  CClassNode* cc;

	  *np = node_new_cclass();
	  CHECK_NULL_RETURN_MEMERR(*np);
	  cc = NCCLASS(*np);
	  r = add_ctype_to_cc(cc, tok->u.prop.ctype, 0,
	      IS_ASCII_RANGE(env->option), env);
	  if (r != 0) return r;
	  if (tok->u.prop.not != 0) NCCLASS_SET_NOT(cc);
	}
	break;

      default:
	return ONIGERR_PARSER_BUG;
	break;
      }
    }
    break;

  case TK_CHAR_PROPERTY:
    r = parse_char_property(np, tok, src, end, env);
    if (r != 0) return r;
    break;

  case TK_CC_OPEN:
    {
      Node *asc_node;
      CClassNode* cc;
      OnigCodePoint code;

      r = parse_char_class(np, &asc_node, tok, src, end, env);
      if (r != 0) {
	onig_node_free(asc_node);
	return r;
      }

      cc = NCCLASS(*np);
      if (is_onechar_cclass(cc, &code)) {
	onig_node_free(*np);
	onig_node_free(asc_node);
	*np = node_new_empty();
	CHECK_NULL_RETURN_MEMERR(*np);
	r = node_str_cat_codepoint(*np, env->enc, code);
	if (r != 0) return r;
	goto string_loop;
      }
      if (IS_IGNORECASE(env->option)) {
	r = cclass_case_fold(np, cc, NCCLASS(asc_node), env);
	if (r != 0) {
	  onig_node_free(asc_node);
	  return r;
	}
      }
      onig_node_free(asc_node);
    }
    break;

  case TK_ANYCHAR:
    *np = node_new_anychar();
    CHECK_NULL_RETURN_MEMERR(*np);
    break;

  case TK_ANYCHAR_ANYTIME:
    *np = node_new_anychar();
    CHECK_NULL_RETURN_MEMERR(*np);
    qn = node_new_quantifier(0, REPEAT_INFINITE, 0);
    CHECK_NULL_RETURN_MEMERR(qn);
    NQTFR(qn)->target = *np;
    *np = qn;
    break;

  case TK_BACKREF:
    len = tok->u.backref.num;
    *np = node_new_backref(len,
		   (len > 1 ? tok->u.backref.refs : &(tok->u.backref.ref1)),
			   tok->u.backref.by_name,
#ifdef USE_BACKREF_WITH_LEVEL
			   tok->u.backref.exist_level,
			   tok->u.backref.level,
#endif
			   env);
    CHECK_NULL_RETURN_MEMERR(*np);
    break;

#ifdef USE_SUBEXP_CALL
  case TK_CALL:
    {
      int gnum = tok->u.call.gnum;

      if (gnum < 0 || tok->u.call.rel != 0) {
	if (gnum > 0) gnum--;
	gnum = BACKREF_REL_TO_ABS(gnum, env);
	if (gnum <= 0)
	  return ONIGERR_INVALID_BACKREF;
      }
      *np = node_new_call(tok->u.call.name, tok->u.call.name_end, gnum);
      CHECK_NULL_RETURN_MEMERR(*np);
      env->num_call++;
    }
    break;
#endif

  case TK_ANCHOR:
    *np = onig_node_new_anchor(tok->u.anchor.subtype);
    CHECK_NULL_RETURN_MEMERR(*np);
    NANCHOR(*np)->ascii_range = tok->u.anchor.ascii_range;
    break;

  case TK_OP_REPEAT:
  case TK_INTERVAL:
    if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_CONTEXT_INDEP_REPEAT_OPS)) {
      if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_CONTEXT_INVALID_REPEAT_OPS))
	return ONIGERR_TARGET_OF_REPEAT_OPERATOR_NOT_SPECIFIED;
      else
	*np = node_new_empty();
    }
    else {
      goto tk_byte;
    }
    break;

  default:
    return ONIGERR_PARSER_BUG;
    break;
  }

  {
    targetp = np;

  re_entry:
    r = fetch_token(tok, src, end, env);
    if (r < 0) return r;

  repeat:
    if (r == TK_OP_REPEAT || r == TK_INTERVAL) {
      if (is_invalid_quantifier_target(*targetp))
	return ONIGERR_TARGET_OF_REPEAT_OPERATOR_INVALID;

      qn = node_new_quantifier(tok->u.repeat.lower, tok->u.repeat.upper,
			       (r == TK_INTERVAL ? 1 : 0));
      CHECK_NULL_RETURN_MEMERR(qn);
      NQTFR(qn)->greedy = tok->u.repeat.greedy;
      r = set_quantifier(qn, *targetp, group, env);
      if (r < 0) {
	onig_node_free(qn);
	return r;
      }

      if (tok->u.repeat.possessive != 0) {
	Node* en;
	en = node_new_enclose(ENCLOSE_STOP_BACKTRACK);
	if (IS_NULL(en)) {
	  onig_node_free(qn);
	  return ONIGERR_MEMORY;
	}
	NENCLOSE(en)->target = qn;
	qn = en;
      }

      if (r == 0) {
	*targetp = qn;
      }
      else if (r == 1) {
	onig_node_free(qn);
      }
      else if (r == 2) { /* split case: /abc+/ */
	Node *tmp;

	*targetp = node_new_list(*targetp, NULL);
	if (IS_NULL(*targetp)) {
	  onig_node_free(qn);
	  return ONIGERR_MEMORY;
	}
	tmp = NCDR(*targetp) = node_new_list(qn, NULL);
	if (IS_NULL(tmp)) {
	  onig_node_free(qn);
	  return ONIGERR_MEMORY;
	}
	targetp = &(NCAR(tmp));
      }
      goto re_entry;
    }
  }

  return r;
}