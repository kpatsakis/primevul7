void reds_on_ic_change(RedsState *reds)
{
    int compression_level = calc_compression_level(reds);

    FOREACH_QXL_INSTANCE(reds, qxl) {
        red_qxl_set_compression_level(qxl, compression_level);
        red_qxl_on_ic_change(qxl, spice_server_get_image_compression(reds));
    }
}