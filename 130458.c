QPDFObjectHandle::makeDirect()
{
    std::set<int> visited;
    makeDirectInternal(visited);
}