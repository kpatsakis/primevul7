static void skip_result_fixed(MYSQL_BIND *param,
			      MYSQL_FIELD *field __attribute__((unused)),
			      uchar **row)

{
  (*row)+= param->pack_length;
}