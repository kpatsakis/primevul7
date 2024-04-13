mj_v_skip(int n, gx_device_printer *pdev, gp_file *stream)
{
        /* This is a kind of magic number. */
  static const int max_y_step = (256 * 15 + 255);

  int l = n - max_y_step;
  for (; l > 0; l -= max_y_step) {    /* move 256 * 15 + 255 dots at once*/
    gp_fwrite("\033(v\2\0\xff\x0f", sizeof(byte), 7, stream);
  }
  l += max_y_step;
  /* move to the end. */
  {
    int n2 = l / 256;
    int n1 = l - n2 * 256;
    gp_fwrite("\033(v\2\0", sizeof(byte) ,5 ,stream);
    gp_fputc(n1, stream);
    gp_fputc(n2, stream);
    gp_fputc('\r', stream);
  }
  return 0;
}