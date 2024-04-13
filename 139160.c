mysql_stmt_row_seek(MYSQL_STMT *stmt, MYSQL_ROW_OFFSET row)
{
  MYSQL_ROW_OFFSET offset= stmt->data_cursor;
  DBUG_ENTER("mysql_stmt_row_seek");

  stmt->data_cursor= row;
  DBUG_RETURN(offset);
}