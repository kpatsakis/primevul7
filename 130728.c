static int fts3SetHasStat(Fts3Table *p){
  int rc = SQLITE_OK;
  if( p->bHasStat==2 ){
    const char *zFmt ="SELECT 1 FROM %Q.sqlite_master WHERE tbl_name='%q_stat'";
    char *zSql = sqlite3_mprintf(zFmt, p->zDb, p->zName);
    if( zSql ){
      sqlite3_stmt *pStmt = 0;
      rc = sqlite3_prepare_v2(p->db, zSql, -1, &pStmt, 0);
      if( rc==SQLITE_OK ){
        int bHasStat = (sqlite3_step(pStmt)==SQLITE_ROW);
        rc = sqlite3_finalize(pStmt);
        if( rc==SQLITE_OK ) p->bHasStat = bHasStat;
      }
      sqlite3_free(zSql);
    }else{
      rc = SQLITE_NOMEM;
    }
  }
  return rc;
}