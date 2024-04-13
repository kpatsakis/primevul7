hb_ot_layout_position_lookup   (hb_face_t    *face,
				hb_font_t    *font,
				hb_buffer_t  *buffer,
				unsigned int  lookup_index,
				hb_mask_t     mask)
{
  hb_ot_layout_context_t context;
  context.font = font;
  context.face = face;
  return _get_gpos (face).position_lookup (&context, buffer, lookup_index, mask);
}