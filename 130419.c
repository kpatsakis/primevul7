QPDFObjectHandle::getArrayNItems()
{
    assertArray();
    return dynamic_cast<QPDF_Array*>(obj.getPointer())->getNItems();
}