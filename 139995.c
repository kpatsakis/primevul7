hb_ot_layout_has_substitution (hb_face_t *face)
{
  return &_get_gsub (face) != &Null(GSUB);
}