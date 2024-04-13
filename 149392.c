gst_h264_parser_insert_sei_avc (GstH264NalParser * nalparser,
    guint8 nal_length_size, GstBuffer * au, GstMemory * sei)
{
  g_return_val_if_fail (nalparser != NULL, NULL);
  g_return_val_if_fail (nal_length_size > 0 && nal_length_size < 5, NULL);
  g_return_val_if_fail (GST_IS_BUFFER (au), NULL);
  g_return_val_if_fail (sei != NULL, NULL);

  /* the size of start code prefix (3 or 4) is not matter since it will be
   * scanned */
  return gst_h264_parser_insert_sei_internal (nalparser, nal_length_size, TRUE,
      au, sei);
}