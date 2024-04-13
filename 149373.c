gst_h264_parser_parse_sei_message (GstH264NalParser * nalparser,
    NalReader * nr, GstH264SEIMessage * sei)
{
  guint32 payloadSize;
  guint8 payload_type_byte, payload_size_byte;
  guint remaining, payload_size, next;
  GstH264ParserResult res;

  GST_DEBUG ("parsing \"SEI message\"");

  memset (sei, 0, sizeof (*sei));

  do {
    READ_UINT8 (nr, payload_type_byte, 8);
    sei->payloadType += payload_type_byte;
  } while (payload_type_byte == 0xff);

  payloadSize = 0;
  do {
    READ_UINT8 (nr, payload_size_byte, 8);
    payloadSize += payload_size_byte;
  }
  while (payload_size_byte == 0xff);

  remaining = nal_reader_get_remaining (nr);
  payload_size = payloadSize * 8 < remaining ? payloadSize * 8 : remaining;
  next = nal_reader_get_pos (nr) + payload_size;

  GST_DEBUG ("SEI message received: payloadType  %u, payloadSize = %u bits",
      sei->payloadType, payload_size);

  switch (sei->payloadType) {
    case GST_H264_SEI_BUF_PERIOD:
      /* size not set; might depend on emulation_prevention_three_byte */
      res = gst_h264_parser_parse_buffering_period (nalparser,
          &sei->payload.buffering_period, nr);
      break;
    case GST_H264_SEI_PIC_TIMING:
      /* size not set; might depend on emulation_prevention_three_byte */
      res = gst_h264_parser_parse_pic_timing (nalparser,
          &sei->payload.pic_timing, nr);
      break;
    case GST_H264_SEI_REGISTERED_USER_DATA:
      res = gst_h264_parser_parse_registered_user_data (nalparser,
          &sei->payload.registered_user_data, nr, payload_size >> 3);
      break;
    case GST_H264_SEI_RECOVERY_POINT:
      res = gst_h264_parser_parse_recovery_point (nalparser,
          &sei->payload.recovery_point, nr);
      break;
    case GST_H264_SEI_STEREO_VIDEO_INFO:
      res = gst_h264_parser_parse_stereo_video_info (nalparser,
          &sei->payload.stereo_video_info, nr);
      break;
    case GST_H264_SEI_FRAME_PACKING:
      res = gst_h264_parser_parse_frame_packing (nalparser,
          &sei->payload.frame_packing, nr, payload_size);
      break;
    case GST_H264_SEI_MASTERING_DISPLAY_COLOUR_VOLUME:
      res = gst_h264_parser_parse_mastering_display_colour_volume (nalparser,
          &sei->payload.mastering_display_colour_volume, nr);
      break;
    case GST_H264_SEI_CONTENT_LIGHT_LEVEL:
      res = gst_h264_parser_parse_content_light_level_info (nalparser,
          &sei->payload.content_light_level, nr);
      break;
    default:
      res = gst_h264_parser_parse_sei_unhandled_payload (nalparser,
          &sei->payload.unhandled_payload, nr, sei->payloadType,
          payload_size >> 3);
      sei->payloadType = GST_H264_SEI_UNHANDLED_PAYLOAD;
      break;
  }

  /* When SEI message doesn't end at byte boundary,
   * check remaining bits fit the specification.
   */
  if (!nal_reader_is_byte_aligned (nr)) {
    guint8 bit_equal_to_one;
    READ_UINT8 (nr, bit_equal_to_one, 1);
    if (!bit_equal_to_one)
      GST_WARNING ("Bit non equal to one.");

    while (!nal_reader_is_byte_aligned (nr)) {
      guint8 bit_equal_to_zero;
      READ_UINT8 (nr, bit_equal_to_zero, 1);
      if (bit_equal_to_zero)
        GST_WARNING ("Bit non equal to zero.");
    }
  }

  /* Always make sure all the advertised SEI bits
   * were consumed during parsing */
  if (next > nal_reader_get_pos (nr)) {
    GST_LOG ("Skipping %u unused SEI bits", next - nal_reader_get_pos (nr));

    if (!nal_reader_skip_long (nr, next - nal_reader_get_pos (nr)))
      goto error;
  }

  return res;

error:
  GST_WARNING ("error parsing \"Sei message\"");
  return GST_H264_PARSER_ERROR;
}