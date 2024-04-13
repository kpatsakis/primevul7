hb_ot_layout_has_font_glyph_classes (hb_face_t *face)
{
  return _get_gdef (face).has_glyph_classes ();
}