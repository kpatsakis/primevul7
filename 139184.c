my_bool cli_read_prepare_result(MYSQL *mysql, MYSQL_STMT *stmt)
{
  uchar *pos;
  uint field_count, param_count;
  ulong packet_length;
  MYSQL_DATA *fields_data;
  DBUG_ENTER("cli_read_prepare_result");

  if ((packet_length= cli_safe_read(mysql)) == packet_error)
    DBUG_RETURN(1);
  mysql->warning_count= 0;

  pos= (uchar*) mysql->net.read_pos;
  stmt->stmt_id= uint4korr(pos+1); pos+= 5;
  /* Number of columns in result set */
  field_count=   uint2korr(pos);   pos+= 2;
  /* Number of placeholders in the statement */
  param_count=   uint2korr(pos);   pos+= 2;
  if (packet_length >= 12)
    mysql->warning_count= uint2korr(pos+1);

  if (param_count != 0)
  {
    MYSQL_DATA *param_data;

    /* skip parameters data: we don't support it yet */
    if (!(param_data= (*mysql->methods->read_rows)(mysql, (MYSQL_FIELD*)0, 7)))
      DBUG_RETURN(1);
    free_rows(param_data);
  }

  if (field_count != 0)
  {
    if (!(mysql->server_status & SERVER_STATUS_AUTOCOMMIT))
      mysql->server_status|= SERVER_STATUS_IN_TRANS;

    if (!(fields_data= (*mysql->methods->read_rows)(mysql,(MYSQL_FIELD*)0,7)))
      DBUG_RETURN(1);
    if (!(stmt->fields= unpack_fields(mysql, fields_data,&stmt->mem_root,
				      field_count,0,
				      mysql->server_capabilities)))
      DBUG_RETURN(1);
  }
  stmt->field_count=  field_count;
  stmt->param_count=  (ulong) param_count;
  DBUG_PRINT("exit",("field_count: %u  param_count: %u  warning_count: %u",
                     field_count, param_count, (uint) mysql->warning_count));

  DBUG_RETURN(0);
}