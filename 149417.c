gst_h264_parser_parse_pic_timing (GstH264NalParser * nalparser,
    GstH264PicTiming * tim, NalReader * nr)
{
  GstH264ParserResult error = GST_H264_PARSER_ERROR;

  GST_DEBUG ("parsing \"Picture timing\"");
  if (!nalparser->last_sps || !nalparser->last_sps->valid) {
    GST_WARNING ("didn't get the associated sequence parameter set for the "
        "current access unit");
    error = GST_H264_PARSER_BROKEN_LINK;
    goto error;
  }

  if (nalparser->last_sps->vui_parameters_present_flag) {
    GstH264VUIParams *vui = &nalparser->last_sps->vui_parameters;
    GstH264HRDParams *hrd = NULL;

    if (vui->nal_hrd_parameters_present_flag) {
      hrd = &vui->nal_hrd_parameters;
    } else if (vui->vcl_hrd_parameters_present_flag) {
      hrd = &vui->vcl_hrd_parameters;
    }

    tim->CpbDpbDelaysPresentFlag = ! !hrd;
    tim->pic_struct_present_flag = vui->pic_struct_present_flag;

    if (tim->CpbDpbDelaysPresentFlag) {
      tim->cpb_removal_delay_length_minus1 =
          hrd->cpb_removal_delay_length_minus1;
      tim->dpb_output_delay_length_minus1 = hrd->dpb_output_delay_length_minus1;

      READ_UINT32 (nr, tim->cpb_removal_delay,
          tim->cpb_removal_delay_length_minus1 + 1);
      READ_UINT32 (nr, tim->dpb_output_delay,
          tim->dpb_output_delay_length_minus1 + 1);
    }

    if (tim->pic_struct_present_flag) {
      const guint8 num_clock_ts_table[9] = {
        1, 1, 1, 2, 2, 3, 3, 2, 3
      };
      guint8 num_clock_num_ts;
      guint i;

      READ_UINT8 (nr, tim->pic_struct, 4);
      CHECK_ALLOWED ((gint8) tim->pic_struct, 0, 8);

      tim->time_offset_length = 24;
      if (hrd)
        tim->time_offset_length = hrd->time_offset_length;

      num_clock_num_ts = num_clock_ts_table[tim->pic_struct];
      for (i = 0; i < num_clock_num_ts; i++) {
        READ_UINT8 (nr, tim->clock_timestamp_flag[i], 1);
        if (tim->clock_timestamp_flag[i]) {
          if (!gst_h264_parse_clock_timestamp (&tim->clock_timestamp[i],
                  tim->time_offset_length, nr))
            goto error;
        }
      }
    }
  }

  if (!tim->CpbDpbDelaysPresentFlag && !tim->pic_struct_present_flag) {
    GST_WARNING
        ("Invalid pic_timing SEI NAL with neither CpbDpbDelays nor pic_struct");
    return GST_H264_PARSER_BROKEN_DATA;
  }

  return GST_H264_PARSER_OK;

error:
  GST_WARNING ("error parsing \"Picture timing\"");
  return error;
}