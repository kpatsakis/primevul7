make_data_reader(Gif_Reader *grr, const uint8_t *data, uint32_t length)
{
  grr->v = data;
  grr->pos = 0;
  grr->length = length;
  grr->is_record = 1;
  grr->byte_getter = record_byte_getter;
  grr->block_getter = record_block_getter;
  grr->eofer = record_eofer;
}