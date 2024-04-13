QPDFObjectHandle::pipeStreamData(Pipeline* p, bool filter,
				 bool normalize, bool compress)
{
    unsigned long encode_flags = 0;
    qpdf_stream_decode_level_e decode_level = qpdf_dl_none;
    if (filter)
    {
        decode_level = qpdf_dl_generalized;
        if (normalize)
        {
            encode_flags |= qpdf_ef_normalize;
        }
        if (compress)
        {
            encode_flags |= qpdf_ef_compress;
        }
    }
    return pipeStreamData(p, encode_flags, decode_level, false);
}