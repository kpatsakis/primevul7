void ConstantFolding::ReplaceBinaryOperationWithBroadcastTo(
    int input_to_broadcast, const GraphProperties& properties, NodeDef* node,
    GraphDef* graph) {
  if (!ReplaceOperationWithBroadcastTo(input_to_broadcast, properties, node,
                                       graph)) {
    return;
  }
  graph_modified_ = true;
}