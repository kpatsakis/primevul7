static int fts3GobbleInt(const char **pp, int *pnOut){
  const char *p;                  /* Iterator pointer */
  int nInt = 0;                   /* Output value */

  for(p=*pp; p[0]>='0' && p[0]<='9'; p++){
    nInt = nInt * 10 + (p[0] - '0');
  }
  if( p==*pp ) return SQLITE_ERROR;
  *pnOut = nInt;
  *pp = p;
  return SQLITE_OK;
}