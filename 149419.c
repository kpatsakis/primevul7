gst_h264_pps_clear (GstH264PPS * pps)
{
  g_return_if_fail (pps != NULL);

  g_free (pps->slice_group_id);
  pps->slice_group_id = NULL;
}