QPDFObjectHandle::replaceKey(std::string const& key,
			    QPDFObjectHandle const& value)
{
    assertDictionary();
    return dynamic_cast<QPDF_Dictionary*>(
	obj.getPointer())->replaceKey(key, value);
}