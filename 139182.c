int STDCALL mysql_stmt_store_result(MYSQL_STMT *stmt)
{
  MYSQL *mysql= stmt->mysql;
  MYSQL_DATA *result= &stmt->result;
  DBUG_ENTER("mysql_stmt_store_result");

  if (!mysql)
  {
    /* mysql can be reset in mysql_close called from mysql_reconnect */
    set_stmt_error(stmt, CR_SERVER_LOST, unknown_sqlstate, NULL);
    DBUG_RETURN(1);
  }

  if (!stmt->field_count)
    DBUG_RETURN(0);

  if ((int) stmt->state < (int) MYSQL_STMT_EXECUTE_DONE)
  {
    set_stmt_error(stmt, CR_COMMANDS_OUT_OF_SYNC, unknown_sqlstate, NULL);
    DBUG_RETURN(1);
  }

  if (stmt->last_errno)
  {
    /* An attempt to use an invalid statement handle. */
    DBUG_RETURN(1);
  }

  if (mysql->status == MYSQL_STATUS_READY &&
      stmt->server_status & SERVER_STATUS_CURSOR_EXISTS)
  {
    /*
      Server side cursor exist, tell server to start sending the rows
    */
    NET *net= &mysql->net;
    uchar buff[4 /* statement id */ +
               4 /* number of rows to fetch */];

    /* Send row request to the server */
    int4store(buff, stmt->stmt_id);
    int4store(buff + 4, (int)~0); /* number of rows to fetch */
    if (cli_advanced_command(mysql, COM_STMT_FETCH, buff, sizeof(buff),
                             (uchar*) 0, 0, 1, stmt))
    {
      /* 
        Don't set stmt error if stmt->mysql is NULL, as the error in this case 
        has already been set by mysql_prune_stmt_list(). 
      */
      if (stmt->mysql)
        set_stmt_errmsg(stmt, net);
      DBUG_RETURN(1);
    }
  }
  else if (mysql->status != MYSQL_STATUS_STATEMENT_GET_RESULT)
  {
    set_stmt_error(stmt, CR_COMMANDS_OUT_OF_SYNC, unknown_sqlstate, NULL);
    DBUG_RETURN(1);
  }

  if (stmt->update_max_length && !stmt->bind_result_done)
  {
    /*
      We must initalize the bind structure to be able to calculate
      max_length
    */
    MYSQL_BIND  *my_bind, *end;
    MYSQL_FIELD *field;
    bzero((char*) stmt->bind, sizeof(*stmt->bind)* stmt->field_count);

    for (my_bind= stmt->bind, end= my_bind + stmt->field_count,
           field= stmt->fields;
	 my_bind < end ;
	 my_bind++, field++)
    {
      my_bind->buffer_type= MYSQL_TYPE_NULL;
      my_bind->buffer_length=1;
    }

    if (mysql_stmt_bind_result(stmt, stmt->bind))
      DBUG_RETURN(1);
    stmt->bind_result_done= 0;			/* No normal bind done */
  }

  if ((*mysql->methods->read_binary_rows)(stmt))
  {
    free_root(&result->alloc, MYF(MY_KEEP_PREALLOC));
    result->data= NULL;
    result->rows= 0;
    mysql->status= MYSQL_STATUS_READY;
    DBUG_RETURN(1);
  }

  /* Assert that if there was a cursor, all rows have been fetched */
  DBUG_ASSERT(mysql->status != MYSQL_STATUS_READY ||
              (mysql->server_status & SERVER_STATUS_LAST_ROW_SENT));

  if (stmt->update_max_length)
  {
    MYSQL_ROWS *cur= result->data;
    for(; cur; cur=cur->next)
      stmt_update_metadata(stmt, cur);
  }

  stmt->data_cursor= result->data;
  mysql->affected_rows= stmt->affected_rows= result->rows;
  stmt->read_row_func= stmt_read_row_buffered;
  mysql->unbuffered_fetch_owner= 0;             /* set in stmt_execute */
  mysql->status= MYSQL_STATUS_READY;		/* server is ready */
  DBUG_RETURN(0); /* Data buffered, must be fetched with mysql_stmt_fetch() */
}