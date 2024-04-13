static void fetch_result_bin(MYSQL_BIND *param,
                             MYSQL_FIELD *field __attribute__((unused)),
                             uchar **row)
{
  ulong length= net_field_length(row);
  ulong copy_length= min(length, param->buffer_length);
  memcpy(param->buffer, (char *)*row, copy_length);
  *param->length= length;
  *param->error= copy_length < length;
  *row+= length;
}