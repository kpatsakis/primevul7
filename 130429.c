QPDFObjectHandle::assertInitialized() const
{
    if (! this->initialized)
    {
	throw std::logic_error("operation attempted on uninitialized "
			       "QPDFObjectHandle");
    }
}