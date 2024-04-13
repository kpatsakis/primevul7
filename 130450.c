QPDFObjectHandle::getOwningQPDF()
{
    // Will be null for direct objects
    return this->qpdf;
}