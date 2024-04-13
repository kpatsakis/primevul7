gst_h264_parser_parse_nal (GstH264NalParser * nalparser, GstH264NalUnit * nalu)
{
  GstH264SPS sps;
  GstH264PPS pps;

  switch (nalu->type) {
    case GST_H264_NAL_SPS:
      return gst_h264_parser_parse_sps (nalparser, nalu, &sps);
      break;
    case GST_H264_NAL_PPS:
      return gst_h264_parser_parse_pps (nalparser, nalu, &pps);
  }

  return GST_H264_PARSER_OK;
}