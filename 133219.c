static uint32_t data_block_size(void **data, x3f_info_t *I,
                                x3f_directory_entry_t *DE, uint32_t footer)
{
  uint32_t size =
      DE->input.size + DE->input.offset - I->input.file->tell() - footer;
  return size;
}