scan_unsigned_hexadecimal_number(UChar** src, UChar* end, int minlen,
				 int maxlen, OnigEncoding enc)
{
  OnigCodePoint c;
  unsigned int num, val;
  int restlen;
  UChar* p = *src;
  PFETCH_READY;

  restlen = maxlen - minlen;
  num = 0;
  while (!PEND && maxlen-- != 0) {
    PFETCH(c);
    if (ONIGENC_IS_CODE_XDIGIT(enc, c)) {
      val = (unsigned int )XDIGITVAL(enc,c);
      if ((INT_MAX_LIMIT - val) / 16UL < num)
	return -1;  /* overflow */

      num = (num << 4) + XDIGITVAL(enc,c);
    }
    else {
      PUNFETCH;
      maxlen++;
      break;
    }
  }
  if (maxlen > restlen)
    return -2;  /* not enough digits */
  *src = p;
  return num;
}