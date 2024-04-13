_get_gsub (hb_face_t *face)
{
  return HB_LIKELY (face->ot_layout.gsub) ? *face->ot_layout.gsub : Null(GSUB);
}