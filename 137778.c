file_eofer(Gif_Reader *grr)
{
  int c = getc(grr->f);
  if (c == EOF)
    return 1;
  else {
    ungetc(c, grr->f);
    return 0;
  }
}