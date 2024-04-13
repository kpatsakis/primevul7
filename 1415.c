Status ConstantFolding::SimplifyGraph(
    GraphDef* optimized_graph, GraphProperties* properties,
    absl::flat_hash_set<string>* nodes_to_not_simplify) {
  for (int i = 0; i < optimized_graph->node_size(); ++i) {
    NodeDef* node = optimized_graph->mutable_node(i);
    // TODO(lyandy): Move nodes to not simplify check into SimplifyNode and
    // generalize to only restrict certain simplifications.
    if (nodes_to_not_simplify->find(node->name()) ==
        nodes_to_not_simplify->end()) {
      if (HasTPUAttributes(*node)) {
        nodes_to_not_simplify->insert(node->name());
        continue;
      }

      TF_RETURN_IF_ERROR(SimplifyNode(node, optimized_graph, properties));
    }
  }
  return Status::OK();
}