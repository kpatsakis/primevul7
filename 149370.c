gst_h264_video_calculate_framerate (const GstH264SPS * sps,
    guint field_pic_flag, guint pic_struct, gint * fps_num, gint * fps_den)
{
  gint num = 0;
  gint den = 1;

  /* To calculate framerate, we use this formula:
   *          time_scale                1                         1
   * fps = -----------------  x  ---------------  x  ------------------------
   *       num_units_in_tick     DeltaTfiDivisor     (field_pic_flag ? 2 : 1)
   *
   * See H264 specification E2.1 for more details.
   */

  if (sps) {
    if (sps->vui_parameters_present_flag) {
      const GstH264VUIParams *vui = &sps->vui_parameters;
      if (vui->timing_info_present_flag) {
        int delta_tfi_divisor = 1;
        num = vui->time_scale;
        den = vui->num_units_in_tick;

        if (vui->pic_struct_present_flag) {
          switch (pic_struct) {
            case 1:
            case 2:
              delta_tfi_divisor = 1;
              break;
            case 0:
            case 3:
            case 4:
              delta_tfi_divisor = 2;
              break;
            case 5:
            case 6:
              delta_tfi_divisor = 3;
              break;
            case 7:
              delta_tfi_divisor = 4;
              break;
            case 8:
              delta_tfi_divisor = 6;
              break;
          }
        } else {
          delta_tfi_divisor = field_pic_flag ? 1 : 2;
        }
        den *= delta_tfi_divisor;

        /* Picture is two fields ? */
        den *= (field_pic_flag ? 2 : 1);
      }
    }
  }

  *fps_num = num;
  *fps_den = den;
}