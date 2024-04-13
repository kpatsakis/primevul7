static int stmt_fetch_row(MYSQL_STMT *stmt, uchar *row)
{
  MYSQL_BIND  *my_bind, *end;
  MYSQL_FIELD *field;
  uchar *null_ptr, bit;
  int truncation_count= 0;
  /*
    Precondition: if stmt->field_count is zero or row is NULL, read_row_*
    function must return no data.
  */
  DBUG_ASSERT(stmt->field_count);
  DBUG_ASSERT(row);

  if (!stmt->bind_result_done)
  {
    /* If output parameters were not bound we should just return success */
    return 0;
  }

  null_ptr= row;
  row+= (stmt->field_count+9)/8;		/* skip null bits */
  bit= 4;					/* first 2 bits are reserved */

  /* Copy complete row to application buffers */
  for (my_bind= stmt->bind, end= my_bind + stmt->field_count,
         field= stmt->fields ;
       my_bind < end ;
       my_bind++, field++)
  {
    *my_bind->error= 0;
    if (*null_ptr & bit)
    {
      /*
        We should set both row_ptr and is_null to be able to see
        nulls in mysql_stmt_fetch_column. This is because is_null may point
        to user data which can be overwritten between mysql_stmt_fetch and
        mysql_stmt_fetch_column, and in this case nullness of column will be
        lost. See mysql_stmt_fetch_column for details.
      */
      my_bind->row_ptr= NULL;
      *my_bind->is_null= 1;
    }
    else
    {
      *my_bind->is_null= 0;
      my_bind->row_ptr= row;
      (*my_bind->fetch_result)(my_bind, field, &row);
      truncation_count+= *my_bind->error;
    }
    if (!((bit<<=1) & 255))
    {
      bit= 1;					/* To next uchar */
      null_ptr++;
    }
  }
  if (truncation_count && (stmt->bind_result_done & REPORT_DATA_TRUNCATION))
    return MYSQL_DATA_TRUNCATED;
  return 0;
}