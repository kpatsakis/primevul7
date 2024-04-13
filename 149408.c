gst_h264_parse_subset_sps (GstH264NalUnit * nalu, GstH264SPS * sps)
{
  NalReader nr;

  GST_DEBUG ("parsing Subset SPS");

  nal_reader_init (&nr, nalu->data + nalu->offset + nalu->header_bytes,
      nalu->size - nalu->header_bytes);

  if (!gst_h264_parse_sps_data (&nr, sps))
    goto error;

  if (sps->profile_idc == GST_H264_PROFILE_MULTIVIEW_HIGH ||
      sps->profile_idc == GST_H264_PROFILE_STEREO_HIGH) {
    if (!gst_h264_parse_sps_mvc_data (&nr, sps))
      goto error;
  }

  sps->valid = TRUE;
  return GST_H264_PARSER_OK;

error:
  GST_WARNING ("error parsing \"Subset sequence parameter set\"");
  gst_h264_sps_clear (sps);
  sps->valid = FALSE;
  return GST_H264_PARSER_ERROR;
}