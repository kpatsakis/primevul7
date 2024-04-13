/* extern */ x3f_return_t x3f_load_image_block(x3f_t *x3f,
                                               x3f_directory_entry_t *DE)
{
  x3f_info_t *I = &x3f->info;

  if (DE == NULL)
    return X3F_ARGUMENT_ERROR;

  switch (DE->header.identifier)
  {
  case X3F_SECi:
    read_data_set_offset(I, DE, X3F_IMAGE_HEADER_SIZE);
    x3f_load_image_verbatim(I, DE);
    break;
  default:
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
    return X3F_INTERNAL_ERROR;
  }

  return X3F_OK;
}