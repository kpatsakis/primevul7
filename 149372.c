gst_h264_nal_parser_new (void)
{
  GstH264NalParser *nalparser;

  nalparser = g_slice_new0 (GstH264NalParser);

  return nalparser;
}