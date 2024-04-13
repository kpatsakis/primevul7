hb_ot_layout_get_glyph_class (hb_face_t      *face,
			      hb_codepoint_t  glyph)
{
  return (hb_ot_layout_glyph_class_t) (_hb_ot_layout_get_glyph_property (face, glyph) & 0xff);
}