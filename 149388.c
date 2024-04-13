gst_h264_sps_copy (GstH264SPS * dst_sps, const GstH264SPS * src_sps)
{
  g_return_val_if_fail (dst_sps != NULL, FALSE);
  g_return_val_if_fail (src_sps != NULL, FALSE);

  gst_h264_sps_clear (dst_sps);

  *dst_sps = *src_sps;

  switch (dst_sps->extension_type) {
    case GST_H264_NAL_EXTENSION_MVC:
      if (!gst_h264_sps_mvc_copy (dst_sps, src_sps))
        return FALSE;
      break;
  }
  return TRUE;
}