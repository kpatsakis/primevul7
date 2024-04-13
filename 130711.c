static int fts3CursorSeekStmt(Fts3Cursor *pCsr, sqlite3_stmt **ppStmt){
  int rc = SQLITE_OK;
  if( pCsr->pStmt==0 ){
    Fts3Table *p = (Fts3Table *)pCsr->base.pVtab;
    char *zSql;
    zSql = sqlite3_mprintf("SELECT %s WHERE rowid = ?", p->zReadExprlist);
    if( !zSql ) return SQLITE_NOMEM;
    rc = sqlite3_prepare_v2(p->db, zSql, -1, &pCsr->pStmt, 0);
    sqlite3_free(zSql);
  }
  *ppStmt = pCsr->pStmt;
  return rc;
}