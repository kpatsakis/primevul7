static void read_data_set_offset(x3f_info_t *I, x3f_directory_entry_t *DE,
                                 uint32_t header_size)
{
  uint32_t i_off = DE->input.offset + header_size;

  I->input.file->seek(i_off, SEEK_SET);
}