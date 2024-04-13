static int stmt_read_row_unbuffered(MYSQL_STMT *stmt, unsigned char **row)
{
  int rc= 1;
  MYSQL *mysql= stmt->mysql;
  /*
    This function won't be called if stmt->field_count is zero
    or execution wasn't done: this is ensured by mysql_stmt_execute.
  */
  if (!mysql)
  {
    set_stmt_error(stmt, CR_SERVER_LOST, unknown_sqlstate, NULL);
    return 1;
  }
  if (mysql->status != MYSQL_STATUS_STATEMENT_GET_RESULT)
  {
    set_stmt_error(stmt, stmt->unbuffered_fetch_cancelled ?
                   CR_FETCH_CANCELED : CR_COMMANDS_OUT_OF_SYNC,
                   unknown_sqlstate, NULL);
    goto error;
  }
  if ((*mysql->methods->unbuffered_fetch)(mysql, (char**) row))
  {
    set_stmt_errmsg(stmt, &mysql->net);
    /*
      If there was an error, there are no more pending rows:
      reset statement status to not hang up in following
      mysql_stmt_close (it will try to flush result set before
      closing the statement).
    */
    mysql->status= MYSQL_STATUS_READY;
    goto error;
  }
  if (!*row)
  {
    mysql->status= MYSQL_STATUS_READY;
    rc= MYSQL_NO_DATA;
    goto error;
  }
  return 0;
error:
  if (mysql->unbuffered_fetch_owner == &stmt->unbuffered_fetch_cancelled)
    mysql->unbuffered_fetch_owner= 0;
  return rc;
}