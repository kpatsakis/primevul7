gst_h264_sps_mvc_copy (GstH264SPS * dst_sps, const GstH264SPS * src_sps)
{
  GstH264SPSExtMVC *const dst_mvc = &dst_sps->extension.mvc;
  const GstH264SPSExtMVC *const src_mvc = &src_sps->extension.mvc;
  guint i, j, k;

  g_assert (dst_sps->extension_type == GST_H264_NAL_EXTENSION_MVC);

  dst_mvc->num_views_minus1 = src_mvc->num_views_minus1;
  dst_mvc->view = g_new0 (GstH264SPSExtMVCView, dst_mvc->num_views_minus1 + 1);
  if (!dst_mvc->view)
    return FALSE;

  dst_mvc->view[0].view_id = src_mvc->view[0].view_id;

  for (i = 1; i <= dst_mvc->num_views_minus1; i++) {
    GstH264SPSExtMVCView *const dst_view = &dst_mvc->view[i];
    const GstH264SPSExtMVCView *const src_view = &src_mvc->view[i];

    dst_view->view_id = src_view->view_id;

    dst_view->num_anchor_refs_l0 = src_view->num_anchor_refs_l0;
    for (j = 0; j < dst_view->num_anchor_refs_l0; j++)
      dst_view->anchor_ref_l0[j] = src_view->anchor_ref_l0[j];

    dst_view->num_anchor_refs_l1 = src_view->num_anchor_refs_l1;
    for (j = 0; j < dst_view->num_anchor_refs_l1; j++)
      dst_view->anchor_ref_l1[j] = src_view->anchor_ref_l1[j];

    dst_view->num_non_anchor_refs_l0 = src_view->num_non_anchor_refs_l0;
    for (j = 0; j < dst_view->num_non_anchor_refs_l0; j++)
      dst_view->non_anchor_ref_l0[j] = src_view->non_anchor_ref_l0[j];

    dst_view->num_non_anchor_refs_l1 = src_view->num_non_anchor_refs_l1;
    for (j = 0; j < dst_view->num_non_anchor_refs_l1; j++)
      dst_view->non_anchor_ref_l1[j] = src_view->non_anchor_ref_l1[j];
  }

  dst_mvc->num_level_values_signalled_minus1 =
      src_mvc->num_level_values_signalled_minus1;
  dst_mvc->level_value = g_new0 (GstH264SPSExtMVCLevelValue,
      dst_mvc->num_level_values_signalled_minus1 + 1);
  if (!dst_mvc->level_value)
    return FALSE;

  for (i = 0; i <= dst_mvc->num_level_values_signalled_minus1; i++) {
    GstH264SPSExtMVCLevelValue *const dst_value = &dst_mvc->level_value[i];
    const GstH264SPSExtMVCLevelValue *const src_value =
        &src_mvc->level_value[i];

    dst_value->level_idc = src_value->level_idc;

    dst_value->num_applicable_ops_minus1 = src_value->num_applicable_ops_minus1;
    dst_value->applicable_op = g_new0 (GstH264SPSExtMVCLevelValueOp,
        dst_value->num_applicable_ops_minus1 + 1);
    if (!dst_value->applicable_op)
      return FALSE;

    for (j = 0; j <= dst_value->num_applicable_ops_minus1; j++) {
      GstH264SPSExtMVCLevelValueOp *const dst_op = &dst_value->applicable_op[j];
      const GstH264SPSExtMVCLevelValueOp *const src_op =
          &src_value->applicable_op[j];

      dst_op->temporal_id = src_op->temporal_id;
      dst_op->num_target_views_minus1 = src_op->num_target_views_minus1;
      dst_op->target_view_id =
          g_new (guint16, dst_op->num_target_views_minus1 + 1);
      if (!dst_op->target_view_id)
        return FALSE;

      for (k = 0; k <= dst_op->num_target_views_minus1; k++)
        dst_op->target_view_id[k] = src_op->target_view_id[k];
      dst_op->num_views_minus1 = src_op->num_views_minus1;
    }
  }
  return TRUE;
}