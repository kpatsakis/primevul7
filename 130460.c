QPDFObjectHandle::getArrayItem(int n)
{
    assertArray();
    return dynamic_cast<QPDF_Array*>(obj.getPointer())->getItem(n);
}