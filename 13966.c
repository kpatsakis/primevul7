SPICE_GNUC_VISIBLE int spice_server_set_image_compression(SpiceServer *s,
                                                          SpiceImageCompression comp)
{
#ifndef USE_LZ4
    if (comp == SPICE_IMAGE_COMPRESSION_LZ4) {
        spice_warning("LZ4 compression not supported, falling back to auto GLZ");
        comp = SPICE_IMAGE_COMPRESSION_AUTO_GLZ;
        reds_config_set_image_compression(s, comp);
        return -1;
    }
#endif
    reds_config_set_image_compression(s, comp);
    return 0;
}