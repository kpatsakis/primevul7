_hb_ot_layout_set_glyph_property (hb_face_t      *face,
				  hb_codepoint_t  glyph,
				  unsigned int    property)
{ _hb_ot_layout_set_glyph_class (face, glyph, (hb_ot_layout_glyph_class_t) (property & 0xff)); }