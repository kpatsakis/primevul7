QPDFObjectHandle::getIntValue()
{
    assertInteger();
    return dynamic_cast<QPDF_Integer*>(obj.getPointer())->getVal();
}