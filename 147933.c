gdev_mjc_encode_color(gx_device *dev, const gx_color_value colors[])
{
    return gdev_mjc_map_rgb_color(dev, colors);
}