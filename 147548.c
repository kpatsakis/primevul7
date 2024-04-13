nbd_internal_free_option (struct nbd_handle *h)
{
  if (h->opt_current == NBD_OPT_LIST)
    FREE_CALLBACK (h->opt_cb.fn.list);
  else if (h->opt_current == NBD_OPT_LIST_META_CONTEXT)
    FREE_CALLBACK (h->opt_cb.fn.context);
  FREE_CALLBACK (h->opt_cb.completion);
}