hb_ot_layout_has_positioning (hb_face_t *face)
{
  return &_get_gpos (face) != &Null(GPOS);
}