MultiPartInputFile::getInputPart(int partNumber)
{
    Lock lock(*_data);
            if (_data->_inputFiles.find(partNumber) == _data->_inputFiles.end())
        {
            T* file = new T(_data->getPart(partNumber));
            _data->_inputFiles.insert(std::make_pair(partNumber, (GenericInputFile*) file));
            return file;
        }
        else return (T*) _data->_inputFiles[partNumber];
}