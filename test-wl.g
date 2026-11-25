LoadPackage("digraphs");

Read("gap/wl.gi");

# Define a simple undirected graph A-B-C ie define edges to be directed both directions
D := Digraph(["a", "b", "c"], 
["a", "b", "b", "c"], 
["b", "a", "c", "b"]);

# Giving every vertex the same initial colour (start from same level)
colours := ListWithIdenticalEntries(DigraphNrVertices(D), 1);

# Run 1-WL
result := OneDimensionalWL(D, colours);

# Display results
Print("Final colours: ", result.colours, "\n");
Print("Refinement history: ", result.history, "\n");
Print("Final colour multiset: ", result.multiset, "\n");
