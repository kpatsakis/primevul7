QPDFObjectHandle::parseContentStream(QPDFObjectHandle stream_or_array,
                                     ParserCallbacks* callbacks)
{
    std::vector<QPDFObjectHandle> streams;
    if (stream_or_array.isArray())
    {
        streams = stream_or_array.getArrayAsVector();
    }
    else
    {
        streams.push_back(stream_or_array);
    }
    Pl_Buffer buf("concatenated stream data buffer");
    std::string all_description = "content stream objects";
    bool first = true;
    for (std::vector<QPDFObjectHandle>::iterator iter = streams.begin();
         iter != streams.end(); ++iter)
    {
        QPDFObjectHandle stream = *iter;
        if (! stream.isStream())
        {
            QTC::TC("qpdf", "QPDFObjectHandle non-stream in parsecontent");
            warn(stream.getOwningQPDF(),
                 QPDFExc(qpdf_e_damaged_pdf, "content stream",
                         "", 0,
                         "ignoring non-stream while parsing content streams"));
        }
        else
        {
            std::string og = QUtil::int_to_string(stream.getObjectID()) + " " +
                QUtil::int_to_string(stream.getGeneration());
            std::string description = "content stream object " + og;
            if (first)
            {
                first = false;
            }
            else
            {
                all_description += ",";
            }
            all_description += " " + og;
            if (! stream.pipeStreamData(&buf, 0, qpdf_dl_specialized))
            {
                QTC::TC("qpdf", "QPDFObjectHandle errors in parsecontent");
                warn(stream.getOwningQPDF(),
                     QPDFExc(qpdf_e_damaged_pdf, "content stream",
                             description, 0,
                             "errors while decoding content stream"));
            }
        }
    }
    PointerHolder<Buffer> stream_data = buf.getBuffer();
    try
    {
        parseContentStream_internal(stream_data, all_description, callbacks);
    }
    catch (TerminateParsing&)
    {
        return;
    }
    callbacks->handleEOF();
}