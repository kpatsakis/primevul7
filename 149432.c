gst_h264_parser_parse_stereo_video_info (GstH264NalParser * nalparser,
    GstH264StereoVideoInfo * info, NalReader * nr)
{
  GST_DEBUG ("parsing \"Stereo Video info\"");

  READ_UINT8 (nr, info->field_views_flag, 1);
  if (info->field_views_flag) {
    READ_UINT8 (nr, info->top_field_is_left_view_flag, 1);
  } else {
    READ_UINT8 (nr, info->current_frame_is_left_view_flag, 1);
    READ_UINT8 (nr, info->next_frame_is_second_view_flag, 1);
  }
  READ_UINT8 (nr, info->left_view_self_contained_flag, 1);
  READ_UINT8 (nr, info->right_view_self_contained_flag, 1);

  return GST_H264_PARSER_OK;

error:
  GST_WARNING ("error parsing \"Stereo Video info\"");
  return GST_H264_PARSER_ERROR;
}