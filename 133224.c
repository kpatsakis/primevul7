static uint32_t x3f_load_image_size(x3f_info_t *I, x3f_directory_entry_t *DE)
{
  x3f_directory_entry_header_t *DEH = &DE->header;
  x3f_image_data_t *ID = &DEH->data_subsection.image_data;

  read_data_set_offset(I, DE, X3F_IMAGE_HEADER_SIZE);

  switch (ID->type_format)
  {
  case X3F_IMAGE_THUMB_PLAIN:
    return x3f_load_pixmap_size(I, DE);
  case X3F_IMAGE_THUMB_JPEG:
    return x3f_load_jpeg_size(I, DE);
    break;
  default:
    return 0;
  }
}