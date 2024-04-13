static void fetch_result_int32(MYSQL_BIND *param,
                               MYSQL_FIELD *field __attribute__((unused)),
                               uchar **row)
{
  my_bool field_is_unsigned= test(field->flags & UNSIGNED_FLAG);
  uint32 data= (uint32) sint4korr(*row);
  longstore(param->buffer, data);
  *param->error= param->is_unsigned != field_is_unsigned && data > INT_MAX32;
  *row+= 4;
}