Status ConstantFolding::FoldGraph(
    const GraphProperties& properties, GraphDef* optimized_graph,
    absl::flat_hash_set<string>* nodes_to_not_simplify) {
  // We build a new optimized_graph by inserting the folded nodes into it, then
  // copy other nodes that might be needed at the end of this function.
  absl::flat_hash_set<string> processed_nodes;
  std::deque<NodeDef*> queue;
  for (int i = 0; i < graph_->node_size(); i++) {
    const NodeDef& node = graph_->node(i);
    if (IsFoldable(node, &properties) &&
        !nodes_to_not_simplify->count(node.name())) {
      queue.push_back(graph_->mutable_node(i));
    }
  }
  while (!queue.empty()) {
    NodeDef* node = queue.front();
    queue.pop_front();
    if (processed_nodes.count(node->name())) {
      continue;
    }
    // We need to record a copy of output nodes before FoldNode() modifies it.
    // We also need to ensure that the fanout is sorted deterministically.
    std::vector<NodeDef*> fanout =
        node_map_->GetOutputsOrderedByNodeName(node->name());
    bool result_too_large = false;
    Status s = FoldNode(node, optimized_graph, &result_too_large);
    processed_nodes.insert(node->name());
    if (!s.ok()) {
      VLOG(1) << "Failed to fold node " << node->DebugString()
              << "\nError message: " << s;
      if (result_too_large) {
        nodes_to_not_simplify->emplace(node->name());
      }
    } else {
      for (auto& fanout_node : fanout) {
        if (IsFoldable(*fanout_node, &properties) &&
            !nodes_to_not_simplify->count(fanout_node->name())) {
          queue.push_back(fanout_node);
        }
      }
    }
  }

  // Delete the newly created nodes that don't feed anything.
  std::vector<int> nodes_to_delete;
  for (int i = 0; i < optimized_graph->node_size(); i++) {
    const auto& fanout = node_map_->GetOutputs(optimized_graph->node(i).name());
    if (fanout.empty()) nodes_to_delete.push_back(i);
  }
  EraseNodesFromGraph(std::move(nodes_to_delete), optimized_graph);

  for (int i = 0; i < graph_->node_size(); ++i) {
    NodeDef* node = graph_->mutable_node(i);
    // If no fetch nodes is provided, we conservatively
    // move all nodes in the original graph to the output, in case users need
    // to fetch their values.
    const auto& fanout = node_map_->GetOutputs(node->name());
    if (!fanout.empty() || !has_fetch_ ||
        nodes_to_preserve_.find(node->name()) != nodes_to_preserve_.end()) {
      *(optimized_graph->add_node()) = std::move(*node);
    }
  }
  return Status::OK();
}