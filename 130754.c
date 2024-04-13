static int fts3PutColNumber(char **pp, int iCol){
  int n = 0;                      /* Number of bytes written */
  if( iCol ){
    char *p = *pp;                /* Output pointer */
    n = 1 + sqlite3Fts3PutVarint(&p[1], iCol);
    *p = 0x01;
    *pp = &p[n];
  }
  return n;
}