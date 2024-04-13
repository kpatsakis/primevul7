static void hashDestroy(void *p){
  Fts3Hash *pHash = (Fts3Hash *)p;
  sqlite3Fts3HashClear(pHash);
  sqlite3_free(pHash);
}