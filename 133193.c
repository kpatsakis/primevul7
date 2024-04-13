/* extern */ int64_t x3f_load_data_size(x3f_t *x3f, x3f_directory_entry_t *DE)
{
  x3f_info_t *I = &x3f->info;

  if (DE == NULL)
    return -1;

  switch (DE->header.identifier)
  {
  case X3F_SECi:
    return x3f_load_image_size(I, DE);
  default:
    return 0;
  }
}