QPDFObjectHandle::getTypeCode()
{
    if (this->initialized)
    {
        dereference();
        return obj->getTypeCode();
    }
    else
    {
        return QPDFObject::ot_uninitialized;
    }
}