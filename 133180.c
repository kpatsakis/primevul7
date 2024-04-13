static void x3f_load_camf(x3f_info_t *I, x3f_directory_entry_t *DE)
{
  x3f_directory_entry_header_t *DEH = &DE->header;
  x3f_camf_t *CAMF = &DEH->data_subsection.camf;

  read_data_set_offset(I, DE, X3F_CAMF_HEADER_SIZE);

  if (!CAMF->data_size)
    CAMF->data_size = read_data_block(&CAMF->data, I, DE, 0);

  switch (CAMF->type)
  {
  case 2: /* Older SD9-SD14 */
    x3f_load_camf_decode_type2(CAMF);
    break;
  case 4: /* TRUE ... Merrill */
    x3f_load_camf_decode_type4(CAMF);
    break;
  case 5: /* Quattro ... */
    x3f_load_camf_decode_type5(CAMF);
    break;
  default:
    /* TODO: Shouldn't this be treated as a fatal error? */
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
  }

  if (CAMF->decoded_data != NULL)
    x3f_setup_camf_entries(CAMF);
  else
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
}