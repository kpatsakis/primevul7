QPDFObjectHandle::getDict()
{
    assertStream();
    return dynamic_cast<QPDF_Stream*>(obj.getPointer())->getDict();
}