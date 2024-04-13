QPDFObjectHandle::isInlineImage()
{
    dereference();
    return QPDFObjectTypeAccessor<QPDF_InlineImage>::check(obj.getPointer());
}