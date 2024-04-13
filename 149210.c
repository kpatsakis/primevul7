add_code_range_to_buf(BBuf** pbuf, OnigCodePoint from, OnigCodePoint to)
{
  int r, inc_n, pos;
  int low, high, bound, x;
  OnigCodePoint n, *data;
  BBuf* bbuf;

  if (from > to) {
    n = from; from = to; to = n;
  }

  if (IS_NULL(*pbuf)) {
    r = new_code_range(pbuf);
    if (r != 0) return r;
    bbuf = *pbuf;
    n = 0;
  }
  else {
    bbuf = *pbuf;
    GET_CODE_POINT(n, bbuf->p);
  }
  data = (OnigCodePoint* )(bbuf->p);
  data++;

  for (low = 0, bound = n; low < bound; ) {
    x = (low + bound) >> 1;
    if (from > data[x*2 + 1])
      low = x + 1;
    else
      bound = x;
  }

  high = (to == ~((OnigCodePoint )0)) ? n : low;
  for (bound = n; high < bound; ) {
    x = (high + bound) >> 1;
    if (to + 1 >= data[x*2])
      high = x + 1;
    else
      bound = x;
  }

  inc_n = low + 1 - high;
  if (n + inc_n > ONIG_MAX_MULTI_BYTE_RANGES_NUM)
    return ONIGERR_TOO_MANY_MULTI_BYTE_RANGES;

  if (inc_n != 1) {
    if (from > data[low*2])
      from = data[low*2];
    if (to < data[(high - 1)*2 + 1])
      to = data[(high - 1)*2 + 1];
  }

  if (inc_n != 0 && (OnigCodePoint )high < n) {
    int from_pos = SIZE_CODE_POINT * (1 + high * 2);
    int to_pos   = SIZE_CODE_POINT * (1 + (low + 1) * 2);
    int size = (n - high) * 2 * SIZE_CODE_POINT;

    if (inc_n > 0) {
      BB_MOVE_RIGHT(bbuf, from_pos, to_pos, size);
    }
    else {
      BB_MOVE_LEFT_REDUCE(bbuf, from_pos, to_pos);
    }
  }

  pos = SIZE_CODE_POINT * (1 + low * 2);
  BB_ENSURE_SIZE(bbuf, pos + SIZE_CODE_POINT * 2);
  BB_WRITE_CODE_POINT(bbuf, pos, from);
  BB_WRITE_CODE_POINT(bbuf, pos + SIZE_CODE_POINT, to);
  n += inc_n;
  BB_WRITE_CODE_POINT(bbuf, 0, n);

  return 0;
}