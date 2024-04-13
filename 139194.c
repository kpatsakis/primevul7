mysql_stmt_param_metadata(MYSQL_STMT *stmt)
{
  DBUG_ENTER("mysql_stmt_param_metadata");

  if (!stmt->param_count)
    DBUG_RETURN(0);

  /*
    TODO: Fix this when server sends the information.
    Till then keep a dummy prototype.
  */
  DBUG_RETURN(0); 
}