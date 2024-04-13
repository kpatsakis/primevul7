_hb_ot_layout_has_new_glyph_classes (hb_face_t *face)
{
  return face->ot_layout.new_gdef.len > 0;
}