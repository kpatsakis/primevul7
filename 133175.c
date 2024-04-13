unsigned x3f_get4(LibRaw_abstract_datastream *f)
{
  uchar str[4] = {0xff, 0xff, 0xff, 0xff};
  f->read(str, 1, 4);
  return x3f_sget4(str);
}