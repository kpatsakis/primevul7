gst_h264_parser_parse_subset_sps (GstH264NalParser * nalparser,
    GstH264NalUnit * nalu, GstH264SPS * sps)
{
  GstH264ParserResult res;

  res = gst_h264_parse_subset_sps (nalu, sps);
  if (res == GST_H264_PARSER_OK) {
    GST_DEBUG ("adding sequence parameter set with id: %d to array", sps->id);

    if (!gst_h264_sps_copy (&nalparser->sps[sps->id], sps)) {
      gst_h264_sps_clear (sps);
      return GST_H264_PARSER_ERROR;
    }
    nalparser->last_sps = &nalparser->sps[sps->id];
  }
  return res;
}