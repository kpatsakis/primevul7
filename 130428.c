QPDFObjectHandle::isPagesObject()
{
    return (this->isDictionary() && this->hasKey("/Type") &&
            (this->getKey("/Type").getName() == "/Pages"));
}