QPDFObjectHandle::getKeys()
{
    assertDictionary();
    return dynamic_cast<QPDF_Dictionary*>(obj.getPointer())->getKeys();
}