static int fts3FindFunctionMethod(
  sqlite3_vtab *pVtab,            /* Virtual table handle */
  int nArg,                       /* Number of SQL function arguments */
  const char *zName,              /* Name of SQL function */
  void (**pxFunc)(sqlite3_context*,int,sqlite3_value**), /* OUT: Result */
  void **ppArg                    /* Unused */
){
  struct Overloaded {
    const char *zName;
    void (*xFunc)(sqlite3_context*,int,sqlite3_value**);
  } aOverload[] = {
    { "snippet", fts3SnippetFunc },
    { "offsets", fts3OffsetsFunc },
    { "optimize", fts3OptimizeFunc },
    { "matchinfo", fts3MatchinfoFunc },
  };
  int i;                          /* Iterator variable */

  UNUSED_PARAMETER(pVtab);
  UNUSED_PARAMETER(nArg);
  UNUSED_PARAMETER(ppArg);

  for(i=0; i<SizeofArray(aOverload); i++){
    if( strcmp(zName, aOverload[i].zName)==0 ){
      *pxFunc = aOverload[i].xFunc;
      return 1;
    }
  }

  /* No function of the specified name was found. Return 0. */
  return 0;
}