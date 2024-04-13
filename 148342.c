progress (int c)
{
  if (progress_cb)
    progress_cb (progress_cb_data, "pk_elg", c, 0, 0);
}