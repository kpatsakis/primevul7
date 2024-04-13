QPDFObjectHandle::releaseResolved()
{
    // Recursively break any resolved references to indirect objects.
    // Do not cross over indirect object boundaries to avoid an
    // infinite loop.  This method may only be called during final
    // destruction.  See comments in QPDF::~QPDF().
    if (isIndirect())
    {
	if (this->obj.getPointer())
	{
	    this->obj = 0;
	}
    }
    else
    {
	QPDFObject::ObjAccessor::releaseResolved(this->obj.getPointer());
    }
}