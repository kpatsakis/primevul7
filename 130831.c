com_clear(String *buffer,char *line __attribute__((unused)))
{
#ifdef HAVE_READLINE
  if (status.add_to_history)
    fix_history(buffer);
#endif
  buffer->length(0);
  return 0;
}