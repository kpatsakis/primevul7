gst_h264_write_sei_frame_packing (NalWriter * nw,
    GstH264FramePacking * frame_packing)
{
  WRITE_UE (nw, frame_packing->frame_packing_id);
  WRITE_UINT8 (nw, frame_packing->frame_packing_cancel_flag, 1);

  if (!frame_packing->frame_packing_cancel_flag) {
    WRITE_UINT8 (nw, frame_packing->frame_packing_type, 7);
    WRITE_UINT8 (nw, frame_packing->quincunx_sampling_flag, 1);
    WRITE_UINT8 (nw, frame_packing->content_interpretation_type, 6);
    WRITE_UINT8 (nw, frame_packing->spatial_flipping_flag, 1);
    WRITE_UINT8 (nw, frame_packing->frame0_flipped_flag, 1);
    WRITE_UINT8 (nw, frame_packing->field_views_flag, 1);
    WRITE_UINT8 (nw, frame_packing->current_frame_is_frame0_flag, 1);
    WRITE_UINT8 (nw, frame_packing->frame0_self_contained_flag, 1);
    WRITE_UINT8 (nw, frame_packing->frame1_self_contained_flag, 1);

    if (!frame_packing->quincunx_sampling_flag &&
        frame_packing->frame_packing_type !=
        GST_H264_FRAME_PACKING_TEMPORAL_INTERLEAVING) {
      WRITE_UINT8 (nw, frame_packing->frame0_grid_position_x, 4);
      WRITE_UINT8 (nw, frame_packing->frame0_grid_position_y, 4);
      WRITE_UINT8 (nw, frame_packing->frame1_grid_position_x, 4);
      WRITE_UINT8 (nw, frame_packing->frame1_grid_position_y, 4);
    }

    /* frame_packing_arrangement_reserved_byte */
    WRITE_UINT8 (nw, 0, 8);
    WRITE_UE (nw, frame_packing->frame_packing_repetition_period);
  }

  /* frame_packing_arrangement_extension_flag */
  WRITE_UINT8 (nw, 0, 1);

  return TRUE;

error:
  return FALSE;
}