static my_bool setup_one_fetch_function(MYSQL_BIND *param, MYSQL_FIELD *field)
{
  DBUG_ENTER("setup_one_fetch_function");

  /* Setup data copy functions for the different supported types */
  switch (param->buffer_type) {
  case MYSQL_TYPE_NULL: /* for dummy binds */
    /*
      It's not binary compatible with anything the server can return:
      no need to setup fetch_result, as it'll be reset anyway
    */
    *param->length= 0;
    break;
  case MYSQL_TYPE_TINY:
    param->fetch_result= fetch_result_tinyint;
    *param->length= 1;
    break;
  case MYSQL_TYPE_SHORT:
  case MYSQL_TYPE_YEAR:
    param->fetch_result= fetch_result_short;
    *param->length= 2;
    break;
  case MYSQL_TYPE_INT24:
  case MYSQL_TYPE_LONG:
    param->fetch_result= fetch_result_int32;
    *param->length= 4;
    break;
  case MYSQL_TYPE_LONGLONG:
    param->fetch_result= fetch_result_int64;
    *param->length= 8;
    break;
  case MYSQL_TYPE_FLOAT:
    param->fetch_result= fetch_result_float;
    *param->length= 4;
    break;
  case MYSQL_TYPE_DOUBLE:
    param->fetch_result= fetch_result_double;
    *param->length= 8;
    break;
  case MYSQL_TYPE_TIME:
    param->fetch_result= fetch_result_time;
    *param->length= sizeof(MYSQL_TIME);
    break;
  case MYSQL_TYPE_DATE:
    param->fetch_result= fetch_result_date;
    *param->length= sizeof(MYSQL_TIME);
    break;
  case MYSQL_TYPE_DATETIME:
  case MYSQL_TYPE_TIMESTAMP:
    param->fetch_result= fetch_result_datetime;
    *param->length= sizeof(MYSQL_TIME);
    break;
  case MYSQL_TYPE_TINY_BLOB:
  case MYSQL_TYPE_MEDIUM_BLOB:
  case MYSQL_TYPE_LONG_BLOB:
  case MYSQL_TYPE_BLOB:
  case MYSQL_TYPE_BIT:
    DBUG_ASSERT(param->buffer_length != 0);
    param->fetch_result= fetch_result_bin;
    break;
  case MYSQL_TYPE_VAR_STRING:
  case MYSQL_TYPE_STRING:
  case MYSQL_TYPE_DECIMAL:
  case MYSQL_TYPE_NEWDECIMAL:
  case MYSQL_TYPE_NEWDATE:
    DBUG_ASSERT(param->buffer_length != 0);
    param->fetch_result= fetch_result_str;
    break;
  default:
    DBUG_PRINT("error", ("Unknown param->buffer_type: %u",
                         (uint) param->buffer_type));
    DBUG_RETURN(TRUE);
  }
  if (! is_binary_compatible(param->buffer_type, field->type))
    param->fetch_result= fetch_result_with_conversion;

  /* Setup skip_result functions (to calculate max_length) */
  param->skip_result= skip_result_fixed;
  switch (field->type) {
  case MYSQL_TYPE_NULL: /* for dummy binds */
    param->pack_length= 0;
    field->max_length= 0;
    break;
  case MYSQL_TYPE_TINY:
    param->pack_length= 1;
    field->max_length= 4;                     /* as in '-127' */
    break;
  case MYSQL_TYPE_YEAR:
  case MYSQL_TYPE_SHORT:
    param->pack_length= 2;
    field->max_length= 6;                     /* as in '-32767' */
    break;
  case MYSQL_TYPE_INT24:
    field->max_length= 9;  /* as in '16777216' or in '-8388607' */
    param->pack_length= 4;
    break;
  case MYSQL_TYPE_LONG:
    field->max_length= 11;                    /* '-2147483647' */
    param->pack_length= 4;
    break;
  case MYSQL_TYPE_LONGLONG:
    field->max_length= 21;                    /* '18446744073709551616' */
    param->pack_length= 8;
    break;
  case MYSQL_TYPE_FLOAT:
    param->pack_length= 4;
    field->max_length= MAX_DOUBLE_STRING_REP_LENGTH;
    break;
  case MYSQL_TYPE_DOUBLE:
    param->pack_length= 8;
    field->max_length= MAX_DOUBLE_STRING_REP_LENGTH;
    break;
  case MYSQL_TYPE_TIME:
    field->max_length= 15;                    /* 19:23:48.123456 */
    param->skip_result= skip_result_with_length;
    break;
  case MYSQL_TYPE_DATE:
    field->max_length= 10;                    /* 2003-11-11 */
    param->skip_result= skip_result_with_length;
    break;
  case MYSQL_TYPE_DATETIME:
  case MYSQL_TYPE_TIMESTAMP:
    param->skip_result= skip_result_with_length;
    field->max_length= MAX_DATE_STRING_REP_LENGTH;
    break;
  case MYSQL_TYPE_DECIMAL:
  case MYSQL_TYPE_NEWDECIMAL:
  case MYSQL_TYPE_ENUM:
  case MYSQL_TYPE_SET:
  case MYSQL_TYPE_GEOMETRY:
  case MYSQL_TYPE_TINY_BLOB:
  case MYSQL_TYPE_MEDIUM_BLOB:
  case MYSQL_TYPE_LONG_BLOB:
  case MYSQL_TYPE_BLOB:
  case MYSQL_TYPE_VAR_STRING:
  case MYSQL_TYPE_STRING:
  case MYSQL_TYPE_BIT:
  case MYSQL_TYPE_NEWDATE:
    param->skip_result= skip_result_string;
    break;
  default:
    DBUG_PRINT("error", ("Unknown field->type: %u", (uint) field->type));
    DBUG_RETURN(TRUE);
  }
  DBUG_RETURN(FALSE);
}