gst_h264_parser_parse_mastering_display_colour_volume (GstH264NalParser *
    parser, GstH264MasteringDisplayColourVolume * mdcv, NalReader * nr)
{
  guint i;

  GST_DEBUG ("parsing \"Mastering display colour volume\"");

  for (i = 0; i < 3; i++) {
    READ_UINT16 (nr, mdcv->display_primaries_x[i], 16);
    READ_UINT16 (nr, mdcv->display_primaries_y[i], 16);
  }

  READ_UINT16 (nr, mdcv->white_point_x, 16);
  READ_UINT16 (nr, mdcv->white_point_y, 16);
  READ_UINT32 (nr, mdcv->max_display_mastering_luminance, 32);
  READ_UINT32 (nr, mdcv->min_display_mastering_luminance, 32);

  return GST_H264_PARSER_OK;

error:
  GST_WARNING ("error parsing \"Mastering display colour volume\"");
  return GST_H264_PARSER_ERROR;
}