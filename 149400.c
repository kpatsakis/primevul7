gst_h264_parser_update_pps (GstH264NalParser * nalparser, GstH264PPS * pps)
{
  GstH264SPS *sps;

  g_return_val_if_fail (nalparser != NULL, GST_H264_PARSER_ERROR);
  g_return_val_if_fail (pps != NULL, GST_H264_PARSER_ERROR);
  g_return_val_if_fail (pps->id >= 0 && pps->id < GST_H264_MAX_PPS_COUNT,
      GST_H264_PARSER_ERROR);

  if (!pps->valid) {
    GST_WARNING ("Cannot update with invalid PPS");
    return GST_H264_PARSER_ERROR;
  }

  if (!pps->sequence) {
    GST_WARNING ("No linked SPS struct");
    return GST_H264_PARSER_BROKEN_LINK;
  }

  sps = gst_h264_parser_get_sps (nalparser, pps->sequence->id);
  if (!sps || sps != pps->sequence) {
    GST_WARNING ("Linked SPS is not identical to internal SPS");
    return GST_H264_PARSER_BROKEN_LINK;
  }

  GST_DEBUG ("Updating picture parameter set with id: %d", pps->id);

  if (!gst_h264_pps_copy (&nalparser->pps[pps->id], pps))
    return GST_H264_PARSER_ERROR;

  nalparser->last_pps = &nalparser->pps[pps->id];

  return GST_H264_PARSER_OK;
}