QPDFObjectHandle::getNumericValue()
{
    double result = 0.0;
    if (isInteger())
    {
	result = static_cast<double>(getIntValue());
    }
    else if (isReal())
    {
	result = atof(getRealValue().c_str());
    }
    else
    {
	throw std::logic_error("getNumericValue called for non-numeric object");
    }
    return result;
}