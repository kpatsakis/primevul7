add_ctype_to_cc(CClassNode* cc, int ctype, int not, int ascii_range, ScanEnv* env)
{
  int maxcode;
  int c, r;
  const OnigCodePoint *ranges;
  OnigCodePoint sb_out;
  OnigEncoding enc = env->enc;

  r = ONIGENC_GET_CTYPE_CODE_RANGE(enc, ctype, &sb_out, &ranges);
  if (r == 0) {
    if (ascii_range) {
      CClassNode ccwork;
      initialize_cclass(&ccwork);
      r = add_ctype_to_cc_by_range(&ccwork, ctype, not, env, sb_out,
				   ranges);
      if (r == 0) {
	if (not) {
	  r = add_code_range_to_buf0(&(ccwork.mbuf), env, 0x80, ONIG_LAST_CODE_POINT, FALSE);
	}
	else {
	  CClassNode ccascii;
	  initialize_cclass(&ccascii);
	  if (ONIGENC_MBC_MINLEN(env->enc) > 1) {
	    r = add_code_range(&(ccascii.mbuf), env, 0x00, 0x7F);
	  }
	  else {
	    bitset_set_range(env, ccascii.bs, 0x00, 0x7F);
	    r = 0;
	  }
	  if (r == 0) {
	    r = and_cclass(&ccwork, &ccascii, env);
	  }
	  if (IS_NOT_NULL(ccascii.mbuf)) bbuf_free(ccascii.mbuf);
	}
	if (r == 0) {
	  r = or_cclass(cc, &ccwork, env);
	}
	if (IS_NOT_NULL(ccwork.mbuf)) bbuf_free(ccwork.mbuf);
      }
    }
    else {
      r = add_ctype_to_cc_by_range(cc, ctype, not, env, sb_out, ranges);
    }
    return r;
  }
  else if (r != ONIG_NO_SUPPORT_CONFIG) {
    return r;
  }

  maxcode = ascii_range ? 0x80 : SINGLE_BYTE_SIZE;
  r = 0;
  switch (ctype) {
  case ONIGENC_CTYPE_ALPHA:
  case ONIGENC_CTYPE_BLANK:
  case ONIGENC_CTYPE_CNTRL:
  case ONIGENC_CTYPE_DIGIT:
  case ONIGENC_CTYPE_LOWER:
  case ONIGENC_CTYPE_PUNCT:
  case ONIGENC_CTYPE_SPACE:
  case ONIGENC_CTYPE_UPPER:
  case ONIGENC_CTYPE_XDIGIT:
  case ONIGENC_CTYPE_ASCII:
  case ONIGENC_CTYPE_ALNUM:
    if (not != 0) {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (! ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
	  BITSET_SET_BIT_CHKDUP(cc->bs, c);
      }
      ADD_ALL_MULTI_BYTE_RANGE(enc, cc->mbuf);
    }
    else {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
	  BITSET_SET_BIT_CHKDUP(cc->bs, c);
      }
    }
    break;

  case ONIGENC_CTYPE_GRAPH:
  case ONIGENC_CTYPE_PRINT:
    if (not != 0) {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (! ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype)
	    || c >= maxcode)
	  BITSET_SET_BIT_CHKDUP(cc->bs, c);
      }
      if (ascii_range)
	ADD_ALL_MULTI_BYTE_RANGE(enc, cc->mbuf);
    }
    else {
      for (c = 0; c < maxcode; c++) {
	if (ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
	  BITSET_SET_BIT_CHKDUP(cc->bs, c);
      }
      if (! ascii_range)
	ADD_ALL_MULTI_BYTE_RANGE(enc, cc->mbuf);
    }
    break;

  case ONIGENC_CTYPE_WORD:
    if (not == 0) {
      for (c = 0; c < maxcode; c++) {
	if (ONIGENC_IS_CODE_WORD(enc, c)) BITSET_SET_BIT_CHKDUP(cc->bs, c);
      }
      if (! ascii_range)
	ADD_ALL_MULTI_BYTE_RANGE(enc, cc->mbuf);
    }
    else {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if ((ONIGENC_CODE_TO_MBCLEN(enc, c) > 0) /* check invalid code point */
	    && (! ONIGENC_IS_CODE_WORD(enc, c) || c >= maxcode))
	  BITSET_SET_BIT_CHKDUP(cc->bs, c);
      }
      if (ascii_range)
	ADD_ALL_MULTI_BYTE_RANGE(enc, cc->mbuf);
    }
    break;

  default:
    return ONIGERR_PARSER_BUG;
    break;
  }

  return r;
}