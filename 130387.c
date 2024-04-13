QPDFObjectHandle::getUTF8Value()
{
    assertString();
    return dynamic_cast<QPDF_String*>(obj.getPointer())->getUTF8Val();
}