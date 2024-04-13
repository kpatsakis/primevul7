list_complete (void *opaque, int *err)
{
  struct list_helper *h = opaque;
  h->err = *err;
  FREE_CALLBACK (h->list);
  return 0;
}