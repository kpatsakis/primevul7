gdev_mjc_map_color_rgb(gx_device *pdev, gx_color_index color,
                            gx_color_value prgb[3])
{
  /* For the moment, we simply ignore any black correction */
  switch (pdev->color_info.depth) {
  case 1:
    prgb[0] = prgb[1] = prgb[2] = -((gx_color_value)color ^ 1);
    break;
  case 8:
      if (pdev->color_info.num_components >= 3)
        { gx_color_value c = (gx_color_value)color ^ 7;
          prgb[0] = -(c & 1);
          prgb[1] = -((c >> 1) & 1);
          prgb[2] = -(c >> 2);
        }
      else
        { gx_color_value value = (gx_color_value)color ^ 0xff;
          prgb[0] = prgb[1] = prgb[2] = (value << 8) + value;
        }
    break;
  case 16:
    { gx_color_value c = (gx_color_value)color ^ 0xffff;
      ushort value = c >> 11;
      prgb[0] = ((value << 11) + (value << 6) + (value << 1) +
                 (value >> 4)) >> (16 - gx_color_value_bits);
      value = (c >> 6) & 0x3f;
      prgb[1] = ((value << 10) + (value << 4) + (value >> 2))
        >> (16 - gx_color_value_bits);
      value = c & 0x1f;
      prgb[2] = ((value << 11) + (value << 6) + (value << 1) +
                 (value >> 4)) >> (16 - gx_color_value_bits);
    }
    break;
  case 24:
    { gx_color_value c = (gx_color_value)color ^ 0xffffff;
      prgb[0] = gx_color_value_from_byte(c >> 16);
      prgb[1] = gx_color_value_from_byte((c >> 8) & 0xff);
      prgb[2] = gx_color_value_from_byte(c & 0xff);
    }
    break;
  case 32:
#define  gx_maxcol gx_color_value_from_byte(gx_color_value_to_byte(gx_max_color_value))
    { gx_color_value w = gx_maxcol - gx_color_value_from_byte(color >> 24);
      prgb[0] = w - gx_color_value_from_byte((color >> 16) & 0xff);
      prgb[1] = w - gx_color_value_from_byte((color >> 8) & 0xff);
      prgb[2] = w - gx_color_value_from_byte(color & 0xff);
    }
    break;
  }
  return 0;
}