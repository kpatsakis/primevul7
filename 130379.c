QPDFObjectHandle::replaceDict(QPDFObjectHandle new_dict)
{
    assertStream();
    dynamic_cast<QPDF_Stream*>(obj.getPointer())->replaceDict(new_dict);
}