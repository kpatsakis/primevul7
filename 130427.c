QPDFObjectHandle::getObjGen() const
{
    return QPDFObjGen(this->objid, this->generation);
}