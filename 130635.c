static void fts3PutDeltaVarint(
  char **pp,                      /* IN/OUT: Output pointer */
  sqlite3_int64 *piPrev,          /* IN/OUT: Previous value written to list */
  sqlite3_int64 iVal              /* Write this value to the list */
){
  assert( iVal-*piPrev > 0 || (*piPrev==0 && iVal==0) );
  *pp += sqlite3Fts3PutVarint(*pp, iVal-*piPrev);
  *piPrev = iVal;
}