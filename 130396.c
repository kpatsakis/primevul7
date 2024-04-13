QPDFObjectHandle::getArrayAsVector()
{
    assertArray();
    return dynamic_cast<QPDF_Array*>(obj.getPointer())->getAsVector();
}