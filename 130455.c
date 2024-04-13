QPDFObjectHandle::getBoolValue()
{
    assertBool();
    return dynamic_cast<QPDF_Bool*>(obj.getPointer())->getVal();
}