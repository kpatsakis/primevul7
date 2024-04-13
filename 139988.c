_hb_ot_layout_get_glyph_property (hb_face_t      *face,
				  hb_codepoint_t  glyph)
{
  hb_ot_layout_class_t klass;
  const GDEF &gdef = _get_gdef (face);

  klass = gdef.get_glyph_class (glyph);

  if (!klass && glyph < face->ot_layout.new_gdef.len)
    klass = face->ot_layout.new_gdef.klasses[glyph];

  switch (klass) {
  default:
  case GDEF::UnclassifiedGlyph:	return HB_OT_LAYOUT_GLYPH_CLASS_UNCLASSIFIED;
  case GDEF::BaseGlyph:		return HB_OT_LAYOUT_GLYPH_CLASS_BASE_GLYPH;
  case GDEF::LigatureGlyph:	return HB_OT_LAYOUT_GLYPH_CLASS_LIGATURE;
  case GDEF::ComponentGlyph:	return HB_OT_LAYOUT_GLYPH_CLASS_COMPONENT;
  case GDEF::MarkGlyph:
	klass = gdef.get_mark_attachment_type (glyph);
	return HB_OT_LAYOUT_GLYPH_CLASS_MARK + (klass << 8);
  }
}