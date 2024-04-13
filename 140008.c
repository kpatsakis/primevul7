hb_ot_layout_language_get_feature_indexes (hb_face_t    *face,
					   hb_tag_t      table_tag,
					   unsigned int  script_index,
					   unsigned int  language_index,
					   unsigned int *feature_count /* IN/OUT */,
					   unsigned int *feature_indexes /* OUT */)
{
  const GSUBGPOS &g = get_gsubgpos_table (face, table_tag);
  const LangSys &l = g.get_script (script_index).get_lang_sys (language_index);

  return l.get_feature_indexes (feature_count, feature_indexes);
}