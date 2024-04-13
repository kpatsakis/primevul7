QPDFObjectHandle::getRawStreamData()
{
    assertStream();
    return dynamic_cast<QPDF_Stream*>(obj.getPointer())->getRawStreamData();
}