MultiPartInputFile::~MultiPartInputFile()
{
    for (map<int, GenericInputFile*>::iterator it = _data->_inputFiles.begin();
         it != _data->_inputFiles.end(); it++)
    {
        delete it->second;
    }

    delete _data;
}