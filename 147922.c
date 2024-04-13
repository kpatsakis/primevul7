gdev_mjc_decode_color(gx_device * dev, gx_color_index color, gx_color_value * out)
{
    return gdev_mjc_map_color_rgb(dev, color, out);
}