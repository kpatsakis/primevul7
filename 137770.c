read_comment_extension(Gif_Image *gfi, Gif_Reader *grr)
{
  int len;
  Gif_Comment *gfcom = gfi->comment;
  char *m = suck_data(0, &len, grr);
  if (m) {
    if (!gfcom)
      gfcom = gfi->comment = Gif_NewComment();
    if (!gfcom || !Gif_AddCommentTake(gfcom, m, len))
      return 0;
  }
  return 1;
}