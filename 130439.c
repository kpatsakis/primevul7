QPDFObjectHandle::getName()
{
    assertName();
    return dynamic_cast<QPDF_Name*>(obj.getPointer())->getName();
}