gst_h264_parser_parse_recovery_point (GstH264NalParser * nalparser,
    GstH264RecoveryPoint * rp, NalReader * nr)
{
  GstH264SPS *const sps = nalparser->last_sps;

  GST_DEBUG ("parsing \"Recovery point\"");
  if (!sps || !sps->valid) {
    GST_WARNING ("didn't get the associated sequence parameter set for the "
        "current access unit");
    goto error;
  }

  READ_UE_MAX (nr, rp->recovery_frame_cnt, sps->max_frame_num - 1);
  READ_UINT8 (nr, rp->exact_match_flag, 1);
  READ_UINT8 (nr, rp->broken_link_flag, 1);
  READ_UINT8 (nr, rp->changing_slice_group_idc, 2);

  return GST_H264_PARSER_OK;

error:
  GST_WARNING ("error parsing \"Recovery point\"");
  return GST_H264_PARSER_ERROR;
}