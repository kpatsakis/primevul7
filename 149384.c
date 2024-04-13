gst_h264_write_sei_content_light_level_info (NalWriter * nw,
    GstH264ContentLightLevel * cll)
{
  WRITE_UINT16 (nw, cll->max_content_light_level, 16);
  WRITE_UINT16 (nw, cll->max_pic_average_light_level, 16);

  return TRUE;

error:
  return FALSE;
}