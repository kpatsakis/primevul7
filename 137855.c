exsltSaxonRegister (void) {
     xsltRegisterExtModule (SAXON_NAMESPACE,
			    (xsltExtInitFunction) exsltSaxonInit,
			    (xsltExtShutdownFunction) exsltSaxonShutdown);
     xsltRegisterExtModuleFunction((const xmlChar *) "expression",
				   SAXON_NAMESPACE,
				   exsltSaxonExpressionFunction);
     xsltRegisterExtModuleFunction((const xmlChar *) "eval",
				   SAXON_NAMESPACE,
				   exsltSaxonEvalFunction);
     xsltRegisterExtModuleFunction((const xmlChar *) "evaluate",
				   SAXON_NAMESPACE,
				   exsltSaxonEvaluateFunction);
    xsltRegisterExtModuleFunction ((const xmlChar *) "line-number",
				   SAXON_NAMESPACE,
				   exsltSaxonLineNumberFunction);
    xsltRegisterExtModuleFunction ((const xmlChar *) "systemId",
				   SAXON_NAMESPACE,
				   exsltSaxonSystemIdFunction);
}