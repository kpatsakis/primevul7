void PacketReader::xfrBlob(string& blob)
try
{
  if(d_recordlen && !(d_pos == (d_startrecordpos + d_recordlen)))
    blob.assign(&d_content.at(d_pos), &d_content.at(d_startrecordpos + d_recordlen - 1 ) + 1);
  else
    blob.clear();

  d_pos = d_startrecordpos + d_recordlen;
}