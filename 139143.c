static void fetch_result_with_conversion(MYSQL_BIND *param, MYSQL_FIELD *field,
                                         uchar **row)
{
  enum enum_field_types field_type= field->type;
  uint field_is_unsigned= field->flags & UNSIGNED_FLAG;

  switch (field_type) {
  case MYSQL_TYPE_TINY:
  {
    uchar value= **row;
    /* sic: we need to cast to 'signed char' as 'char' may be unsigned */
    longlong data= field_is_unsigned ? (longlong) value :
                                       (longlong) (signed char) value;
    fetch_long_with_conversion(param, field, data, 0);
    *row+= 1;
    break;
  }
  case MYSQL_TYPE_SHORT:
  case MYSQL_TYPE_YEAR:
  {
    short value= sint2korr(*row);
    longlong data= field_is_unsigned ? (longlong) (unsigned short) value :
                                       (longlong) value;
    fetch_long_with_conversion(param, field, data, 0);
    *row+= 2;
    break;
  }
  case MYSQL_TYPE_INT24: /* mediumint is sent as 4 bytes int */
  case MYSQL_TYPE_LONG:
  {
    int32 value= sint4korr(*row);
    longlong data= field_is_unsigned ? (longlong) (uint32) value :
                                       (longlong) value;
    fetch_long_with_conversion(param, field, data, 0);
    *row+= 4;
    break;
  }
  case MYSQL_TYPE_LONGLONG:
  {
    longlong value= (longlong)sint8korr(*row);
    fetch_long_with_conversion(param, field, value,
                               field->flags & UNSIGNED_FLAG);
    *row+= 8;
    break;
  }
  case MYSQL_TYPE_FLOAT:
  {
    float value;
    float4get(value,*row);
    fetch_float_with_conversion(param, field, value, MY_GCVT_ARG_FLOAT);
    *row+= 4;
    break;
  }
  case MYSQL_TYPE_DOUBLE:
  {
    double value;
    float8get(value,*row);
    fetch_float_with_conversion(param, field, value, MY_GCVT_ARG_DOUBLE);
    *row+= 8;
    break;
  }
  case MYSQL_TYPE_DATE:
  {
    MYSQL_TIME tm;

    read_binary_date(&tm, row);
    fetch_datetime_with_conversion(param, field, &tm);
    break;
  }
  case MYSQL_TYPE_TIME:
  {
    MYSQL_TIME tm;

    read_binary_time(&tm, row);
    fetch_datetime_with_conversion(param, field, &tm);
    break;
  }
  case MYSQL_TYPE_DATETIME:
  case MYSQL_TYPE_TIMESTAMP:
  {
    MYSQL_TIME tm;

    read_binary_datetime(&tm, row);
    fetch_datetime_with_conversion(param, field, &tm);
    break;
  }
  default:
  {
    ulong length= net_field_length(row);
    fetch_string_with_conversion(param, (char*) *row, length);
    *row+= length;
    break;
  }
  }
}