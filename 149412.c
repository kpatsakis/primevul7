gst_h264_write_sei_mastering_display_colour_volume (NalWriter * nw,
    GstH264MasteringDisplayColourVolume * mdcv)
{
  gint i;

  for (i = 0; i < 3; i++) {
    WRITE_UINT16 (nw, mdcv->display_primaries_x[i], 16);
    WRITE_UINT16 (nw, mdcv->display_primaries_y[i], 16);
  }

  WRITE_UINT16 (nw, mdcv->white_point_x, 16);
  WRITE_UINT16 (nw, mdcv->white_point_y, 16);
  WRITE_UINT32 (nw, mdcv->max_display_mastering_luminance, 32);
  WRITE_UINT32 (nw, mdcv->min_display_mastering_luminance, 32);

  return TRUE;

error:
  return FALSE;
}