read_color_table(int size, Gif_Reader *grr)
{
  Gif_Colormap *gfcm = Gif_NewFullColormap(size, size);
  Gif_Color *c;
  if (!gfcm) return 0;

  GIF_DEBUG(("colormap(%d) ", size));
  for (c = gfcm->col; size; size--, c++) {
    c->gfc_red = gifgetbyte(grr);
    c->gfc_green = gifgetbyte(grr);
    c->gfc_blue = gifgetbyte(grr);
    c->haspixel = 0;
  }

  return gfcm;
}