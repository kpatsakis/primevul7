ulong STDCALL mysql_stmt_param_count(MYSQL_STMT * stmt)
{
  DBUG_ENTER("mysql_stmt_param_count");
  DBUG_RETURN(stmt->param_count);
}