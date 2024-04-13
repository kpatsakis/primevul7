mysql_stmt_row_tell(MYSQL_STMT *stmt)
{
  DBUG_ENTER("mysql_stmt_row_tell");

  DBUG_RETURN(stmt->data_cursor);
}