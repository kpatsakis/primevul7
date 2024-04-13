QPDFObjectHandle::isPageObject()
{
    return (this->isDictionary() && this->hasKey("/Type") &&
            (this->getKey("/Type").getName() == "/Page"));
}