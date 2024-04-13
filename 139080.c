static void fetch_result_short(MYSQL_BIND *param, MYSQL_FIELD *field,
                               uchar **row)
{
  my_bool field_is_unsigned= test(field->flags & UNSIGNED_FLAG);
  ushort data= (ushort) sint2korr(*row);
  shortstore(param->buffer, data);
  *param->error= param->is_unsigned != field_is_unsigned && data > INT_MAX16;
  *row+= 2;
}