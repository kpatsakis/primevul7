QPDFObjectHandle::insertItem(int at, QPDFObjectHandle const& item)
{
    assertArray();
    return dynamic_cast<QPDF_Array*>(obj.getPointer())->insertItem(at, item);
}