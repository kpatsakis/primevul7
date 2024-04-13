static int fts3SegReaderCursorAddZero(
  Fts3Table *p,                   /* FTS virtual table handle */
  int iLangid,
  const char *zTerm,              /* Term to scan doclist of */
  int nTerm,                      /* Number of bytes in zTerm */
  Fts3MultiSegReader *pCsr        /* Fts3MultiSegReader to modify */
){
  return fts3SegReaderCursor(p, 
      iLangid, 0, FTS3_SEGCURSOR_ALL, zTerm, nTerm, 0, 0,pCsr
  );
}