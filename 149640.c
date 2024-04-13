case_map(OnigCaseFoldType* flagP, const OnigUChar** pp,
	 const OnigUChar* end, OnigUChar* to, OnigUChar* to_end,
	 const struct OnigEncodingTypeST* enc)
{
  OnigCodePoint code;
  OnigUChar *to_start = to;
  OnigCaseFoldType flags = *flagP;

  while (*pp < end && to < to_end) {
    code = *(*pp)++;
    if ((EncCP1251_CtypeTable[code] & BIT_CTYPE_UPPER)
	     && (flags & (ONIGENC_CASE_DOWNCASE | ONIGENC_CASE_FOLD))) {
      flags |= ONIGENC_CASE_MODIFIED;
      code = ENC_CP1251_TO_LOWER_CASE(code);
    }
    else if (code == 0xB5)
      ;
    else if ((EncCP1251_CtypeTable[code]&BIT_CTYPE_LOWER)
	     && (flags & ONIGENC_CASE_UPCASE)) {
      flags |= ONIGENC_CASE_MODIFIED;
      if ((0x61 <= code && code <= 0x7A) || (0xE0 <= code && code <= 0xFF))
	code -= 0x20;
      else if (code == 0xA2 || code == 0xB3 || code == 0xBE)
	code -= 0x01;
      else if (code == 0x83)
	code = 0x81;
      else if (code == 0xBC)
	code = 0xA3;
      else if (code == 0xB4)
	code = 0xA5;
      else
	code -= 0x10;
    }
    *to++ = code;
    if (flags & ONIGENC_CASE_TITLECASE)  /* switch from titlecase to lowercase for capitalize */
      flags ^= (ONIGENC_CASE_UPCASE | ONIGENC_CASE_DOWNCASE | ONIGENC_CASE_TITLECASE);
  }
  *flagP = flags;
  return (int )(to - to_start);
}