QPDFObjectHandle::isOperator()
{
    dereference();
    return QPDFObjectTypeAccessor<QPDF_Operator>::check(obj.getPointer());
}