gst_h264_create_sei_memory (guint8 start_code_prefix_length, GArray * messages)
{
  g_return_val_if_fail (start_code_prefix_length == 3
      || start_code_prefix_length == 4, NULL);
  g_return_val_if_fail (messages != NULL, NULL);
  g_return_val_if_fail (messages->len > 0, NULL);

  return gst_h264_create_sei_memory_internal (start_code_prefix_length,
      FALSE, messages);
}