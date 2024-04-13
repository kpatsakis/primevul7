QPDFObjectHandle::getTypeName()
{
    if (this->initialized)
    {
        dereference();
        return obj->getTypeName();
    }
    else
    {
        return "uninitialized";
    }
}