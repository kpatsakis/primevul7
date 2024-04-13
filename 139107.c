void set_stmt_errmsg(MYSQL_STMT *stmt, NET *net)
{
  DBUG_ENTER("set_stmt_errmsg");
  DBUG_PRINT("enter", ("error: %d/%s '%s'",
                       net->last_errno,
                       net->sqlstate,
                       net->last_error));
  DBUG_ASSERT(stmt != 0);

  stmt->last_errno= net->last_errno;
  if (net->last_error && net->last_error[0])
    strmov(stmt->last_error, net->last_error);
  strmov(stmt->sqlstate, net->sqlstate);

  DBUG_VOID_RETURN;
}