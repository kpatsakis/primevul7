int STDCALL mysql_stmt_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *my_bind,
                                    uint column, ulong offset)
{
  MYSQL_BIND *param= stmt->bind+column;
  DBUG_ENTER("mysql_stmt_fetch_column");

  if ((int) stmt->state < (int) MYSQL_STMT_FETCH_DONE)
  {
    set_stmt_error(stmt, CR_NO_DATA, unknown_sqlstate, NULL);
    DBUG_RETURN(1);
  }
  if (column >= stmt->field_count)
  {
    set_stmt_error(stmt, CR_INVALID_PARAMETER_NO, unknown_sqlstate, NULL);
    DBUG_RETURN(1);
  }

  if (!my_bind->error)
    my_bind->error= &my_bind->error_value;
  *my_bind->error= 0;
  if (param->row_ptr)
  {
    MYSQL_FIELD *field= stmt->fields+column;
    uchar *row= param->row_ptr;
    my_bind->offset= offset;
    if (my_bind->is_null)
      *my_bind->is_null= 0;
    if (my_bind->length) /* Set the length if non char/binary types */
      *my_bind->length= *param->length;
    else
      my_bind->length= &param->length_value;       /* Needed for fetch_result() */
    fetch_result_with_conversion(my_bind, field, &row);
  }
  else
  {
    if (my_bind->is_null)
      *my_bind->is_null= 1;
  }
  DBUG_RETURN(0);
}