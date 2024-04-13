_hb_ot_layout_set_glyph_class (hb_face_t                  *face,
			       hb_codepoint_t              glyph,
			       hb_ot_layout_glyph_class_t  klass)
{
  if (HB_OBJECT_IS_INERT (face))
    return;

  /* TODO optimize this? similar to old harfbuzz code for example */

  hb_ot_layout_t *layout = &face->ot_layout;
  hb_ot_layout_class_t gdef_klass;
  unsigned int len = layout->new_gdef.len;

  if (HB_UNLIKELY (glyph > 65535))
    return;

  /* XXX this is not threadsafe */
  if (glyph >= len) {
    unsigned int new_len;
    unsigned char *new_klasses;

    new_len = len == 0 ? 120 : 2 * len;
    while (new_len <= glyph)
      new_len *= 2;

    if (new_len > 65536)
      new_len = 65536;
    new_klasses = (unsigned char *) realloc (layout->new_gdef.klasses, new_len * sizeof (unsigned char));

    if (HB_UNLIKELY (!new_klasses))
      return;

    memset (new_klasses + len, 0, new_len - len);

    layout->new_gdef.klasses = new_klasses;
    layout->new_gdef.len = new_len;
  }

  switch (klass) {
  default:
  case HB_OT_LAYOUT_GLYPH_CLASS_UNCLASSIFIED:	gdef_klass = GDEF::UnclassifiedGlyph;	break;
  case HB_OT_LAYOUT_GLYPH_CLASS_BASE_GLYPH:	gdef_klass = GDEF::BaseGlyph;		break;
  case HB_OT_LAYOUT_GLYPH_CLASS_LIGATURE:	gdef_klass = GDEF::LigatureGlyph;	break;
  case HB_OT_LAYOUT_GLYPH_CLASS_MARK:		gdef_klass = GDEF::MarkGlyph;		break;
  case HB_OT_LAYOUT_GLYPH_CLASS_COMPONENT:	gdef_klass = GDEF::ComponentGlyph;	break;
  }

  layout->new_gdef.klasses[glyph] = gdef_klass;
  return;
}