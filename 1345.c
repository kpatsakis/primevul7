void ConstantFolding::RemoveSplitOrSplitV(const GraphProperties& properties,
                                          GraphDef* optimized_graph,
                                          NodeDef* node) {
  if (node->attr().count("num_split") == 0) return;
  if (IsSplit(*node) && node->attr().at("num_split").i() == 1) {
    ReplaceOperationWithIdentity(1, properties, node, optimized_graph);
  }
  if (IsSplitV(*node) && node->attr().at("num_split").i() == 1) {
    ReplaceOperationWithIdentity(0, properties, node, optimized_graph);
  }
}