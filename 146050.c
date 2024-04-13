    void TiffReader::readDataEntryBase(TiffDataEntryBase* object)
    {
        assert(object != 0);

        readTiffEntry(object);
        TiffFinder finder(object->szTag(), object->szGroup());
        pRoot_->accept(finder);
        TiffEntryBase* te = dynamic_cast<TiffEntryBase*>(finder.result());
        if (te && te->pValue()) {
            object->setStrips(te->pValue(), pData_, size_, baseOffset());
        }
    }