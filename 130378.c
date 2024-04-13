QPDFObjectHandle::QPDFObjectHandle() :
    initialized(false),
    qpdf(0),
    objid(0),
    generation(0),
    reserved(false)
{
}