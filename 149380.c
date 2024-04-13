gst_h264_parser_insert_sei_internal (GstH264NalParser * nalparser,
    guint8 nal_prefix_size, gboolean packetized, GstBuffer * au,
    GstMemory * sei)
{
  GstH264NalUnit nalu;
  GstMapInfo info;
  GstH264ParserResult pres;
  guint offset = 0;
  GstBuffer *new_buffer = NULL;

  if (!gst_buffer_map (au, &info, GST_MAP_READ)) {
    GST_ERROR ("Cannot map au buffer");
    return NULL;
  }

  /* Find the offset of the first slice */
  do {
    if (packetized) {
      pres = gst_h264_parser_identify_nalu_avc (nalparser,
          info.data, offset, info.size, nal_prefix_size, &nalu);
    } else {
      pres = gst_h264_parser_identify_nalu (nalparser,
          info.data, offset, info.size, &nalu);
    }

    if (pres != GST_H264_PARSER_OK && pres != GST_H264_PARSER_NO_NAL_END) {
      GST_DEBUG ("Failed to identify nal unit, ret: %d", pres);
      gst_buffer_unmap (au, &info);

      return NULL;
    }

    if ((nalu.type >= GST_H264_NAL_SLICE && nalu.type <= GST_H264_NAL_SLICE_IDR)
        || (nalu.type >= GST_H264_NAL_SLICE_AUX
            && nalu.type <= GST_H264_NAL_SLICE_DEPTH)) {
      GST_DEBUG ("Found slice nal type %d at offset %d",
          nalu.type, nalu.sc_offset);
      break;
    }

    offset = nalu.offset + nalu.size;
  } while (pres == GST_H264_PARSER_OK);
  gst_buffer_unmap (au, &info);

  /* found the best position now, create new buffer */
  new_buffer = gst_buffer_new ();

  /* copy all metadata */
  if (!gst_buffer_copy_into (new_buffer, au, GST_BUFFER_COPY_METADATA, 0, -1)) {
    GST_ERROR ("Failed to copy metadata into new buffer");
    gst_clear_buffer (&new_buffer);
    goto out;
  }

  /* copy non-slice nal */
  if (nalu.sc_offset > 0) {
    if (!gst_buffer_copy_into (new_buffer, au,
            GST_BUFFER_COPY_MEMORY, 0, nalu.sc_offset)) {
      GST_ERROR ("Failed to copy buffer");
      gst_clear_buffer (&new_buffer);
      goto out;
    }
  }

  /* insert sei */
  gst_buffer_append_memory (new_buffer, gst_memory_ref (sei));

  /* copy the rest */
  if (!gst_buffer_copy_into (new_buffer, au,
          GST_BUFFER_COPY_MEMORY, nalu.sc_offset, -1)) {
    GST_ERROR ("Failed to copy buffer");
    gst_clear_buffer (&new_buffer);
    goto out;
  }

out:
  return new_buffer;
}