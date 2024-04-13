hb_ot_layout_substitute_lookup (hb_face_t    *face,
				hb_buffer_t  *buffer,
				unsigned int  lookup_index,
				hb_mask_t     mask)
{
  hb_ot_layout_context_t context;
  context.font = NULL;
  context.face = face;
  return _get_gsub (face).substitute_lookup (&context, buffer, lookup_index, mask);
}