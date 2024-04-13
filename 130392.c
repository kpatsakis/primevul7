QPDFObjectHandle::getDictAsMap()
{
    assertDictionary();
    return dynamic_cast<QPDF_Dictionary*>(obj.getPointer())->getAsMap();
}