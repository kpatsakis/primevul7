go_complete (void *opaque, int *err)
{
  int *i = opaque;
  *i = *err;
  return 0;
}