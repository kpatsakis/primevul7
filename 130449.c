QPDFObjectHandle::replaceStreamData(PointerHolder<StreamDataProvider> provider,
				    QPDFObjectHandle const& filter,
				    QPDFObjectHandle const& decode_parms)
{
    assertStream();
    dynamic_cast<QPDF_Stream*>(obj.getPointer())->replaceStreamData(
	provider, filter, decode_parms);
}