onigenc_unicode_get_case_fold_codes_by_str(OnigEncoding enc,
    OnigCaseFoldType flag, const OnigUChar* p, const OnigUChar* end,
    OnigCaseFoldCodeItem items[])
{
  int n, i, j, k, len;
  OnigCodePoint code, codes[3];
  const CodePointList3 *to, *z3;
  const CodePointList2 *z2;

  n = 0;

  code = ONIGENC_MBC_TO_CODE(enc, p, end);
  len = enclen(enc, p, end);

#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
  if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
    switch (code) {
    case 'I':
      items[0].byte_len = len;
      items[0].code_len = 1;
      items[0].code[0]  = DOTLESS_i;
      return 1;
    case I_WITH_DOT_ABOVE:
      items[0].byte_len = len;
      items[0].code_len = 1;
      items[0].code[0]  = 'i';
      return 1;
    case DOTLESS_i:
      items[0].byte_len = len;
      items[0].code_len = 1;
      items[0].code[0]  = 'I';
      return 1;
    case 'i':
      items[0].byte_len = len;
      items[0].code_len = 1;
      items[0].code[0]  = I_WITH_DOT_ABOVE;
      return 1;
    }
  }
#endif

  if ((to = onigenc_unicode_fold_lookup(code)) != 0) {
    if (OnigCodePointCount(to->n) == 1) {
      OnigCodePoint orig_code = code;

      items[0].byte_len = len;
      items[0].code_len = 1;
      items[0].code[0]  = to->code[0];
      n++;

      code = to->code[0];
      if ((to = onigenc_unicode_unfold1_lookup(code)) != 0 &&
	  CodePointListValidP(to)) {
	for (i = 0; i < OnigCodePointCount(to->n); i++) {
	  if (to->code[i] != orig_code) {
	    items[n].byte_len = len;
	    items[n].code_len = 1;
	    items[n].code[0]  = to->code[i];
	    n++;
	  }
	}
      }
    }
    else if ((flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0) {
      OnigCodePoint cs[3][4];
      int fn, ncs[3];

      for (fn = 0; fn < OnigCodePointCount(to->n); fn++) {
	cs[fn][0] = to->code[fn];
	if ((z3 = onigenc_unicode_unfold1_lookup(cs[fn][0])) != 0) {
	  for (i = 0; i < OnigCodePointCount(z3->n); i++) {
	    cs[fn][i+1] = z3->code[i];
	  }
	  ncs[fn] = OnigCodePointCount(z3->n) + 1;
	}
	else
	  ncs[fn] = 1;
      }

      if (fn == 2) {
	for (i = 0; i < ncs[0]; i++) {
	  for (j = 0; j < ncs[1]; j++) {
	    items[n].byte_len = len;
	    items[n].code_len = 2;
	    items[n].code[0]  = cs[0][i];
	    items[n].code[1]  = cs[1][j];
	    n++;
	  }
	}

	if ((z2 = onigenc_unicode_unfold2_lookup(to->code)) != 0 &&
	    CodePointListValidP(z2)) {
	  for (i = 0; i < OnigCodePointCount(z2->n); i++) {
	    if (z2->code[i] == code) continue;

	    items[n].byte_len = len;
	    items[n].code_len = 1;
	    items[n].code[0]  = z2->code[i];
	    n++;
	  }
	}
      }
      else {
	for (i = 0; i < ncs[0]; i++) {
	  for (j = 0; j < ncs[1]; j++) {
	    for (k = 0; k < ncs[2]; k++) {
	      items[n].byte_len = len;
	      items[n].code_len = 3;
	      items[n].code[0]  = cs[0][i];
	      items[n].code[1]  = cs[1][j];
	      items[n].code[2]  = cs[2][k];
	      n++;
	    }
	  }
	}

	if ((z2 = onigenc_unicode_unfold3_lookup(to->code)) != 0 &&
	    CodePointListValidP(z2)) {
	  for (i = 0; i < OnigCodePointCount(z2->n); i++) {
	    if (z2->code[i] == code) continue;

	    items[n].byte_len = len;
	    items[n].code_len = 1;
	    items[n].code[0]  = z2->code[i];
	    n++;
	  }
	}
      }

      /* multi char folded code is not head of another folded multi char */
      flag = 0; /* DISABLE_CASE_FOLD_MULTI_CHAR(flag); */
    }
  }
  else {
    if ((to = onigenc_unicode_unfold1_lookup(code)) != 0 &&
	CodePointListValidP(to)) {
      for (i = 0; i < OnigCodePointCount(to->n); i++) {
	items[n].byte_len = len;
	items[n].code_len = 1;
	items[n].code[0]  = to->code[i];
	n++;
      }
    }
  }


  if ((flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0) {
    p += len;
    if (p < end) {
      int clen;

      codes[0] = code;
      code = ONIGENC_MBC_TO_CODE(enc, p, end);
      if ((to = onigenc_unicode_fold_lookup(code)) != 0
	  && OnigCodePointCount(to->n) == 1) {
	codes[1] = to->code[0];
      }
      else
	codes[1] = code;

      clen = enclen(enc, p, end);
      len += clen;
      if ((z2 = onigenc_unicode_unfold2_lookup(codes)) != 0 &&
	  CodePointListValidP(z2)) {
	for (i = 0; i < OnigCodePointCount(z2->n); i++) {
	  items[n].byte_len = len;
	  items[n].code_len = 1;
	  items[n].code[0]  = z2->code[i];
	  n++;
	}
      }

      p += clen;
      if (p < end) {
	code = ONIGENC_MBC_TO_CODE(enc, p, end);
	if ((to = onigenc_unicode_fold_lookup(code)) != 0
	    && OnigCodePointCount(to->n) == 1) {
	  codes[2] = to->code[0];
	}
	else
	  codes[2] = code;

	clen = enclen(enc, p, end);
	len += clen;
	if ((z2 = onigenc_unicode_unfold3_lookup(codes)) != 0 &&
	    CodePointListValidP(z2)) {
	  for (i = 0; i < OnigCodePointCount(z2->n); i++) {
	    items[n].byte_len = len;
	    items[n].code_len = 1;
	    items[n].code[0]  = z2->code[i];
	    n++;
	  }
	}
      }
    }
  }

  return n;
}