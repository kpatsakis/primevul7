    uint32_t ArrayDef::size(uint16_t tag, IfdId group) const
    {
        TypeId typeId = toTypeId(tiffType_, tag, group);
        return count_ * TypeInfo::typeSize(typeId);
    }