QPDFObjectHandle::appendItem(QPDFObjectHandle const& item)
{
    assertArray();
    return dynamic_cast<QPDF_Array*>(obj.getPointer())->appendItem(item);
}