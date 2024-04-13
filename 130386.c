QPDFObjectHandle::getStreamData(qpdf_stream_decode_level_e level)
{
    assertStream();
    return dynamic_cast<QPDF_Stream*>(obj.getPointer())->getStreamData(level);
}