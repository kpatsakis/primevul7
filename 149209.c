scan_unsigned_hexadecimal_number(UChar** src, UChar* end, int minlen,
                                 int maxlen, OnigEncoding enc)
{
  OnigCodePoint c;
  unsigned int num, val;
  int n;
  UChar* p = *src;
  PFETCH_READY;

  num = 0;
  n = 0;
  while (! PEND && n < maxlen) {
    PFETCH(c);
    if (IS_CODE_XDIGIT_ASCII(enc, c)) {
      n++;
      val = (unsigned int )XDIGITVAL(enc,c);
      if ((INT_MAX_LIMIT - val) / 16UL < num)
        return ONIGERR_TOO_BIG_NUMBER; /* overflow */

      num = (num << 4) + XDIGITVAL(enc,c);
    }
    else {
      PUNFETCH;
      break;
    }
  }

  if (n < minlen)
    return ONIGERR_INVALID_CODE_POINT_VALUE;

  *src = p;
  return num;
}