static void fetch_string_with_conversion(MYSQL_BIND *param, char *value,
                                         uint length)
{
  char *buffer= (char *)param->buffer;
  int err= 0;
  char *endptr= value + length;

  /*
    This function should support all target buffer types: the rest
    of conversion functions can delegate conversion to it.
  */
  switch (param->buffer_type) {
  case MYSQL_TYPE_NULL: /* do nothing */
    break;
  case MYSQL_TYPE_TINY:
  {
    longlong data= my_strtoll10(value, &endptr, &err);
    *param->error= (IS_TRUNCATED(data, param->is_unsigned,
                                 INT_MIN8, INT_MAX8, UINT_MAX8) || err > 0);
    *buffer= (uchar) data;
    break;
  }
  case MYSQL_TYPE_SHORT:
  {
    longlong data= my_strtoll10(value, &endptr, &err);
    *param->error= (IS_TRUNCATED(data, param->is_unsigned,
                                 INT_MIN16, INT_MAX16, UINT_MAX16) || err > 0);
    shortstore(buffer, (short) data);
    break;
  }
  case MYSQL_TYPE_LONG:
  {
    longlong data= my_strtoll10(value, &endptr, &err);
    *param->error= (IS_TRUNCATED(data, param->is_unsigned,
                                 INT_MIN32, INT_MAX32, UINT_MAX32) || err > 0);
    longstore(buffer, (int32) data);
    break;
  }
  case MYSQL_TYPE_LONGLONG:
  {
    longlong data= my_strtoll10(value, &endptr, &err);
    *param->error= param->is_unsigned ? err != 0 :
                                       (err > 0 || (err == 0 && data < 0));
    longlongstore(buffer, data);
    break;
  }
  case MYSQL_TYPE_FLOAT:
  {
    double data= my_strntod(&my_charset_latin1, value, length, &endptr, &err);
    float fdata= (float) data;
    *param->error= (fdata != data) | test(err);
    floatstore(buffer, fdata);
    break;
  }
  case MYSQL_TYPE_DOUBLE:
  {
    double data= my_strntod(&my_charset_latin1, value, length, &endptr, &err);
    *param->error= test(err);
    doublestore(buffer, data);
    break;
  }
  case MYSQL_TYPE_TIME:
  {
    MYSQL_TIME *tm= (MYSQL_TIME *)buffer;
    str_to_time(value, length, tm, &err);
    *param->error= test(err);
    break;
  }
  case MYSQL_TYPE_DATE:
  case MYSQL_TYPE_DATETIME:
  case MYSQL_TYPE_TIMESTAMP:
  {
    MYSQL_TIME *tm= (MYSQL_TIME *)buffer;
    (void) str_to_datetime(value, length, tm, TIME_FUZZY_DATE, &err);
    *param->error= test(err) && (param->buffer_type == MYSQL_TYPE_DATE &&
                                 tm->time_type != MYSQL_TIMESTAMP_DATE);
    break;
  }
  case MYSQL_TYPE_TINY_BLOB:
  case MYSQL_TYPE_MEDIUM_BLOB:
  case MYSQL_TYPE_LONG_BLOB:
  case MYSQL_TYPE_BLOB:
  case MYSQL_TYPE_DECIMAL:
  case MYSQL_TYPE_NEWDECIMAL:
  default:
  {
    /*
      Copy column data to the buffer taking into account offset,
      data length and buffer length.
    */
    char *start= value + param->offset;
    char *end= value + length;
    ulong copy_length;
    if (start < end)
    {
      copy_length= end - start;
      /* We've got some data beyond offset: copy up to buffer_length bytes */
      if (param->buffer_length)
        memcpy(buffer, start, min(copy_length, param->buffer_length));
    }
    else
      copy_length= 0;
    if (copy_length < param->buffer_length)
      buffer[copy_length]= '\0';
    *param->error= copy_length > param->buffer_length;
    /*
      param->length will always contain length of entire column;
      number of copied bytes may be way different:
    */
    *param->length= length;
    break;
  }
  }
}