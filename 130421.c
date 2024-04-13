QPDFObjectHandle::unparseResolved()
{
    if (this->reserved)
    {
        throw std::logic_error(
            "QPDFObjectHandle: attempting to unparse a reserved object");
    }
    dereference();
    return this->obj->unparse();
}