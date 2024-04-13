SPICE_GNUC_VISIBLE SpiceImageCompression spice_server_get_image_compression(SpiceServer *s)
{
    return s->config->image_compression;
}