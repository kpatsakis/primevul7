my_bool STDCALL mysql_stmt_bind_result(MYSQL_STMT *stmt, MYSQL_BIND *my_bind)
{
  MYSQL_BIND *param, *end;
  MYSQL_FIELD *field;
  ulong       bind_count= stmt->field_count;
  uint        param_count= 0;
  DBUG_ENTER("mysql_stmt_bind_result");
  DBUG_PRINT("enter",("field_count: %lu", bind_count));

  if (!bind_count)
  {
    int errorcode= (int) stmt->state < (int) MYSQL_STMT_PREPARE_DONE ?
                   CR_NO_PREPARE_STMT : CR_NO_STMT_METADATA;
    set_stmt_error(stmt, errorcode, unknown_sqlstate, NULL);
    DBUG_RETURN(1);
  }

  /*
    We only need to check that stmt->field_count - if it is not null
    stmt->bind was initialized in mysql_stmt_prepare
    stmt->bind overlaps with bind if mysql_stmt_bind_param
    is called from mysql_stmt_store_result.
  */

  if (stmt->bind != my_bind)
    memcpy((char*) stmt->bind, (char*) my_bind,
           sizeof(MYSQL_BIND) * bind_count);

  for (param= stmt->bind, end= param + bind_count, field= stmt->fields ;
       param < end ;
       param++, field++)
  {
    DBUG_PRINT("info",("buffer_type: %u  field_type: %u",
                       (uint) param->buffer_type, (uint) field->type));
    /*
      Set param->is_null to point to a dummy variable if it's not set.
      This is to make the execute code easier
    */
    if (!param->is_null)
      param->is_null= &param->is_null_value;

    if (!param->length)
      param->length= &param->length_value;

    if (!param->error)
      param->error= &param->error_value;

    param->param_number= param_count++;
    param->offset= 0;

    if (setup_one_fetch_function(param, field))
    {
      strmov(stmt->sqlstate, unknown_sqlstate);
      sprintf(stmt->last_error,
              ER(stmt->last_errno= CR_UNSUPPORTED_PARAM_TYPE),
              field->type, param_count);
      DBUG_RETURN(1);
    }
  }
  stmt->bind_result_done= BIND_RESULT_DONE;
  if (stmt->mysql->options.report_data_truncation)
    stmt->bind_result_done|= REPORT_DATA_TRUNCATION;

  DBUG_RETURN(0);
}