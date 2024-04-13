static void fts3ReversePoslist(char *pStart, char **ppPoslist){
  char *p = &(*ppPoslist)[-2];
  char c = 0;

  while( p>pStart && (c=*p--)==0 );
  while( p>pStart && (*p & 0x80) | c ){ 
    c = *p--; 
  }
  if( p>pStart ){ p = &p[2]; }
  while( *p++&0x80 );
  *ppPoslist = p;
}