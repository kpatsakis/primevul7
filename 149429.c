gst_h264_parse_sps_mvc_data (NalReader * nr, GstH264SPS * sps)
{
  GstH264SPSExtMVC *const mvc = &sps->extension.mvc;
  guint8 bit_equal_to_one;
  guint i, j, k;

  READ_UINT8 (nr, bit_equal_to_one, 1);
  if (!bit_equal_to_one)
    return FALSE;

  sps->extension_type = GST_H264_NAL_EXTENSION_MVC;

  READ_UE_MAX (nr, mvc->num_views_minus1, GST_H264_MAX_VIEW_COUNT - 1);

  mvc->view = g_new0 (GstH264SPSExtMVCView, mvc->num_views_minus1 + 1);
  if (!mvc->view)
    goto error_allocation_failed;

  for (i = 0; i <= mvc->num_views_minus1; i++)
    READ_UE_MAX (nr, mvc->view[i].view_id, GST_H264_MAX_VIEW_ID);

  for (i = 1; i <= mvc->num_views_minus1; i++) {
    /* for RefPicList0 */
    READ_UE_MAX (nr, mvc->view[i].num_anchor_refs_l0, 15);
    for (j = 0; j < mvc->view[i].num_anchor_refs_l0; j++) {
      READ_UE_MAX (nr, mvc->view[i].anchor_ref_l0[j], GST_H264_MAX_VIEW_ID);
    }

    /* for RefPicList1 */
    READ_UE_MAX (nr, mvc->view[i].num_anchor_refs_l1, 15);
    for (j = 0; j < mvc->view[i].num_anchor_refs_l1; j++) {
      READ_UE_MAX (nr, mvc->view[i].anchor_ref_l1[j], GST_H264_MAX_VIEW_ID);
    }
  }

  for (i = 1; i <= mvc->num_views_minus1; i++) {
    /* for RefPicList0 */
    READ_UE_MAX (nr, mvc->view[i].num_non_anchor_refs_l0, 15);
    for (j = 0; j < mvc->view[i].num_non_anchor_refs_l0; j++) {
      READ_UE_MAX (nr, mvc->view[i].non_anchor_ref_l0[j], GST_H264_MAX_VIEW_ID);
    }

    /* for RefPicList1 */
    READ_UE_MAX (nr, mvc->view[i].num_non_anchor_refs_l1, 15);
    for (j = 0; j < mvc->view[i].num_non_anchor_refs_l1; j++) {
      READ_UE_MAX (nr, mvc->view[i].non_anchor_ref_l1[j], GST_H264_MAX_VIEW_ID);
    }
  }

  READ_UE_MAX (nr, mvc->num_level_values_signalled_minus1, 63);

  mvc->level_value =
      g_new0 (GstH264SPSExtMVCLevelValue,
      mvc->num_level_values_signalled_minus1 + 1);
  if (!mvc->level_value)
    goto error_allocation_failed;

  for (i = 0; i <= mvc->num_level_values_signalled_minus1; i++) {
    GstH264SPSExtMVCLevelValue *const level_value = &mvc->level_value[i];

    READ_UINT8 (nr, level_value->level_idc, 8);

    READ_UE_MAX (nr, level_value->num_applicable_ops_minus1, 1023);
    level_value->applicable_op =
        g_new0 (GstH264SPSExtMVCLevelValueOp,
        level_value->num_applicable_ops_minus1 + 1);
    if (!level_value->applicable_op)
      goto error_allocation_failed;

    for (j = 0; j <= level_value->num_applicable_ops_minus1; j++) {
      GstH264SPSExtMVCLevelValueOp *const op = &level_value->applicable_op[j];

      READ_UINT8 (nr, op->temporal_id, 3);

      READ_UE_MAX (nr, op->num_target_views_minus1, 1023);
      op->target_view_id = g_new (guint16, op->num_target_views_minus1 + 1);
      if (!op->target_view_id)
        goto error_allocation_failed;

      for (k = 0; k <= op->num_target_views_minus1; k++)
        READ_UE_MAX (nr, op->target_view_id[k], GST_H264_MAX_VIEW_ID);
      READ_UE_MAX (nr, op->num_views_minus1, 1023);
    }
  }
  return TRUE;

error_allocation_failed:
  GST_WARNING ("failed to allocate memory");
  gst_h264_sps_clear (sps);
  return FALSE;

error:
  gst_h264_sps_clear (sps);
  return FALSE;
}