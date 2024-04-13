gst_h264_parser_parse_pps (GstH264NalParser * nalparser,
    GstH264NalUnit * nalu, GstH264PPS * pps)
{
  GstH264ParserResult res = gst_h264_parse_pps (nalparser, nalu, pps);

  if (res == GST_H264_PARSER_OK) {
    GST_DEBUG ("adding picture parameter set with id: %d to array", pps->id);

    if (!gst_h264_pps_copy (&nalparser->pps[pps->id], pps))
      return GST_H264_PARSER_ERROR;
    nalparser->last_pps = &nalparser->pps[pps->id];
  }

  return res;
}