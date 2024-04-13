static int fts3TermSegReaderCursor(
  Fts3Cursor *pCsr,               /* Virtual table cursor handle */
  const char *zTerm,              /* Term to query for */
  int nTerm,                      /* Size of zTerm in bytes */
  int isPrefix,                   /* True for a prefix search */
  Fts3MultiSegReader **ppSegcsr   /* OUT: Allocated seg-reader cursor */
){
  Fts3MultiSegReader *pSegcsr;    /* Object to allocate and return */
  int rc = SQLITE_NOMEM;          /* Return code */

  pSegcsr = sqlite3_malloc(sizeof(Fts3MultiSegReader));
  if( pSegcsr ){
    int i;
    int bFound = 0;               /* True once an index has been found */
    Fts3Table *p = (Fts3Table *)pCsr->base.pVtab;

    if( isPrefix ){
      for(i=1; bFound==0 && i<p->nIndex; i++){
        if( p->aIndex[i].nPrefix==nTerm ){
          bFound = 1;
          rc = sqlite3Fts3SegReaderCursor(p, pCsr->iLangid, 
              i, FTS3_SEGCURSOR_ALL, zTerm, nTerm, 0, 0, pSegcsr
          );
          pSegcsr->bLookup = 1;
        }
      }

      for(i=1; bFound==0 && i<p->nIndex; i++){
        if( p->aIndex[i].nPrefix==nTerm+1 ){
          bFound = 1;
          rc = sqlite3Fts3SegReaderCursor(p, pCsr->iLangid, 
              i, FTS3_SEGCURSOR_ALL, zTerm, nTerm, 1, 0, pSegcsr
          );
          if( rc==SQLITE_OK ){
            rc = fts3SegReaderCursorAddZero(
                p, pCsr->iLangid, zTerm, nTerm, pSegcsr
            );
          }
        }
      }
    }

    if( bFound==0 ){
      rc = sqlite3Fts3SegReaderCursor(p, pCsr->iLangid, 
          0, FTS3_SEGCURSOR_ALL, zTerm, nTerm, isPrefix, 0, pSegcsr
      );
      pSegcsr->bLookup = !isPrefix;
    }
  }

  *ppSegcsr = pSegcsr;
  return rc;
}