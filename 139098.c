mysql_set_local_infile_handler(MYSQL *mysql,
                               int (*local_infile_init)(void **, const char *,
                               void *),
                               int (*local_infile_read)(void *, char *, uint),
                               void (*local_infile_end)(void *),
                               int (*local_infile_error)(void *, char *, uint),
                               void *userdata)
{
  mysql->options.local_infile_init=  local_infile_init;
  mysql->options.local_infile_read=  local_infile_read;
  mysql->options.local_infile_end=   local_infile_end;
  mysql->options.local_infile_error= local_infile_error;
  mysql->options.local_infile_userdata = userdata;
}