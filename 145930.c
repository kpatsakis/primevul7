    TiffComponent::UniquePtr TiffParserWorker::parse(
        const byte*              pData,
              uint32_t           size,
              uint32_t           root,
              TiffHeaderBase*    pHeader
    )
    {
        if (pData == 0 || size == 0)
            return nullptr;
        if (!pHeader->read(pData, size) || pHeader->offset() >= size) {
            throw Error(kerNotAnImage, "TIFF");
        }
        TiffComponent::UniquePtr rootDir = TiffCreator::create(root, ifdIdNotSet);
        if (0 != rootDir.get()) {
            rootDir->setStart(pData + pHeader->offset());
            TiffRwState state(pHeader->byteOrder(), 0);
            TiffReader reader(pData, size, rootDir.get(), state);
            rootDir->accept(reader);
            reader.postProcess();
        }
        return rootDir;

    } // TiffParserWorker::parse