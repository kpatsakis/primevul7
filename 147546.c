context_complete (void *opaque, int *err)
{
  struct context_helper *h = opaque;
  h->err = *err;
  FREE_CALLBACK (h->context);
  return 0;
}