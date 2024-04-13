void reds_on_vc_change(RedsState *reds)
{
    FOREACH_QXL_INSTANCE(reds, qxl) {
        red_qxl_on_vc_change(qxl, reds_get_video_codecs(reds));
    }
}