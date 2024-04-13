onigenc_unicode_apply_all_case_fold(OnigCaseFoldType flag,
				    OnigApplyAllCaseFoldFunc f, void* arg,
				    OnigEncoding enc ARG_UNUSED)
{
  const CaseUnfold_11_Type* p11;
  OnigCodePoint code;
  int i, j, k, r;

  for (i = 0; i < numberof(CaseUnfold_11); i++) {
    p11 = &CaseUnfold_11[i];
    for (j = 0; j < OnigCodePointCount(p11->to.n); j++) {
      code = p11->from;
      r = (*f)(p11->to.code[j], &code, 1, arg);
      if (r != 0) return r;

      code = p11->to.code[j];
      r = (*f)(p11->from, &code, 1, arg);
      if (r != 0) return r;

      for (k = 0; k < j; k++) {
	r = (*f)(p11->to.code[j], (OnigCodePoint* )(&p11->to.code[k]), 1, arg);
	if (r != 0) return r;

	r = (*f)(p11->to.code[k], (OnigCodePoint* )(&p11->to.code[j]), 1, arg);
	if (r != 0) return r;
      }
    }
  }

#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
  if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
    code = DOTLESS_i;
    r = (*f)('I', &code, 1, arg);
    if (r != 0) return r;
    code = 'I';
    r = (*f)(DOTLESS_i, &code, 1, arg);
    if (r != 0) return r;

    code = I_WITH_DOT_ABOVE;
    r = (*f)('i', &code, 1, arg);
    if (r != 0) return r;
    code = 'i';
    r = (*f)(I_WITH_DOT_ABOVE, &code, 1, arg);
    if (r != 0) return r;
  }
  else {
#endif
    for (i = 0; i < numberof(CaseUnfold_11_Locale); i++) {
      p11 = &CaseUnfold_11_Locale[i];
      for (j = 0; j < OnigCodePointCount(p11->to.n); j++) {
	code = p11->from;
	r = (*f)(p11->to.code[j], &code, 1, arg);
	if (r != 0) return r;

	code = p11->to.code[j];
	r = (*f)(p11->from, &code, 1, arg);
	if (r != 0) return r;

	for (k = 0; k < j; k++) {
	  r = (*f)(p11->to.code[j], (OnigCodePoint* )(&p11->to.code[k]),
		   1, arg);
	  if (r != 0) return r;

	  r = (*f)(p11->to.code[k], (OnigCodePoint* )(&p11->to.code[j]),
		   1, arg);
	  if (r != 0) return r;
	}
      }
    }
#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
  }
#endif

  if ((flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0) {
    for (i = 0; i < numberof(CaseUnfold_12); i++) {
      for (j = 0; j < OnigCodePointCount(CaseUnfold_12[i].to.n); j++) {
	r = (*f)(CaseUnfold_12[i].to.code[j],
		 (OnigCodePoint* )CaseUnfold_12[i].from, 2, arg);
	if (r != 0) return r;

	for (k = 0; k < OnigCodePointCount(CaseUnfold_12[i].to.n); k++) {
	  if (k == j) continue;

	  r = (*f)(CaseUnfold_12[i].to.code[j],
		   (OnigCodePoint* )(&CaseUnfold_12[i].to.code[k]), 1, arg);
	  if (r != 0) return r;
	}
      }
    }

#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
    if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) == 0) {
#endif
      for (i = 0; i < numberof(CaseUnfold_12_Locale); i++) {
	for (j = 0; j < OnigCodePointCount(CaseUnfold_12_Locale[i].to.n); j++) {
	  r = (*f)(CaseUnfold_12_Locale[i].to.code[j],
		   (OnigCodePoint* )CaseUnfold_12_Locale[i].from, 2, arg);
	  if (r != 0) return r;

	  for (k = 0; k < OnigCodePointCount(CaseUnfold_12_Locale[i].to.n); k++) {
	    if (k == j) continue;

	    r = (*f)(CaseUnfold_12_Locale[i].to.code[j],
		     (OnigCodePoint* )(&CaseUnfold_12_Locale[i].to.code[k]),
		     1, arg);
	    if (r != 0) return r;
	  }
	}
      }
#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
    }
#endif

    for (i = 0; i < numberof(CaseUnfold_13); i++) {
      for (j = 0; j < OnigCodePointCount(CaseUnfold_13[i].to.n); j++) {
	r = (*f)(CaseUnfold_13[i].to.code[j],
		 (OnigCodePoint* )CaseUnfold_13[i].from, 3, arg);
	if (r != 0) return r;

	for (k = 0; k < OnigCodePointCount(CaseUnfold_13[i].to.n); k++) {
	  if (k == j) continue;

	  r = (*f)(CaseUnfold_13[i].to.code[j],
		   (OnigCodePoint* )(&CaseUnfold_13[i].to.code[k]), 1, arg);
	  if (r != 0) return r;
	}
      }
    }
  }

  return 0;
}