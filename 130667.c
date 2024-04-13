int sqlite3Fts3GetVarint(const char *p, sqlite_int64 *v){
  const char *pStart = p;
  u32 a;
  u64 b;
  int shift;

  GETVARINT_INIT(a, p, 0,  0x00,     0x80, *v, 1);
  GETVARINT_STEP(a, p, 7,  0x7F,     0x4000, *v, 2);
  GETVARINT_STEP(a, p, 14, 0x3FFF,   0x200000, *v, 3);
  GETVARINT_STEP(a, p, 21, 0x1FFFFF, 0x10000000, *v, 4);
  b = (a & 0x0FFFFFFF );

  for(shift=28; shift<=63; shift+=7){
    u64 c = *p++;
    b += (c&0x7F) << shift;
    if( (c & 0x80)==0 ) break;
  }
  *v = b;
  return (int)(p - pStart);
}