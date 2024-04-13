_get_gdef (hb_face_t *face)
{
  return HB_LIKELY (face->ot_layout.gdef) ? *face->ot_layout.gdef : Null(GDEF);
}