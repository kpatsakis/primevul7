static int calc_compression_level(RedsState *reds)
{
    spice_assert(reds_get_streaming_video(reds) != SPICE_STREAM_VIDEO_INVALID);

    if ((reds_get_streaming_video(reds) != SPICE_STREAM_VIDEO_OFF) ||
        (spice_server_get_image_compression(reds) != SPICE_IMAGE_COMPRESSION_QUIC)) {
        return 0;
    } else {
        return 1;
    }
}