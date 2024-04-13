QPDFObjectHandle::newStream(QPDF* qpdf)
{
    QTC::TC("qpdf", "QPDFObjectHandle newStream");
    QPDFObjectHandle stream_dict = newDictionary();
    QPDFObjectHandle result = qpdf->makeIndirectObject(
	QPDFObjectHandle(
	    new QPDF_Stream(qpdf, 0, 0, stream_dict, 0, 0)));
    result.dereference();
    QPDF_Stream* stream = dynamic_cast<QPDF_Stream*>(result.obj.getPointer());
    stream->setObjGen(result.getObjectID(), result.getGeneration());
    return result;
}