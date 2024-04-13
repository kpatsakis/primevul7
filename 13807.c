static char* parse_next_video_codec(char *codecs, char **encoder, char **codec)
{
    if (!codecs) {
        return NULL;
    }
    codecs += strspn(codecs, ";");
    if (!*codecs) {
        return NULL;
    }
    int end_encoder, end_codec = -1;
    *encoder = *codec = NULL;
    if (sscanf(codecs, "%*[0-9a-zA-Z_]:%n%*[0-9a-zA-Z_];%n", &end_encoder, &end_codec) == 0
        && end_codec > 0) {
        codecs[end_encoder - 1] = '\0';
        codecs[end_codec - 1] = '\0';
        *encoder = codecs;
        *codec = codecs + end_encoder;
        return codecs + end_codec;
    }
    return codecs + strcspn(codecs, ";");
}