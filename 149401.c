gst_h264_sps_clear (GstH264SPS * sps)
{
  g_return_if_fail (sps != NULL);

  switch (sps->extension_type) {
    case GST_H264_NAL_EXTENSION_MVC:
      gst_h264_sps_mvc_clear (sps);
      break;
  }
}