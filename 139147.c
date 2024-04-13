stmt_read_row_from_cursor(MYSQL_STMT *stmt, unsigned char **row)
{
  if (stmt->data_cursor)
    return stmt_read_row_buffered(stmt, row);
  if (stmt->server_status & SERVER_STATUS_LAST_ROW_SENT)
    stmt->server_status &= ~SERVER_STATUS_LAST_ROW_SENT;
  else
  {
    MYSQL *mysql= stmt->mysql;
    NET *net= &mysql->net;
    MYSQL_DATA *result= &stmt->result;
    uchar buff[4 /* statement id */ +
               4 /* number of rows to fetch */];

    free_root(&result->alloc, MYF(MY_KEEP_PREALLOC));
    result->data= NULL;
    result->rows= 0;
    /* Send row request to the server */
    int4store(buff, stmt->stmt_id);
    int4store(buff + 4, stmt->prefetch_rows); /* number of rows to fetch */
    if ((*mysql->methods->advanced_command)(mysql, COM_STMT_FETCH,
                                            buff, sizeof(buff), (uchar*) 0, 0,
                                            1, stmt))
    {
      /* 
        Don't set stmt error if stmt->mysql is NULL, as the error in this case 
        has already been set by mysql_prune_stmt_list(). 
      */
      if (stmt->mysql)
        set_stmt_errmsg(stmt, net);
      return 1;
    }
    if ((*mysql->methods->read_rows_from_cursor)(stmt))
      return 1;
    stmt->server_status= mysql->server_status;

    stmt->data_cursor= result->data;
    return stmt_read_row_buffered(stmt, row);
  }
  *row= 0;
  return MYSQL_NO_DATA;
}