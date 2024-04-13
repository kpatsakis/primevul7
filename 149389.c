gst_h264_parser_parse_frame_packing (GstH264NalParser * nalparser,
    GstH264FramePacking * frame_packing, NalReader * nr, guint payload_size)
{
  guint8 frame_packing_extension_flag;
  guint start_pos;

  GST_DEBUG ("parsing \"Frame Packing Arrangement\"");

  start_pos = nal_reader_get_pos (nr);
  READ_UE (nr, frame_packing->frame_packing_id);
  READ_UINT8 (nr, frame_packing->frame_packing_cancel_flag, 1);

  if (!frame_packing->frame_packing_cancel_flag) {
    READ_UINT8 (nr, frame_packing->frame_packing_type, 7);
    READ_UINT8 (nr, frame_packing->quincunx_sampling_flag, 1);
    READ_UINT8 (nr, frame_packing->content_interpretation_type, 6);
    READ_UINT8 (nr, frame_packing->spatial_flipping_flag, 1);
    READ_UINT8 (nr, frame_packing->frame0_flipped_flag, 1);
    READ_UINT8 (nr, frame_packing->field_views_flag, 1);
    READ_UINT8 (nr, frame_packing->current_frame_is_frame0_flag, 1);
    READ_UINT8 (nr, frame_packing->frame0_self_contained_flag, 1);
    READ_UINT8 (nr, frame_packing->frame1_self_contained_flag, 1);

    if (!frame_packing->quincunx_sampling_flag &&
        frame_packing->frame_packing_type !=
        GST_H264_FRAME_PACKING_TEMPORAL_INTERLEAVING) {
      READ_UINT8 (nr, frame_packing->frame0_grid_position_x, 4);
      READ_UINT8 (nr, frame_packing->frame0_grid_position_y, 4);
      READ_UINT8 (nr, frame_packing->frame1_grid_position_x, 4);
      READ_UINT8 (nr, frame_packing->frame1_grid_position_y, 4);
    }

    /* Skip frame_packing_arrangement_reserved_byte */
    if (!nal_reader_skip (nr, 8))
      goto error;

    READ_UE_MAX (nr, frame_packing->frame_packing_repetition_period, 16384);
  }

  READ_UINT8 (nr, frame_packing_extension_flag, 1);

  /* All data that follows within a frame packing arrangement SEI message
     after the value 1 for frame_packing_arrangement_extension_flag shall
     be ignored (D.2.25) */
  if (frame_packing_extension_flag) {
    nal_reader_skip_long (nr,
        payload_size - (nal_reader_get_pos (nr) - start_pos));
  }

  return GST_H264_PARSER_OK;

error:
  GST_WARNING ("error parsing \"Frame Packing Arrangement\"");
  return GST_H264_PARSER_ERROR;
}