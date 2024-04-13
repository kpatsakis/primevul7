gst_h264_parser_update_sps (GstH264NalParser * nalparser, GstH264SPS * sps)
{
  g_return_val_if_fail (nalparser != NULL, GST_H264_PARSER_ERROR);
  g_return_val_if_fail (sps != NULL, GST_H264_PARSER_ERROR);
  g_return_val_if_fail (sps->id >= 0 && sps->id < GST_H264_MAX_SPS_COUNT,
      GST_H264_PARSER_ERROR);

  if (!sps->valid) {
    GST_WARNING ("Cannot update with invalid SPS");
    return GST_H264_PARSER_ERROR;
  }

  GST_DEBUG ("Updating sequence parameter set with id: %d", sps->id);

  if (!gst_h264_sps_copy (&nalparser->sps[sps->id], sps))
    return GST_H264_PARSER_ERROR;

  nalparser->last_sps = &nalparser->sps[sps->id];

  return GST_H264_PARSER_OK;
}