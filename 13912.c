void reds_on_sv_change(RedsState *reds)
{
    int compression_level = calc_compression_level(reds);

    FOREACH_QXL_INSTANCE(reds, qxl) {
        red_qxl_set_compression_level(qxl, compression_level);
        red_qxl_on_sv_change(qxl, reds_get_streaming_video(reds));
    }
}