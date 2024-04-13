QPDFObjectHandle::assertType(char const* type_name, bool istype) const
{
    if (! istype)
    {
	throw std::logic_error(std::string("operation for ") + type_name +
			       " object attempted on object of wrong type");
    }
}