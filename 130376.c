QPDFObjectHandle::eraseItem(int at)
{
    assertArray();
    return dynamic_cast<QPDF_Array*>(obj.getPointer())->eraseItem(at);
}