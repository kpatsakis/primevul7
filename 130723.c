static int fts3EofMethod(sqlite3_vtab_cursor *pCursor){
  return ((Fts3Cursor *)pCursor)->isEof;
}