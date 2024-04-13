mysql_stmt_init(MYSQL *mysql)
{
  MYSQL_STMT *stmt;
  DBUG_ENTER("mysql_stmt_init");

  if (!(stmt=
          (MYSQL_STMT *) my_malloc(sizeof (MYSQL_STMT),
                                   MYF(MY_WME | MY_ZEROFILL))) ||
      !(stmt->extension=
          (MYSQL_STMT_EXT *) my_malloc(sizeof (MYSQL_STMT_EXT),
                                       MYF(MY_WME | MY_ZEROFILL))))
  {
    set_mysql_error(mysql, CR_OUT_OF_MEMORY, unknown_sqlstate);
    my_free(stmt);
    DBUG_RETURN(NULL);
  }

  init_alloc_root(&stmt->mem_root, 2048, 2048);
  init_alloc_root(&stmt->result.alloc, 4096, 4096);
  stmt->result.alloc.min_malloc= sizeof(MYSQL_ROWS);
  mysql->stmts= list_add(mysql->stmts, &stmt->list);
  stmt->list.data= stmt;
  stmt->state= MYSQL_STMT_INIT_DONE;
  stmt->mysql= mysql;
  stmt->read_row_func= stmt_read_row_no_result_set;
  stmt->prefetch_rows= DEFAULT_PREFETCH_ROWS;
  strmov(stmt->sqlstate, not_error_sqlstate);
  /* The rest of statement members was bzeroed inside malloc */

  init_alloc_root(&stmt->extension->fields_mem_root, 2048, 0);

  DBUG_RETURN(stmt);
}