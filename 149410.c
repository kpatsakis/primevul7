gst_h264_sps_mvc_clear (GstH264SPS * sps)
{
  GstH264SPSExtMVC *const mvc = &sps->extension.mvc;
  guint i, j;

  g_assert (sps->extension_type == GST_H264_NAL_EXTENSION_MVC);

  g_free (mvc->view);
  mvc->view = NULL;

  for (i = 0; i <= mvc->num_level_values_signalled_minus1; i++) {
    GstH264SPSExtMVCLevelValue *const level_value = &mvc->level_value[i];

    for (j = 0; j <= level_value->num_applicable_ops_minus1; j++) {
      g_free (level_value->applicable_op[j].target_view_id);
      level_value->applicable_op[j].target_view_id = NULL;
    }
    g_free (level_value->applicable_op);
    level_value->applicable_op = NULL;
  }
  g_free (mvc->level_value);
  mvc->level_value = NULL;

  /* All meaningful MVC info are now gone, just pretend to be a
   * standard AVC struct now */
  sps->extension_type = GST_H264_NAL_EXTENSION_NONE;
}