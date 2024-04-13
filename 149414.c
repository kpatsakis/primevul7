gst_h264_create_sei_memory_internal (guint8 nal_prefix_size,
    gboolean packetized, GArray * messages)
{
  NalWriter nw;
  gint i;
  gboolean have_written_data = FALSE;

  nal_writer_init (&nw, nal_prefix_size, packetized);

  if (messages->len == 0)
    goto error;

  GST_DEBUG ("Create SEI nal from array, len: %d", messages->len);

  /* nal header */
  /* forbidden_zero_bit */
  WRITE_UINT8 (&nw, 0, 1);
  /* nal_ref_idc, zero for sei nalu */
  WRITE_UINT8 (&nw, 0, 2);
  /* nal_unit_type */
  WRITE_UINT8 (&nw, GST_H264_NAL_SEI, 5);

  for (i = 0; i < messages->len; i++) {
    GstH264SEIMessage *msg = &g_array_index (messages, GstH264SEIMessage, i);
    guint32 payload_size_data = 0;
    guint32 payload_size_in_bits = 0;
    guint32 payload_type_data = msg->payloadType;
    gboolean need_align = FALSE;

    switch (payload_type_data) {
      case GST_H264_SEI_REGISTERED_USER_DATA:{
        GstH264RegisteredUserData *rud = &msg->payload.registered_user_data;

        /* itu_t_t35_country_code: 8 bits */
        payload_size_data = 1;
        if (rud->country_code == 0xff) {
          /* itu_t_t35_country_code_extension_byte */
          payload_size_data++;
        }

        payload_size_data += rud->size;
        break;
      }
      case GST_H264_SEI_FRAME_PACKING:{
        GstH264FramePacking *frame_packing = &msg->payload.frame_packing;
        guint leading_zeros, rest;

        /* frame_packing_arrangement_id: exp-golomb bits */
        count_exp_golomb_bits (frame_packing->frame_packing_id,
            &leading_zeros, &rest);
        payload_size_in_bits = leading_zeros + rest;

        /* frame_packing_arrangement_cancel_flag: 1 bit */
        payload_size_in_bits++;
        if (!frame_packing->frame_packing_cancel_flag) {
          /* frame_packing_arrangement_type: 7 bits
           * quincunx_sampling_flag: 1 bit
           * content_interpretation_type: 6 bit
           * spatial_flipping_flag: 1 bit
           * frame0_flipped_flag: 1 bit
           * field_views_flag: 1 bit
           * current_frame_is_frame0_flag: 1 bit
           * frame0_self_contained_flag: 1 bit
           * frame1_self_contained_flag: 1 bit
           */
          payload_size_in_bits += 20;

          if (!frame_packing->quincunx_sampling_flag &&
              frame_packing->frame_packing_type !=
              GST_H264_FRAME_PACKING_TEMPORAL_INTERLEAVING) {
            /* frame0_grid_position_x: 4bits
             * frame0_grid_position_y: 4bits
             * frame1_grid_position_x: 4bits
             * frame1_grid_position_y: 4bits
             */
            payload_size_in_bits += 16;
          }

          /* frame_packing_arrangement_reserved_byte: 8 bits */
          payload_size_in_bits += 8;

          /* frame_packing_arrangement_repetition_period: exp-golomb bits */
          count_exp_golomb_bits (frame_packing->frame_packing_repetition_period,
              &leading_zeros, &rest);
          payload_size_in_bits += (leading_zeros + rest);
        }
        /* frame_packing_arrangement_extension_flag: 1 bit */
        payload_size_in_bits++;

        payload_size_data = payload_size_in_bits >> 3;

        if ((payload_size_in_bits & 0x7) != 0) {
          GST_INFO ("Bits for Frame Packing SEI is not byte aligned");
          payload_size_data++;
          need_align = TRUE;
        }
        break;
      }
      case GST_H264_SEI_MASTERING_DISPLAY_COLOUR_VOLUME:
        /* x, y 16 bits per RGB channel
         * x, y 16 bits white point
         * max, min luminance 32 bits
         *
         * (2 * 2 * 3) + (2 * 2) + (4 * 2) = 24 bytes
         */
        payload_size_data = 24;
        break;
      case GST_H264_SEI_CONTENT_LIGHT_LEVEL:
        /* maxCLL and maxFALL per 16 bits
         *
         * 2 * 2 = 4 bytes
         */
        payload_size_data = 4;
        break;
      case GST_H264_SEI_PIC_TIMING:{
        GstH264PicTiming *tim = &msg->payload.pic_timing;
        const guint8 num_clock_ts_table[9] = {
          1, 1, 1, 2, 2, 3, 3, 2, 3
        };
        guint8 num_clock_num_ts;
        guint i;

        if (!tim->CpbDpbDelaysPresentFlag && !tim->pic_struct_present_flag) {
          GST_WARNING
              ("Both CpbDpbDelaysPresentFlag and pic_struct_present_flag are zero");
          break;
        }

        if (tim->CpbDpbDelaysPresentFlag) {
          payload_size_in_bits = tim->cpb_removal_delay_length_minus1 + 1;
          payload_size_in_bits += tim->dpb_output_delay_length_minus1 + 1;
        }

        if (tim->pic_struct_present_flag) {
          /* pic_struct: 4bits */
          payload_size_in_bits += 4;

          num_clock_num_ts = num_clock_ts_table[tim->pic_struct];
          for (i = 0; i < num_clock_num_ts; i++) {
            /* clock_timestamp_flag: 1bit */
            payload_size_in_bits++;

            if (tim->clock_timestamp_flag[i]) {
              GstH264ClockTimestamp *timestamp = &tim->clock_timestamp[i];

              /* ct_type: 2bits
               * nuit_field_based_flag: 1bit
               * counting_type: 5bits
               * full_timestamp_flag: 1bit
               * discontinuity_flag: 1bit
               * cnt_dropped_flag: 1bit
               * n_frames: 8bits
               */
              payload_size_in_bits += 19;
              if (timestamp->full_timestamp_flag) {
                /* seconds_value: 6bits
                 * minutes_value: 6bits
                 * hours_value: 5bits
                 */
                payload_size_in_bits += 17;
              } else {
                /* seconds_flag: 1bit */
                payload_size_in_bits++;

                if (timestamp->seconds_flag) {
                  /* seconds_value: 6bits
                   * minutes_flag: 1bit
                   */
                  payload_size_in_bits += 7;
                  if (timestamp->minutes_flag) {
                    /* minutes_value: 6bits
                     * hours_flag: 1bits
                     */
                    payload_size_in_bits += 7;
                    if (timestamp->hours_flag) {
                      /* hours_value: 5bits */
                      payload_size_in_bits += 5;
                    }
                  }
                }
              }

              /* time_offset_length bits */
              payload_size_in_bits += tim->time_offset_length;
            }
          }
        }

        payload_size_data = payload_size_in_bits >> 3;

        if ((payload_size_in_bits & 0x7) != 0) {
          GST_INFO ("Bits for Picture Timing SEI is not byte aligned");
          payload_size_data++;
          need_align = TRUE;
        }
        break;
      }
      default:
        break;
    }

    if (payload_size_data == 0) {
      GST_FIXME ("Unsupported SEI type %d", msg->payloadType);
      continue;
    }

    /* write payload type bytes */
    while (payload_type_data >= 0xff) {
      WRITE_UINT8 (&nw, 0xff, 8);
      payload_type_data -= -0xff;
    }
    WRITE_UINT8 (&nw, payload_type_data, 8);

    /* write payload size bytes */
    while (payload_size_data >= 0xff) {
      WRITE_UINT8 (&nw, 0xff, 8);
      payload_size_data -= -0xff;
    }
    WRITE_UINT8 (&nw, payload_size_data, 8);

    switch (msg->payloadType) {
      case GST_H264_SEI_REGISTERED_USER_DATA:
        GST_DEBUG ("Writing \"Registered user data\"");
        if (!gst_h264_write_sei_registered_user_data (&nw,
                &msg->payload.registered_user_data)) {
          GST_WARNING ("Failed to write \"Registered user data\"");
          goto error;
        }
        have_written_data = TRUE;
        break;
      case GST_H264_SEI_FRAME_PACKING:
        GST_DEBUG ("Writing \"Frame packing\"");
        if (!gst_h264_write_sei_frame_packing (&nw,
                &msg->payload.frame_packing)) {
          GST_WARNING ("Failed to write \"Frame packing\"");
          goto error;
        }
        have_written_data = TRUE;
        break;
      case GST_H264_SEI_MASTERING_DISPLAY_COLOUR_VOLUME:
        GST_DEBUG ("Wrtiting \"Mastering display colour volume\"");
        if (!gst_h264_write_sei_mastering_display_colour_volume (&nw,
                &msg->payload.mastering_display_colour_volume)) {
          GST_WARNING ("Failed to write \"Mastering display colour volume\"");
          goto error;
        }
        have_written_data = TRUE;
        break;
      case GST_H264_SEI_CONTENT_LIGHT_LEVEL:
        GST_DEBUG ("Writing \"Content light level\"");
        if (!gst_h264_write_sei_content_light_level_info (&nw,
                &msg->payload.content_light_level)) {
          GST_WARNING ("Failed to write \"Content light level\"");
          goto error;
        }
        have_written_data = TRUE;
        break;
      case GST_H264_SEI_PIC_TIMING:
        GST_DEBUG ("Writing \"Picture timing\"");
        if (!gst_h264_write_sei_pic_timing (&nw, &msg->payload.pic_timing)) {
          GST_WARNING ("Failed to write \"Picture timing\"");
          goto error;
        }
        have_written_data = TRUE;
        break;
      default:
        break;
    }

    if (need_align && !nal_writer_do_rbsp_trailing_bits (&nw)) {
      GST_WARNING ("Cannot insert traling bits");
      goto error;
    }
  }

  if (!have_written_data) {
    GST_WARNING ("No written sei data");
    goto error;
  }

  if (!nal_writer_do_rbsp_trailing_bits (&nw)) {
    GST_WARNING ("Failed to insert rbsp trailing bits");
    goto error;
  }

  return nal_writer_reset_and_get_memory (&nw);

error:
  nal_writer_reset (&nw);

  return NULL;
}