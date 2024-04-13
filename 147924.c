set_bpp(gx_device *pdev, int bits_per_pixel)
{ gx_device_color_info *ci = &pdev->color_info;
  /* Only valid bits-per-pixel are 1, 3, 8, 16, 24, 32 */
  int bpp = bits_per_pixel < 3 ? 1 : bits_per_pixel < 8 ? 3 :
    (bits_per_pixel >> 3) << 3;
  ci->num_components = ((bpp == 1) || (bpp == 8) ? 1 : 3);
  ci->depth = ((bpp > 1) && (bpp < 8) ? 8 : bpp);
  ci->max_gray = (bpp >= 8 ? 255 : 1);
  ci->max_color = (bpp >= 8 ? 255 : bpp > 1 ? 1 : 0);
  ci->dither_grays = (bpp >= 8 ? 5 : 2);
  ci->dither_colors = (bpp >= 8 ? 5 : bpp > 1 ? 2 : 0);
  mj->colorcomp = (bpp == 8 ? 1 : 4);
}