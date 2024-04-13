QPDFObjectHandle::replaceStreamData(std::string const& data,
				    QPDFObjectHandle const& filter,
				    QPDFObjectHandle const& decode_parms)
{
    assertStream();
    PointerHolder<Buffer> b = new Buffer(data.length());
    unsigned char* bp = b->getBuffer();
    memcpy(bp, data.c_str(), data.length());
    dynamic_cast<QPDF_Stream*>(obj.getPointer())->replaceStreamData(
	b, filter, decode_parms);
}