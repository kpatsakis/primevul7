nbd_unlocked_opt_list (struct nbd_handle *h, nbd_list_callback *list)
{
  struct list_helper s = { .list = *list };
  nbd_list_callback l = { .callback = list_visitor, .user_data = &s };
  nbd_completion_callback c = { .callback = list_complete, .user_data = &s };

  if (nbd_unlocked_aio_opt_list (h, &l, &c) == -1)
    return -1;

  SET_CALLBACK_TO_NULL (*list);
  if (wait_for_option (h) == -1)
    return -1;
  if (s.err) {
    set_error (s.err, "server replied with error to list request");
    return -1;
  }
  return s.count;
}