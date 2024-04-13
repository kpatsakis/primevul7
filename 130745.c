static int fts3DestroyMethod(sqlite3_vtab *pVtab){
  Fts3Table *p = (Fts3Table *)pVtab;
  int rc = SQLITE_OK;              /* Return code */
  const char *zDb = p->zDb;        /* Name of database (e.g. "main", "temp") */
  sqlite3 *db = p->db;             /* Database handle */

  /* Drop the shadow tables */
  if( p->zContentTbl==0 ){
    fts3DbExec(&rc, db, "DROP TABLE IF EXISTS %Q.'%q_content'", zDb, p->zName);
  }
  fts3DbExec(&rc, db, "DROP TABLE IF EXISTS %Q.'%q_segments'", zDb,p->zName);
  fts3DbExec(&rc, db, "DROP TABLE IF EXISTS %Q.'%q_segdir'", zDb, p->zName);
  fts3DbExec(&rc, db, "DROP TABLE IF EXISTS %Q.'%q_docsize'", zDb, p->zName);
  fts3DbExec(&rc, db, "DROP TABLE IF EXISTS %Q.'%q_stat'", zDb, p->zName);

  /* If everything has worked, invoke fts3DisconnectMethod() to free the
  ** memory associated with the Fts3Table structure and return SQLITE_OK.
  ** Otherwise, return an SQLite error code.
  */
  return (rc==SQLITE_OK ? fts3DisconnectMethod(pVtab) : rc);
}