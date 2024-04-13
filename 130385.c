QPDFObjectHandle::getPageContents()
{
    assertPageObject();

    std::vector<QPDFObjectHandle> result;
    QPDFObjectHandle contents = this->getKey("/Contents");
    if (contents.isArray())
    {
	int n_items = contents.getArrayNItems();
	for (int i = 0; i < n_items; ++i)
	{
	    QPDFObjectHandle item = contents.getArrayItem(i);
	    if (item.isStream())
	    {
		result.push_back(item);
	    }
	    else
	    {
		throw std::runtime_error(
		    "unknown item type while inspecting "
		    "element of /Contents array in page "
		    "dictionary");
	    }
	}
    }
    else if (contents.isStream())
    {
	result.push_back(contents);
    }
    else if (! contents.isNull())
    {
	throw std::runtime_error("unknown object type inspecting /Contents "
				 "key in page dictionary");
    }

    return result;
}