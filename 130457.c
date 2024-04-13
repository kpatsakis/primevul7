QPDFObjectHandle::newReserved(QPDF* qpdf)
{
    // Reserve a spot for this object by assigning it an object
    // number, but then return an unresolved handle to the object.
    QPDFObjectHandle reserved = qpdf->makeIndirectObject(
	QPDFObjectHandle(new QPDF_Reserved()));
    QPDFObjectHandle result =
        newIndirect(qpdf, reserved.objid, reserved.generation);
    result.reserved = true;
    return result;
}