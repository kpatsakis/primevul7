gst_h264_pps_copy (GstH264PPS * dst_pps, const GstH264PPS * src_pps)
{
  g_return_val_if_fail (dst_pps != NULL, FALSE);
  g_return_val_if_fail (src_pps != NULL, FALSE);

  gst_h264_pps_clear (dst_pps);

  *dst_pps = *src_pps;

  if (src_pps->slice_group_id)
    dst_pps->slice_group_id = g_memdup (src_pps->slice_group_id,
        src_pps->pic_size_in_map_units_minus1 + 1);

  return TRUE;
}