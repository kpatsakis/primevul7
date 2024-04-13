print_as_hex(FILE *output_file, const char *str, ulong len, ulong total_bytes_to_send)
{
  const char *ptr= str, *end= ptr+len;
  ulong i;
  fprintf(output_file, "0x");
  for(; ptr < end; ptr++)
    fprintf(output_file, "%02X", *((uchar*)ptr));
  for (i= 2*len+2; i < total_bytes_to_send; i++)
    tee_putc((int)' ', output_file);
}