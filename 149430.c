gst_h264_parser_parse_content_light_level_info (GstH264NalParser * parser,
    GstH264ContentLightLevel * cll, NalReader * nr)
{
  GST_DEBUG ("parsing \"Content light level\"");

  READ_UINT16 (nr, cll->max_content_light_level, 16);
  READ_UINT16 (nr, cll->max_pic_average_light_level, 16);

  return GST_H264_PARSER_OK;

error:
  GST_WARNING ("error parsing \"Content light level\"");
  return GST_H264_PARSER_ERROR;
}