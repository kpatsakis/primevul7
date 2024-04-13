hb_ot_layout_get_lig_carets (hb_face_t      *face,
			     hb_font_t      *font,
			     hb_codepoint_t  glyph,
			     unsigned int   *caret_count /* IN/OUT */,
			     int            *caret_array /* OUT */)
{
  hb_ot_layout_context_t context;
  context.font = font;
  context.face = face;
  return _get_gdef (face).get_lig_carets (&context, glyph, caret_count, caret_array);
}