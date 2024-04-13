int sqlite3Fts3GetVarint32(const char *p, int *pi){
  u32 a;

#ifndef fts3GetVarint32
  GETVARINT_INIT(a, p, 0,  0x00,     0x80, *pi, 1);
#else
  a = (*p++);
  assert( a & 0x80 );
#endif

  GETVARINT_STEP(a, p, 7,  0x7F,     0x4000, *pi, 2);
  GETVARINT_STEP(a, p, 14, 0x3FFF,   0x200000, *pi, 3);
  GETVARINT_STEP(a, p, 21, 0x1FFFFF, 0x10000000, *pi, 4);
  a = (a & 0x0FFFFFFF );
  *pi = (int)(a | ((u32)(*p & 0x0F) << 28));
  return 5;
}