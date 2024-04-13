MultiPartInputFile::partComplete(int part) const
{
  return _data->parts[part]->completed;
}