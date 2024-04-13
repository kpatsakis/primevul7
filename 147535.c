context_visitor (void *opaque, const char *name)
{
  struct context_helper *h = opaque;
  if (h->count < INT_MAX)
    h->count++;
  CALL_CALLBACK (h->context, name);
  return 0;
}