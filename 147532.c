list_visitor (void *opaque, const char *name, const char *description)
{
  struct list_helper *h = opaque;
  if (h->count < INT_MAX)
    h->count++;
  CALL_CALLBACK (h->list, name, description);
  return 0;
}