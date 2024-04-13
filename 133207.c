static int x3f_get1(LibRaw_abstract_datastream *f)
{
  /* Little endian file */
  return f->get_char();
}