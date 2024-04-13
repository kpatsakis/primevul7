Status ConstantFolding::MaterializeConstants(
    const GraphProperties& properties) {
  const int node_count = graph_->node_size();
  for (int i = 0; i < node_count; ++i) {
    NodeDef& node = *graph_->mutable_node(i);
    const string& op = node.op();
    if (op == "BroadcastGradientArgs") {
      TF_RETURN_IF_ERROR(MaterializeBroadcastGradientArgs(node, properties));
    } else if (IsReduction(node)) {
      TF_RETURN_IF_ERROR(MaterializeReductionIndices(&node, properties));
    } else if (IsFill(node) || IsZerosLike(node) || IsOnesLike(node)) {
      TF_RETURN_IF_ERROR(MaterializeConstantValuedNode(&node, properties));
    } else {
      TF_RETURN_IF_ERROR(MaterializeOutputValues(&node, properties));
    }
  }
  return Status::OK();
}