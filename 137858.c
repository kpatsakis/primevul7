exsltSaxonInit (xsltTransformContextPtr ctxt ATTRIBUTE_UNUSED,
		const xmlChar *URI ATTRIBUTE_UNUSED) {
    return xmlHashCreate(1);
}