QPDFObjectHandle::getRealValue()
{
    assertReal();
    return dynamic_cast<QPDF_Real*>(obj.getPointer())->getVal();
}