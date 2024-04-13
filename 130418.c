QPDFObjectHandle::shallowCopy()
{
    assertInitialized();

    if (isStream())
    {
	QTC::TC("qpdf", "QPDFObjectHandle ERR shallow copy stream");
	throw std::runtime_error(
	    "attempt to make a shallow copy of a stream");
    }

    QPDFObjectHandle new_obj;
    if (isArray())
    {
	QTC::TC("qpdf", "QPDFObjectHandle shallow copy array");
	new_obj = newArray(getArrayAsVector());
    }
    else if (isDictionary())
    {
	QTC::TC("qpdf", "QPDFObjectHandle shallow copy dictionary");
        new_obj = newDictionary(getDictAsMap());
    }
    else
    {
	QTC::TC("qpdf", "QPDFObjectHandle shallow copy scalar");
        new_obj = *this;
    }

    return new_obj;
}