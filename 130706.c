int sqlite3Fts3FirstFilter(
  sqlite3_int64 iDelta,           /* Varint that may be written to pOut */
  char *pList,                    /* Position list (no 0x00 term) */
  int nList,                      /* Size of pList in bytes */
  char *pOut                      /* Write output here */
){
  int nOut = 0;
  int bWritten = 0;               /* True once iDelta has been written */
  char *p = pList;
  char *pEnd = &pList[nList];

  if( *p!=0x01 ){
    if( *p==0x02 ){
      nOut += sqlite3Fts3PutVarint(&pOut[nOut], iDelta);
      pOut[nOut++] = 0x02;
      bWritten = 1;
    }
    fts3ColumnlistCopy(0, &p);
  }

  while( p<pEnd && *p==0x01 ){
    sqlite3_int64 iCol;
    p++;
    p += sqlite3Fts3GetVarint(p, &iCol);
    if( *p==0x02 ){
      if( bWritten==0 ){
        nOut += sqlite3Fts3PutVarint(&pOut[nOut], iDelta);
        bWritten = 1;
      }
      pOut[nOut++] = 0x01;
      nOut += sqlite3Fts3PutVarint(&pOut[nOut], iCol);
      pOut[nOut++] = 0x02;
    }
    fts3ColumnlistCopy(0, &p);
  }
  if( bWritten ){
    pOut[nOut++] = 0x00;
  }

  return nOut;
}