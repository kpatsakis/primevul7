static int fts3RollbackMethod(sqlite3_vtab *pVtab){
  Fts3Table *p = (Fts3Table*)pVtab;
  sqlite3Fts3PendingTermsClear(p);
  assert( p->inTransaction!=0 );
  TESTONLY( p->inTransaction = 0 );
  TESTONLY( p->mxSavepoint = -1; );
  return SQLITE_OK;
}