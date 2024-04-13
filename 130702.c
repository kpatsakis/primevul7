static int fts3ColumnMethod(
  sqlite3_vtab_cursor *pCursor,   /* Cursor to retrieve value from */
  sqlite3_context *pCtx,          /* Context for sqlite3_result_xxx() calls */
  int iCol                        /* Index of column to read value from */
){
  int rc = SQLITE_OK;             /* Return Code */
  Fts3Cursor *pCsr = (Fts3Cursor *) pCursor;
  Fts3Table *p = (Fts3Table *)pCursor->pVtab;

  /* The column value supplied by SQLite must be in range. */
  assert( iCol>=0 && iCol<=p->nColumn+2 );

  if( iCol==p->nColumn+1 ){
    /* This call is a request for the "docid" column. Since "docid" is an 
    ** alias for "rowid", use the xRowid() method to obtain the value.
    */
    sqlite3_result_int64(pCtx, pCsr->iPrevId);
  }else if( iCol==p->nColumn ){
    /* The extra column whose name is the same as the table.
    ** Return a blob which is a pointer to the cursor.  */
    sqlite3_result_blob(pCtx, &pCsr, sizeof(pCsr), SQLITE_TRANSIENT);
  }else if( iCol==p->nColumn+2 && pCsr->pExpr ){
    sqlite3_result_int64(pCtx, pCsr->iLangid);
  }else{
    /* The requested column is either a user column (one that contains 
    ** indexed data), or the language-id column.  */
    rc = fts3CursorSeek(0, pCsr);

    if( rc==SQLITE_OK ){
      if( iCol==p->nColumn+2 ){
        int iLangid = 0;
        if( p->zLanguageid ){
          iLangid = sqlite3_column_int(pCsr->pStmt, p->nColumn+1);
        }
        sqlite3_result_int(pCtx, iLangid);
      }else if( sqlite3_data_count(pCsr->pStmt)>(iCol+1) ){
        sqlite3_result_value(pCtx, sqlite3_column_value(pCsr->pStmt, iCol+1));
      }
    }
  }

  assert( ((Fts3Table *)pCsr->base.pVtab)->pSegments==0 );
  return rc;
}