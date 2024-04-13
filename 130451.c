QPDFObjectHandle::addPageContents(QPDFObjectHandle new_contents, bool first)
{
    assertPageObject();
    new_contents.assertStream();

    std::vector<QPDFObjectHandle> orig_contents = getPageContents();

    std::vector<QPDFObjectHandle> content_streams;
    if (first)
    {
	QTC::TC("qpdf", "QPDFObjectHandle prepend page contents");
	content_streams.push_back(new_contents);
    }
    for (std::vector<QPDFObjectHandle>::iterator iter = orig_contents.begin();
	 iter != orig_contents.end(); ++iter)
    {
	QTC::TC("qpdf", "QPDFObjectHandle append page contents");
	content_streams.push_back(*iter);
    }
    if (! first)
    {
	content_streams.push_back(new_contents);
    }

    QPDFObjectHandle contents = QPDFObjectHandle::newArray(content_streams);
    this->replaceKey("/Contents", contents);
}