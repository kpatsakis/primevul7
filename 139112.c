static void fetch_long_with_conversion(MYSQL_BIND *param, MYSQL_FIELD *field,
                                       longlong value, my_bool is_unsigned)
{
  char *buffer= (char *)param->buffer;

  switch (param->buffer_type) {
  case MYSQL_TYPE_NULL: /* do nothing */
    break;
  case MYSQL_TYPE_TINY:
    *param->error= IS_TRUNCATED(value, param->is_unsigned,
                                INT_MIN8, INT_MAX8, UINT_MAX8);
    *(uchar *)param->buffer= (uchar) value;
    break;
  case MYSQL_TYPE_SHORT:
    *param->error= IS_TRUNCATED(value, param->is_unsigned,
                                INT_MIN16, INT_MAX16, UINT_MAX16);
    shortstore(buffer, (short) value);
    break;
  case MYSQL_TYPE_LONG:
    *param->error= IS_TRUNCATED(value, param->is_unsigned,
                                INT_MIN32, INT_MAX32, UINT_MAX32);
    longstore(buffer, (int32) value);
    break;
  case MYSQL_TYPE_LONGLONG:
    longlongstore(buffer, value);
    *param->error= param->is_unsigned != is_unsigned && value < 0;
    break;
  case MYSQL_TYPE_FLOAT:
  {
    /*
      We need to mark the local variable volatile to
      workaround Intel FPU executive precision feature.
      (See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=323 for details)
    */
    volatile float data;
    if (is_unsigned)
    {
      data= (float) ulonglong2double(value);
      *param->error= ((ulonglong) value) != ((ulonglong) data);
    }
    else
    {
      data= (float)value;
      *param->error= value != ((longlong) data);
    }
    floatstore(buffer, data);
    break;
  }
  case MYSQL_TYPE_DOUBLE:
  {
    volatile double data;
    if (is_unsigned)
    {
      data= ulonglong2double(value);
      *param->error= ((ulonglong) value) != ((ulonglong) data);
    }
    else
    {
      data= (double)value;
      *param->error= value != ((longlong) data);
    }
    doublestore(buffer, data);
    break;
  }
  case MYSQL_TYPE_TIME:
  case MYSQL_TYPE_DATE:
  case MYSQL_TYPE_TIMESTAMP:
  case MYSQL_TYPE_DATETIME:
  {
    int error;
    value= number_to_datetime(value, (MYSQL_TIME *) buffer, TIME_FUZZY_DATE,
                              &error);
    *param->error= test(error);
    break;
  }
  default:
  {
    uchar buff[22];                              /* Enough for longlong */
    uchar *end= (uchar*) longlong10_to_str(value, (char*) buff,
                                           is_unsigned ? 10: -10);
    /* Resort to string conversion which supports all typecodes */
    uint length= (uint) (end-buff);

    if (field->flags & ZEROFILL_FLAG && length < field->length &&
        field->length < 21)
    {
      bmove_upp(buff+field->length,buff+length, length);
      bfill(buff, field->length - length,'0');
      length= field->length;
    }
    fetch_string_with_conversion(param, (char*) buff, length);
    break;
  }
  }
}