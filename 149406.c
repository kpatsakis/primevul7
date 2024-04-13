gst_h264_parse_sps (GstH264NalUnit * nalu, GstH264SPS * sps)
{
  NalReader nr;

  GST_DEBUG ("parsing SPS");

  nal_reader_init (&nr, nalu->data + nalu->offset + nalu->header_bytes,
      nalu->size - nalu->header_bytes);

  if (!gst_h264_parse_sps_data (&nr, sps))
    goto error;

  sps->valid = TRUE;

  return GST_H264_PARSER_OK;

error:
  GST_WARNING ("error parsing \"Sequence parameter set\"");
  sps->valid = FALSE;
  return GST_H264_PARSER_ERROR;
}