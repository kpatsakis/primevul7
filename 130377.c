QPDFObjectHandle::isReserved()
{
    // dereference will clear reserved if this has been replaced
    dereference();
    return this->reserved;
}