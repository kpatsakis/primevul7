static uint32_t read_data_block(void **data, x3f_info_t *I,
                                x3f_directory_entry_t *DE, uint32_t footer)
{
  INT64 fpos = I->input.file->tell();
  uint32_t size = DE->input.size + DE->input.offset - fpos - footer;

  if (fpos + size > I->input.file->size())
    throw LIBRAW_EXCEPTION_IO_CORRUPT;

  *data = (void *)malloc(size);

  GETN(*data, size);

  return size;
}