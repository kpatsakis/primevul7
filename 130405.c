QPDFObjectHandle::getOperatorValue()
{
    assertOperator();
    return dynamic_cast<QPDF_Operator*>(obj.getPointer())->getVal();
}