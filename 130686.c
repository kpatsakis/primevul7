static void fts3DeclareVtab(int *pRc, Fts3Table *p){
  if( *pRc==SQLITE_OK ){
    int i;                        /* Iterator variable */
    int rc;                       /* Return code */
    char *zSql;                   /* SQL statement passed to declare_vtab() */
    char *zCols;                  /* List of user defined columns */
    const char *zLanguageid;

    zLanguageid = (p->zLanguageid ? p->zLanguageid : "__langid");
    sqlite3_vtab_config(p->db, SQLITE_VTAB_CONSTRAINT_SUPPORT, 1);

    /* Create a list of user columns for the virtual table */
    zCols = sqlite3_mprintf("%Q, ", p->azColumn[0]);
    for(i=1; zCols && i<p->nColumn; i++){
      zCols = sqlite3_mprintf("%z%Q, ", zCols, p->azColumn[i]);
    }

    /* Create the whole "CREATE TABLE" statement to pass to SQLite */
    zSql = sqlite3_mprintf(
        "CREATE TABLE x(%s %Q HIDDEN, docid HIDDEN, %Q HIDDEN)", 
        zCols, p->zName, zLanguageid
    );
    if( !zCols || !zSql ){
      rc = SQLITE_NOMEM;
    }else{
      rc = sqlite3_declare_vtab(p->db, zSql);
    }

    sqlite3_free(zSql);
    sqlite3_free(zCols);
    *pRc = rc;
  }
}