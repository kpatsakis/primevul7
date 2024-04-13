_get_gpos (hb_face_t *face)
{
  return HB_LIKELY (face->ot_layout.gpos) ? *face->ot_layout.gpos : Null(GPOS);
}