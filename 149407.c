gst_h264_nal_parser_free (GstH264NalParser * nalparser)
{
  guint i;

  for (i = 0; i < GST_H264_MAX_SPS_COUNT; i++)
    gst_h264_sps_clear (&nalparser->sps[i]);
  for (i = 0; i < GST_H264_MAX_PPS_COUNT; i++)
    gst_h264_pps_clear (&nalparser->pps[i]);
  g_slice_free (GstH264NalParser, nalparser);

  nalparser = NULL;
}