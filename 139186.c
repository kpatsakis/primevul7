static int stmt_read_row_buffered(MYSQL_STMT *stmt, unsigned char **row)
{
  if (stmt->data_cursor)
  {
    *row= (uchar *) stmt->data_cursor->data;
    stmt->data_cursor= stmt->data_cursor->next;
    return 0;
  }
  *row= 0;
  return MYSQL_NO_DATA;
}