com_edit(String *buffer,char *line __attribute__((unused)))
{
  char	filename[FN_REFLEN],buff[160];
  int	fd,tmp;
  const char *editor;

  if ((fd=create_temp_file(filename,NullS,"sql", O_CREAT | O_WRONLY,
			   MYF(MY_WME))) < 0)
    goto err;
  if (buffer->is_empty() && !old_buffer.is_empty())
    (void) my_write(fd,(uchar*) old_buffer.ptr(),old_buffer.length(),
		    MYF(MY_WME));
  else
    (void) my_write(fd,(uchar*) buffer->ptr(),buffer->length(),MYF(MY_WME));
  (void) my_close(fd,MYF(0));

  if (!(editor = (char *)getenv("EDITOR")) &&
      !(editor = (char *)getenv("VISUAL")))
    editor = "vi";
  strxmov(buff,editor," ",filename,NullS);
  if(system(buff) == -1)
    goto err;

  MY_STAT stat_arg;
  if (!my_stat(filename,&stat_arg,MYF(MY_WME)))
    goto err;
  if ((fd = my_open(filename,O_RDONLY, MYF(MY_WME))) < 0)
    goto err;
  (void) buffer->alloc((uint) stat_arg.st_size);
  if ((tmp=read(fd,(char*) buffer->ptr(),buffer->alloced_length())) >= 0L)
    buffer->length((uint) tmp);
  else
    buffer->length(0);
  (void) my_close(fd,MYF(0));
  (void) my_delete(filename,MYF(MY_WME));
err:
  return 0;
}