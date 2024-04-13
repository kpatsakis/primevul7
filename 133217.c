static int x3f_get2(LibRaw_abstract_datastream *f)
{
  uchar str[2] = {0xff, 0xff};
  f->read(str, 1, 2);
  return x3f_sget2(str);
}