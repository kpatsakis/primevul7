mjc_correct_color(gx_device_printer *pdev, gx_color_index ci)
{
  gx_color_index c, m, y, k, co;
  gx_color_index k2, k3, k4;
#if __WORDSIZE == 64
  gx_color_index c2, m2, y2, k5, k6, k7, k8;
#endif
  const uint cmask = 0xff;
  uint dn = mj->density;
  uint mjy = mj->yellow;
  uint mjc = mj->cyan;
  uint mjm = mj->magenta;
  uint mjb = mj->black;
  switch (pdev->color_info.depth) {
 case 8:
    k = ((ci & cmask) * (mjb)) >> 10;
    k = (k < cmask) ? k : cmask;
    k2 = (((ci >> 8) & cmask) * (mjb)) >> 10;
    k2 = (k2 < cmask) ? k2 : cmask;
    k3 = (((ci >> 16) & cmask) * (mjb)) >> 10;
    k3 = (k3 < cmask) ? k3 : cmask;
    k4 = (((ci >> 24) & cmask) * (mjb)) >> 10;
    k4 = (k4 < cmask) ? k4 : cmask;
#if __WORDSIZE == 64
    /* This code is ugly... (for 64 bit OS) */
    if (sizeof(word) == 8)
      {
        k5 = (((ci >> 32) & cmask) * (mjb)) >> 10;
        k5 = (k5 < cmask) ? k5 : cmask;
        k6 = (((ci >> 40) & cmask) * (mjb)) >> 10;
        k6 = (k6 < cmask) ? k6 : cmask;
        k7 = (((ci >> 48) & cmask) * (mjb)) >> 10;
        k7 = (k7 < cmask) ? k7 : cmask;
        k8 = (((ci >> 56) & cmask) * (mjb)) >> 10;
        k8 = (k8 < cmask) ? k8 : cmask;
        co = k + (k2 << 8) + (k3 << 16) + (k4 << 24)
          + (k5 << 32) + (k6 << 40) + (k7 << 48) + (k8 << 56);
        if (ci != co)
          dprintf1("%s\n", "error");
        return co;
      }
#endif
    return k + (k2 << 8) + (k3 << 16) + (k << 24);
    break;
 case 32:
    y = ((ci & cmask) * mjy * dn) >> 20;
    y = (y < cmask) ? y : cmask;
    m = (((ci >> 8) & cmask) * mjm * dn) >> 20;
    m = (m < cmask) ? m : cmask;
    c = (((ci >> 16) & cmask) * mjc * dn) >> 20;
    c = (c < cmask) ? c : cmask;
    k = (((ci >> 24) & cmask) * mjb * dn) >> 20;
    k = (k < cmask) ? k : cmask;
#if __WORDSIZE == 64
    /* This code is ugly... (for 64 bit OS) */
    if (sizeof(word) == 8)
      {
        y2 = (((ci >> 32) & cmask) * mjy * dn) >> 20;
        y2 = (y2 < cmask) ? y2 : cmask;
        m2 = (((ci >> 40) & cmask) * mjm * dn) >> 20;
        m2 = (m2 < cmask) ? m2 : cmask;
        c2 = (((ci >> 48) & cmask) * mjc * dn) >> 20;
        c2 = (c2 < cmask) ? c2 : cmask;
        k2 = (((ci >> 56) & cmask) * mjb * dn) >> 20;
        k2 = (k2 < cmask) ? k2 : cmask;

        co = y + (m << 8) + (c << 16) + (k << 24)
          + (y2 << 32) + (m2 << 40) + (c2 << 48) + (k2 << 56);

        return co;
      }
#endif
    co =    (y + (m << 8) + (c << 16) + (k << 24));
/*     dprintf6("%d,%d:%d,%d,%d,%d\n", ci,co, y, m, c, k); */
    return co;
/*    return (gx_color_value_to_byte(y) +
            (gx_color_value_to_byte(m) << 8) +
            ((word)gx_color_value_to_byte(c) << 16) +
            ((word)gx_color_value_to_byte(k) << 24)); */
    break;
  }
  return ci;
}