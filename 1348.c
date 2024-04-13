Status ConstantFolding::RunOptimizationPass(Cluster* cluster,
                                            GrapplerItem* item,
                                            GraphProperties* properties,
                                            GraphDef* optimized_graph) {
  optimized_graph->Clear();
  graph_ = &item->graph;
  node_map_.reset(new NodeMap(graph_));
  nodes_allowlist_.clear();
  // Fold fetch nodes iff it has a single fanout. Note that if a fetch node
  // has a single fanout, it would be rewritten as a constant with the same
  // node name, and therefore users are still able to fetch it. This is not
  // the case if the node has multiple fanouts, and constant folding would
  // replace the node with multiple constants (each for one fanout) with
  // new names, and as a result users would not be able to fetch the node any
  // more with the original node name.
  for (const auto& fetch : item->fetch) {
    const NodeDef* fetch_node = node_map_->GetNode(fetch);
    if (fetch_node && NumOutputs(*fetch_node, graph_) == 1) {
      nodes_allowlist_.insert(fetch_node->name());
    }
  }

  absl::flat_hash_set<string> nodes_to_not_simplify;
  if (properties->has_properties()) {
    TF_RETURN_IF_ERROR(MaterializeShapes(*properties));
    TF_RETURN_IF_ERROR(MaterializeConstants(*properties));
    TF_RETURN_IF_ERROR(
        FoldGraph(*properties, optimized_graph, &nodes_to_not_simplify));
  } else {
    *optimized_graph = *graph_;
  }
  node_map_.reset(new NodeMap(optimized_graph));

  TF_RETURN_IF_ERROR(
      SimplifyGraph(optimized_graph, properties, &nodes_to_not_simplify));

  return Status::OK();
}