_hb_ot_layout_skip_mark (hb_face_t    *face,
			 hb_internal_glyph_info_t *ginfo,
			 unsigned int  lookup_flags,
			 unsigned int *property_out)
{
  unsigned int property;

  if (ginfo->gproperty == HB_BUFFER_GLYPH_PROPERTIES_UNKNOWN)
    ginfo->gproperty = _hb_ot_layout_get_glyph_property (face, ginfo->codepoint);
  property = ginfo->gproperty;
  if (property_out)
    *property_out = property;

  if (property & HB_OT_LAYOUT_GLYPH_CLASS_MARK)
  {
    /* Skip mark if lookup_flags includes LookupFlags::IgnoreMarks */
    if (lookup_flags & LookupFlag::IgnoreMarks)
      return true;

    /* If using mark filtering sets, the high short of lookup_flags has the set index. */
    if (lookup_flags & LookupFlag::UseMarkFilteringSet)
      return !_get_gdef (face).mark_set_covers (lookup_flags >> 16, ginfo->codepoint);

    /* The second byte of lookup_flags has the meaning "ignore marks of attachment type
     * different than the attachment type specified." */
    if (lookup_flags & LookupFlag::MarkAttachmentType && property & LookupFlag::MarkAttachmentType)
      return (lookup_flags & LookupFlag::MarkAttachmentType) != (property & LookupFlag::MarkAttachmentType);
  }

  return false;
}