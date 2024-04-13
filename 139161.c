static void fetch_float_with_conversion(MYSQL_BIND *param, MYSQL_FIELD *field,
                                        double value, my_gcvt_arg_type type)
{
  char *buffer= (char *)param->buffer;
  double val64 = (value < 0 ? -floor(-value) : floor(value));

  switch (param->buffer_type) {
  case MYSQL_TYPE_NULL: /* do nothing */
    break;
  case MYSQL_TYPE_TINY:
    /*
      We need to _store_ data in the buffer before the truncation check to
      workaround Intel FPU executive precision feature.
      (See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=323 for details)
      Sic: AFAIU it does not guarantee to work.
    */
    if (param->is_unsigned)
      *buffer= (uint8) value;
    else
      *buffer= (int8) value;
    *param->error= val64 != (param->is_unsigned ? (double)((uint8) *buffer) :
                                                  (double)((int8) *buffer));
    break;
  case MYSQL_TYPE_SHORT:
    if (param->is_unsigned)
    {
      ushort data= (ushort) value;
      shortstore(buffer, data);
    }
    else
    {
      short data= (short) value;
      shortstore(buffer, data);
    }
    *param->error= val64 != (param->is_unsigned ? (double) (*(ushort*) buffer):
                                                  (double) (*(short*) buffer));
    break;
  case MYSQL_TYPE_LONG:
    if (param->is_unsigned)
    {
      uint32 data= (uint32) value;
      longstore(buffer, data);
    }
    else
    {
      int32 data= (int32) value;
      longstore(buffer, data);
    }
    *param->error= val64 != (param->is_unsigned ? (double) (*(uint32*) buffer):
                                                  (double) (*(int32*) buffer));
      break;
  case MYSQL_TYPE_LONGLONG:
    if (param->is_unsigned)
    {
      ulonglong data= (ulonglong) value;
      longlongstore(buffer, data);
    }
    else
    {
      longlong data= (longlong) value;
      longlongstore(buffer, data);
    }
    *param->error= val64 != (param->is_unsigned ?
                             ulonglong2double(*(ulonglong*) buffer) :
                             (double) (*(longlong*) buffer));
    break;
  case MYSQL_TYPE_FLOAT:
  {
    float data= (float) value;
    floatstore(buffer, data);
    *param->error= (*(float*) buffer) != value;
    break;
  }
  case MYSQL_TYPE_DOUBLE:
  {
    doublestore(buffer, value);
    break;
  }
  default:
  {
    /*
      Resort to fetch_string_with_conversion: this should handle
      floating point -> string conversion nicely, honor all typecodes
      and param->offset possibly set in mysql_stmt_fetch_column
    */
    char buff[FLOATING_POINT_BUFFER];
    size_t len;
    if (field->decimals >= NOT_FIXED_DEC)
      len= my_gcvt(value, type,
                   (int) min(sizeof(buff)-1, param->buffer_length),
                   buff, NULL);
    else
      len= my_fcvt(value, (int) field->decimals, buff, NULL);

    if (field->flags & ZEROFILL_FLAG && len < field->length &&
        field->length < MAX_DOUBLE_STRING_REP_LENGTH - 1)
    {
      bmove_upp((uchar*) buff + field->length, (uchar*) buff + len,
                len);
      bfill((char*) buff, field->length - len, '0');
      len= field->length;
    }
    fetch_string_with_conversion(param, buff, len);

    break;
  }
  }
}