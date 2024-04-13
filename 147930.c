gdev_mjc_map_rgb_color(gx_device *pdev, const gx_color_value cv[])
{
  gx_color_value r, g, b;

  r = cv[0]; g = cv[1]; b = cv[2];
  if (gx_color_value_to_byte(r & g & b) == 0xff)
    return (gx_color_index)0;         /* white */
  else {
    gx_color_value c = gx_max_color_value - r;
    gx_color_value m = gx_max_color_value - g;
    gx_color_value y = gx_max_color_value - b;

    switch (pdev->color_info.depth) {
    case 1:
      return ((c | m | y) > gx_max_color_value / 2 ?
              (gx_color_index)1 : (gx_color_index)0);
    case 8:
      if (pdev->color_info.num_components >= 3)
#define gx_color_value_to_1bit(cv) ((cv) >> (gx_color_value_bits - 1))
        return (gx_color_value_to_1bit(c) +
                (gx_color_value_to_1bit(m) << 1) +
                (gx_color_value_to_1bit(y) << 2));
      else
#define red_weight 306
#define green_weight 601
#define blue_weight 117
        return ((((word)c * red_weight +
                  (word)m * green_weight +
                  (word)y * blue_weight)
                 >> (gx_color_value_bits + 2)));
    case 16:
#define gx_color_value_to_5bits(cv) ((cv) >> (gx_color_value_bits - 5))
#define gx_color_value_to_6bits(cv) ((cv) >> (gx_color_value_bits - 6))
      return (gx_color_value_to_5bits(y) +
              (gx_color_value_to_6bits(m) << 5) +
              (gx_color_value_to_5bits(c) << 11));
    case 24:
      return (gx_color_value_to_byte(y) +
              (gx_color_value_to_byte(m) << 8) +
              ((word)gx_color_value_to_byte(c) << 16));
    case 32:
        {
                gx_color_value k;
                c = gx_color_value_to_byte(r);
                m = gx_color_value_to_byte(g);
                y = gx_color_value_to_byte(b);

                mj_color_correct( &c , &m , &y );

                c = esp_dat_c[c];
                m = esp_dat_m[m];
                y = esp_dat_y[y];

                k = c <= m ? (c <= y ? c : y) : (m <= y ? m : y);
                k = black_sep[ k >> 4 ] >> 6;
                c >>= 6;
                m >>= 6;
                y >>= 6;

        return ( (y - k) + ((m - k) << 8) +
                ((word)(c - k) << 16) + ((word)(k) << 24) );
      }
    }
  }
  return (gx_color_index)0;   /* This never happens */
}