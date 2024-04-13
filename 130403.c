QPDFObjectHandle::getStringValue()
{
    assertString();
    return dynamic_cast<QPDF_String*>(obj.getPointer())->getVal();
}