QPDFObjectHandle::replaceOrRemoveKey(std::string const& key,
				     QPDFObjectHandle value)
{
    assertDictionary();
    return dynamic_cast<QPDF_Dictionary*>(
	obj.getPointer())->replaceOrRemoveKey(key, value);
}