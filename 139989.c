_hb_ot_layout_fini (hb_face_t *face)
{
  hb_ot_layout_t *layout = &face->ot_layout;

  hb_blob_unlock (layout->gdef_blob);
  hb_blob_unlock (layout->gsub_blob);
  hb_blob_unlock (layout->gpos_blob);

  hb_blob_destroy (layout->gdef_blob);
  hb_blob_destroy (layout->gsub_blob);
  hb_blob_destroy (layout->gpos_blob);

  free (layout->new_gdef.klasses);
}