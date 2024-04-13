hb_ot_layout_table_get_script_tags (hb_face_t    *face,
				    hb_tag_t      table_tag,
				    unsigned int *script_count /* IN/OUT */,
				    hb_tag_t     *script_tags /* OUT */)
{
  const GSUBGPOS &g = get_gsubgpos_table (face, table_tag);

  return g.get_script_tags (script_count, script_tags);
}