static my_bool reset_stmt_handle(MYSQL_STMT *stmt, uint flags)
{
  /* If statement hasn't been prepared there is nothing to reset */
  if ((int) stmt->state > (int) MYSQL_STMT_INIT_DONE)
  {
    MYSQL *mysql= stmt->mysql;
    MYSQL_DATA *result= &stmt->result;

    /*
      Reset stored result set if so was requested or it's a part
      of cursor fetch.
    */
    if (flags & RESET_STORE_RESULT)
    {
      /* Result buffered */
      free_root(&result->alloc, MYF(MY_KEEP_PREALLOC));
      result->data= NULL;
      result->rows= 0;
      stmt->data_cursor= NULL;
    }
    if (flags & RESET_LONG_DATA)
    {
      MYSQL_BIND *param= stmt->params, *param_end= param + stmt->param_count;
      /* Clear long_data_used flags */
      for (; param < param_end; param++)
        param->long_data_used= 0;
    }
    stmt->read_row_func= stmt_read_row_no_result_set;
    if (mysql)
    {
      if ((int) stmt->state > (int) MYSQL_STMT_PREPARE_DONE)
      {
        if (mysql->unbuffered_fetch_owner == &stmt->unbuffered_fetch_cancelled)
          mysql->unbuffered_fetch_owner= 0;
        if (stmt->field_count && mysql->status != MYSQL_STATUS_READY)
        {
          /* There is a result set and it belongs to this statement */
          (*mysql->methods->flush_use_result)(mysql, FALSE);
          if (mysql->unbuffered_fetch_owner)
            *mysql->unbuffered_fetch_owner= TRUE;
          mysql->status= MYSQL_STATUS_READY;
        }
      }
      if (flags & RESET_SERVER_SIDE)
      {
        /*
          Reset the server side statement and close the server side
          cursor if it exists.
        */
        uchar buff[MYSQL_STMT_HEADER]; /* packet header: 4 bytes for stmt id */
        int4store(buff, stmt->stmt_id);
        if ((*mysql->methods->advanced_command)(mysql, COM_STMT_RESET, buff,
                                                sizeof(buff), 0, 0, 0, stmt))
        {
          set_stmt_errmsg(stmt, &mysql->net);
          stmt->state= MYSQL_STMT_INIT_DONE;
          return 1;
        }
      }
    }
    if (flags & RESET_CLEAR_ERROR)
      stmt_clear_error(stmt);
    stmt->state= MYSQL_STMT_PREPARE_DONE;
  }
  return 0;
}