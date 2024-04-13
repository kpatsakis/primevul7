exsltSaxonEvaluateFunction (xmlXPathParserContextPtr ctxt, int nargs) {
     if (nargs != 1) {
	  xmlXPathSetArityError(ctxt);
	  return;
     }

     exsltSaxonExpressionFunction(ctxt, 1);
     exsltSaxonEvalFunction(ctxt, 1);
}