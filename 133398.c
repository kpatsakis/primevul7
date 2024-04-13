static int compare_flags_for_copy (HEADER* h)
{
  IMAP_HEADER_DATA* hd = (IMAP_HEADER_DATA*)h->data;

  if (h->read != hd->read)
    return 1;
  if (h->old != hd->old)
    return 1;
  if (h->flagged != hd->flagged)
    return 1;
  if (h->replied != hd->replied)
    return 1;

  return 0;
}