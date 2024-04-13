MultiPartInputFile::MultiPartInputFile (OPENEXR_IMF_INTERNAL_NAMESPACE::IStream& is,
                                        int numThreads,
                                        bool reconstructChunkOffsetTable):
    _data(new Data(false, numThreads, reconstructChunkOffsetTable))
{
    try
    {
        _data->is = &is;
        initialize();
    }
    catch (IEX_NAMESPACE::BaseExc &e)
    {
        delete _data;

        REPLACE_EXC (e, "Cannot read image file "
                     "\"" << is.fileName() << "\". " << e.what());
        throw;
    }
    catch (...)
    {
        delete _data;
        throw;
    }
}