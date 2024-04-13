mysql_stmt_data_seek(MYSQL_STMT *stmt, my_ulonglong row)
{
  MYSQL_ROWS *tmp= stmt->result.data;
  DBUG_ENTER("mysql_stmt_data_seek");
  DBUG_PRINT("enter",("row id to seek: %ld",(long) row));

  for (; tmp && row; --row, tmp= tmp->next)
    ;
  stmt->data_cursor= tmp;
  if (!row && tmp)
  {
       /*  Rewind the counter */
    stmt->read_row_func= stmt_read_row_buffered;
    stmt->state= MYSQL_STMT_EXECUTE_DONE;
  }
  DBUG_VOID_RETURN;
}