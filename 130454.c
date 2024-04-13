QPDFObjectHandle::setArrayFromVector(std::vector<QPDFObjectHandle> const& items)
{
    assertArray();
    return dynamic_cast<QPDF_Array*>(obj.getPointer())->setFromVector(items);
}