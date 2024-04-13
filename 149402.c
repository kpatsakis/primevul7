gst_h264_create_sei_memory_avc (guint8 nal_length_size, GArray * messages)
{
  g_return_val_if_fail (nal_length_size > 0 && nal_length_size < 5, NULL);
  g_return_val_if_fail (messages != NULL, NULL);
  g_return_val_if_fail (messages->len > 0, NULL);

  return gst_h264_create_sei_memory_internal (nal_length_size, TRUE, messages);
}