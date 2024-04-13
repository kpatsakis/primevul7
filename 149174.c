new_code_range(BBuf** pbuf)
{
#define INIT_MULTI_BYTE_RANGE_SIZE  (SIZE_CODE_POINT * 5)
  int r;
  OnigCodePoint n;
  BBuf* bbuf;

  bbuf = *pbuf = (BBuf* )xmalloc(sizeof(BBuf));
  CHECK_NULL_RETURN_MEMERR(bbuf);
  r = BB_INIT(bbuf, INIT_MULTI_BYTE_RANGE_SIZE);
  if (r != 0) {
    xfree(bbuf);
    *pbuf = 0;
    return r;
  }

  n = 0;
  BB_WRITE_CODE_POINT(bbuf, 0, n);
  return 0;
}