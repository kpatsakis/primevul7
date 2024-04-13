slice_parse_ref_pic_list_modification_1 (GstH264SliceHdr * slice,
    NalReader * nr, guint list, gboolean is_mvc)
{
  GstH264RefPicListModification *entries;
  guint8 *ref_pic_list_modification_flag, *n_ref_pic_list_modification;
  guint32 modification_of_pic_nums_idc;
  gsize max_entries;
  guint i = 0;

  if (list == 0) {
    entries = slice->ref_pic_list_modification_l0;
    max_entries = G_N_ELEMENTS (slice->ref_pic_list_modification_l0);
    ref_pic_list_modification_flag = &slice->ref_pic_list_modification_flag_l0;
    n_ref_pic_list_modification = &slice->n_ref_pic_list_modification_l0;
  } else {
    entries = slice->ref_pic_list_modification_l1;
    max_entries = G_N_ELEMENTS (slice->ref_pic_list_modification_l1);
    ref_pic_list_modification_flag = &slice->ref_pic_list_modification_flag_l1;
    n_ref_pic_list_modification = &slice->n_ref_pic_list_modification_l1;
  }

  READ_UINT8 (nr, *ref_pic_list_modification_flag, 1);
  if (*ref_pic_list_modification_flag) {
    while (1) {
      READ_UE (nr, modification_of_pic_nums_idc);
      if (modification_of_pic_nums_idc == 0 ||
          modification_of_pic_nums_idc == 1) {
        READ_UE_MAX (nr, entries[i].value.abs_diff_pic_num_minus1,
            slice->max_pic_num - 1);
      } else if (modification_of_pic_nums_idc == 2) {
        READ_UE (nr, entries[i].value.long_term_pic_num);
      } else if (is_mvc && (modification_of_pic_nums_idc == 4 ||
              modification_of_pic_nums_idc == 5)) {
        READ_UE (nr, entries[i].value.abs_diff_view_idx_minus1);
      }
      entries[i++].modification_of_pic_nums_idc = modification_of_pic_nums_idc;
      if (modification_of_pic_nums_idc == 3)
        break;
      if (i >= max_entries)
        goto error;
    }
  }
  *n_ref_pic_list_modification = i;
  return TRUE;

error:
  GST_WARNING ("error parsing \"Reference picture list %u modification\"",
      list);
  return FALSE;
}