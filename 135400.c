MultiPartInputFile::MultiPartInputFile(const char fileName[],
                           int numThreads,
                           bool reconstructChunkOffsetTable):
    _data(new Data(true, numThreads, reconstructChunkOffsetTable))
{
    try
    {
        _data->is = new StdIFStream (fileName);
        initialize();
    }
    catch (IEX_NAMESPACE::BaseExc &e)
    {
        delete _data;

        REPLACE_EXC (e, "Cannot read image file "
                     "\"" << fileName << "\". " << e.what());
        throw;
    }
    catch (...)
    {
        delete _data;
        throw;
    }
}