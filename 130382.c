QPDFObjectHandle::replaceStreamData(PointerHolder<Buffer> data,
				    QPDFObjectHandle const& filter,
				    QPDFObjectHandle const& decode_parms)
{
    assertStream();
    dynamic_cast<QPDF_Stream*>(obj.getPointer())->replaceStreamData(
	data, filter, decode_parms);
}