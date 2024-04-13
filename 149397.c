gst_h264_write_sei_pic_timing (NalWriter * nw, GstH264PicTiming * tim)
{
  if (tim->CpbDpbDelaysPresentFlag) {
    WRITE_UINT32 (nw, tim->cpb_removal_delay,
        tim->cpb_removal_delay_length_minus1 + 1);
    WRITE_UINT32 (nw, tim->dpb_output_delay,
        tim->dpb_output_delay_length_minus1 + 1);
  }

  if (tim->pic_struct_present_flag) {
    const guint8 num_clock_ts_table[9] = {
      1, 1, 1, 2, 2, 3, 3, 2, 3
    };
    guint8 num_clock_num_ts;
    guint i;

    WRITE_UINT8 (nw, tim->pic_struct, 4);

    num_clock_num_ts = num_clock_ts_table[tim->pic_struct];
    for (i = 0; i < num_clock_num_ts; i++) {
      WRITE_UINT8 (nw, tim->clock_timestamp_flag[i], 1);
      if (tim->clock_timestamp_flag[i]) {
        GstH264ClockTimestamp *timestamp = &tim->clock_timestamp[i];

        WRITE_UINT8 (nw, timestamp->ct_type, 2);
        WRITE_UINT8 (nw, timestamp->nuit_field_based_flag, 1);
        WRITE_UINT8 (nw, timestamp->counting_type, 5);
        WRITE_UINT8 (nw, timestamp->full_timestamp_flag, 1);
        WRITE_UINT8 (nw, timestamp->discontinuity_flag, 1);
        WRITE_UINT8 (nw, timestamp->cnt_dropped_flag, 1);
        WRITE_UINT8 (nw, timestamp->n_frames, 8);

        if (timestamp->full_timestamp_flag) {
          WRITE_UINT8 (nw, timestamp->seconds_value, 6);
          WRITE_UINT8 (nw, timestamp->minutes_value, 6);
          WRITE_UINT8 (nw, timestamp->hours_value, 5);
        } else {
          WRITE_UINT8 (nw, timestamp->seconds_flag, 1);
          if (timestamp->seconds_flag) {
            WRITE_UINT8 (nw, timestamp->seconds_value, 6);
            WRITE_UINT8 (nw, timestamp->minutes_flag, 1);
            if (timestamp->minutes_flag) {
              WRITE_UINT8 (nw, timestamp->minutes_value, 6);
              WRITE_UINT8 (nw, timestamp->hours_flag, 1);
              if (timestamp->hours_flag)
                WRITE_UINT8 (nw, timestamp->hours_value, 5);
            }
          }
        }

        if (tim->time_offset_length > 0) {
          WRITE_UINT32 (nw, timestamp->time_offset, tim->time_offset_length);
        }
      }
    }
  }

  return TRUE;

error:
  return FALSE;
}