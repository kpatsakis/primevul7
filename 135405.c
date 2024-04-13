MultiPartInputFile::Data::readChunkOffsetTables(bool reconstructChunkOffsetTable)
{
    bool brokenPartsExist = false;

    for (size_t i = 0; i < parts.size(); i++)
    {
        int chunkOffsetTableSize = getChunkOffsetTableSize(parts[i]->header,false);
        parts[i]->chunkOffsets.resize(chunkOffsetTableSize);

        for (int j = 0; j < chunkOffsetTableSize; j++)
            OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*is, parts[i]->chunkOffsets[j]);

        //
        // Check chunk offsets, reconstruct if broken.
        // At first we assume the table is complete.
        //
        parts[i]->completed = true;
        for (int j = 0; j < chunkOffsetTableSize; j++)
        {
            if (parts[i]->chunkOffsets[j] <= 0)
            {
                brokenPartsExist = true;
                parts[i]->completed = false;
                break;
            }
        }
    }

    if (brokenPartsExist && reconstructChunkOffsetTable)
        chunkOffsetReconstruction(*is, parts);
}