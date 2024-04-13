QPDFObjectHandle::isIndirect()
{
    assertInitialized();
    return (this->objid != 0);
}