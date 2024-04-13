QPDFObjectHandle::removeKey(std::string const& key)
{
    assertDictionary();
    return dynamic_cast<QPDF_Dictionary*>(obj.getPointer())->removeKey(key);
}