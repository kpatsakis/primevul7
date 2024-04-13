QPDFObjectHandle::getInlineImageValue()
{
    assertInlineImage();
    return dynamic_cast<QPDF_InlineImage*>(obj.getPointer())->getVal();
}