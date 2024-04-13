static int default_local_infile_init(void **ptr, const char *filename,
             void *userdata __attribute__ ((unused)))
{
  default_local_infile_data *data;
  char tmp_name[FN_REFLEN];

  if (!(*ptr= data= ((default_local_infile_data *)
		     my_malloc(sizeof(default_local_infile_data),  MYF(0)))))
    return 1; /* out of memory */

  data->error_msg[0]= 0;
  data->error_num=    0;
  data->filename= filename;

  fn_format(tmp_name, filename, "", "", MY_UNPACK_FILENAME);
  if ((data->fd = my_open(tmp_name, O_RDONLY, MYF(0))) < 0)
  {
    data->error_num= my_errno;
    my_snprintf(data->error_msg, sizeof(data->error_msg)-1,
                EE(EE_FILENOTFOUND), tmp_name, data->error_num);
    return 1;
  }
  return 0; /* ok */
}