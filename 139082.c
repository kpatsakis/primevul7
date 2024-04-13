static void stmt_clear_error(MYSQL_STMT *stmt)
{
  if (stmt->last_errno)
  {
    stmt->last_errno= 0;
    stmt->last_error[0]= '\0';
    strmov(stmt->sqlstate, not_error_sqlstate);
  }
}