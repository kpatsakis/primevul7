void mysql_set_local_infile_default(MYSQL *mysql)
{
  mysql->options.local_infile_init=  default_local_infile_init;
  mysql->options.local_infile_read=  default_local_infile_read;
  mysql->options.local_infile_end=   default_local_infile_end;
  mysql->options.local_infile_error= default_local_infile_error;
}