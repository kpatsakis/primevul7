mj_open(gx_device *pdev, int ptype)
{       /* Change the margins if necessary. */
  int xdpi = pdev->x_pixels_per_inch;
  int ydpi = pdev->y_pixels_per_inch;

  static const float mj_margin[4] = { MJ700V2C_MARGINS_A4 };
  static const float mj6000c_a2[4] = { MJ6000C_MARGINS_A2 };
  static const float mj8000c_a2[4] = { MJ8000C_MARGINS_A2 };

  const float *m;

  int paper_size;

#if 0
  /* Set up colour params if put_props has not already done so */
  if (pdev->color_info.num_components == 0)
    set_bpp(pdev, pdev->color_info.depth);
#endif

  paper_size = gdev_mjc_paper_size(pdev);
  if (paper_size == PAPER_SIZE_A2 ) {
    if (ptype == MJ6000C)
      m = mj6000c_a2;
    else if (ptype == MJ8000C)
      m = mj8000c_a2;
    else
      m = mj_margin;
  } else {
    m = mj_margin;
  }

  gx_device_set_margins(pdev, m, true);

  if (mj->colorcomp == 3)
    mj->density = mj->density * 720 / ydpi * 1.5;
  else
    mj->density = mj->density * 720 / ydpi;

  /* Print Resolution Check */
  if (!((xdpi == 180 && ydpi == 180) ||
      (xdpi == 360 && ydpi == 360) ||
      (xdpi == 720 && ydpi == 720) ||
      (xdpi == 360 && ydpi == 720) ||
      (xdpi == 720 && ydpi == 360)))
    return_error(gs_error_rangecheck);

  return gdev_prn_open(pdev);
}