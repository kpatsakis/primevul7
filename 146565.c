    void TiffReader::visitSizeEntry(TiffSizeEntry* object)
    {
        assert(object != 0);

        readTiffEntry(object);
        TiffFinder finder(object->dtTag(), object->dtGroup());
        pRoot_->accept(finder);
        TiffDataEntryBase* te = dynamic_cast<TiffDataEntryBase*>(finder.result());
        if (te && te->pValue()) {
            te->setStrips(object->pValue(), pData_, size_, baseOffset());
        }
    }