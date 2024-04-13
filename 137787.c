read_unknown_extension(Gif_Context* gfc, Gif_Reader* grr,
                       int kind, char* appname, int applength)
{
    uint8_t block_len = gifgetbyte(grr);
    uint8_t* data = 0;
    int data_len = 0;
    Gif_Extension *gfex = 0;

    while (block_len > 0) {
        Gif_ReArray(data, uint8_t, data_len + block_len + 2);
        if (!data)
            goto done;
        data[data_len] = block_len;
        gifgetblock(data + data_len + 1, block_len, grr);
        data_len += block_len + 1;
        block_len = gifgetbyte(grr);
    }

    if (data)
        gfex = Gif_NewExtension(kind, appname, applength);
    if (gfex) {
        gfex->data = data;
        gfex->free_data = Gif_Free;
        gfex->length = data_len;
        gfex->packetized = 1;
        data[data_len] = 0;
        Gif_AddExtension(gfc->stream, gfc->gfi, gfex);
    }

 done:
    if (!gfex)
        Gif_DeleteArray(data);
    while (block_len > 0) {
        uint8_t buffer[GIF_MAX_BLOCK];
        gifgetblock(buffer, block_len, grr);
        block_len = gifgetbyte(grr);
    }
    return gfex != 0;
}