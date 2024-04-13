is_onechar_cclass(CClassNode* cc, OnigCodePoint* code)
{
  const OnigCodePoint not_found = ONIG_LAST_CODE_POINT;
  OnigCodePoint c = not_found;
  int i;
  BBuf *bbuf = cc->mbuf;

  if (IS_NCCLASS_NOT(cc)) return 0;

  /* check bbuf */
  if (IS_NOT_NULL(bbuf)) {
    OnigCodePoint n, *data;
    GET_CODE_POINT(n, bbuf->p);
    data = (OnigCodePoint* )(bbuf->p) + 1;
    if ((n == 1) && (data[0] == data[1])) {
      /* only one char found in the bbuf, save the code point. */
      c = data[0];
      if (((c < SINGLE_BYTE_SIZE) && BITSET_AT(cc->bs, c))) {
	/* skip if c is included in the bitset */
	c = not_found;
      }
    }
    else {
      return 0;  /* the bbuf contains multiple chars */
    }
  }

  /* check bitset */
  for (i = 0; i < BITSET_SIZE; i++) {
    Bits b1 = cc->bs[i];
    if (b1 != 0) {
      if (((b1 & (b1 - 1)) == 0) && (c == not_found)) {
	c = BITS_IN_ROOM * i + countbits(b1 - 1);
      } else {
	return 0;  /* the character class contains multiple chars */
      }
    }
  }

  if (c != not_found) {
    *code = c;
    return 1;
  }

  /* the character class contains no char. */
  return 0;
}