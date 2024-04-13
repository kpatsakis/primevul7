slice_parse_ref_pic_list_modification (GstH264SliceHdr * slice, NalReader * nr,
    gboolean is_mvc)
{
  if (!GST_H264_IS_I_SLICE (slice) && !GST_H264_IS_SI_SLICE (slice)) {
    if (!slice_parse_ref_pic_list_modification_1 (slice, nr, 0, is_mvc))
      return FALSE;
  }

  if (GST_H264_IS_B_SLICE (slice)) {
    if (!slice_parse_ref_pic_list_modification_1 (slice, nr, 1, is_mvc))
      return FALSE;
  }
  return TRUE;
}