static int fts3EvalAverageDocsize(Fts3Cursor *pCsr, int *pnPage){
  if( pCsr->nRowAvg==0 ){
    /* The average document size, which is required to calculate the cost
    ** of each doclist, has not yet been determined. Read the required 
    ** data from the %_stat table to calculate it.
    **
    ** Entry 0 of the %_stat table is a blob containing (nCol+1) FTS3 
    ** varints, where nCol is the number of columns in the FTS3 table.
    ** The first varint is the number of documents currently stored in
    ** the table. The following nCol varints contain the total amount of
    ** data stored in all rows of each column of the table, from left
    ** to right.
    */
    int rc;
    Fts3Table *p = (Fts3Table*)pCsr->base.pVtab;
    sqlite3_stmt *pStmt;
    sqlite3_int64 nDoc = 0;
    sqlite3_int64 nByte = 0;
    const char *pEnd;
    const char *a;

    rc = sqlite3Fts3SelectDoctotal(p, &pStmt);
    if( rc!=SQLITE_OK ) return rc;
    a = sqlite3_column_blob(pStmt, 0);
    assert( a );

    pEnd = &a[sqlite3_column_bytes(pStmt, 0)];
    a += sqlite3Fts3GetVarint(a, &nDoc);
    while( a<pEnd ){
      a += sqlite3Fts3GetVarint(a, &nByte);
    }
    if( nDoc==0 || nByte==0 ){
      sqlite3_reset(pStmt);
      return FTS_CORRUPT_VTAB;
    }

    pCsr->nDoc = nDoc;
    pCsr->nRowAvg = (int)(((nByte / nDoc) + p->nPgsz) / p->nPgsz);
    assert( pCsr->nRowAvg>0 ); 
    rc = sqlite3_reset(pStmt);
    if( rc!=SQLITE_OK ) return rc;
  }

  *pnPage = pCsr->nRowAvg;
  return SQLITE_OK;
}