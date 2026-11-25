# still testing
# Limitations: graphs need to be symmetric/ undirected

WLRefineColours := function(D, colours)
    local n, neighbours, signatures, uniqueSignatures,
          newColours, i, signature;

    n := DigraphNrVertices(D);
    neighbours := OutNeighbours(D);
    signatures := [];

    for i in [1..n] do

        signature := [
            colours[i],
            SortedList(List(neighbours[i], v -> colours[v]))
        ];

        Add(signatures, signature);
    od;

    uniqueSignatures := Set(signatures);

    newColours := List(signatures,
        sig -> Position(uniqueSignatures, sig)
    );

    return newColours;
end;


OneDimensionalWL := function(D, colours)
    local oldColours, newColours, history, n, stable;

    n := DigraphNrVertices(D);

    if Length(colours) <> n then
        Error("The number of colours must equal the number of vertices");
    fi;

    if not IsSymmetricDigraph(D) then
        Error("The graph must be undirected (symmetric)");
    fi;

    if IsMultiDigraph(D) or DigraphHasLoops(D) then
        Error("The graph must be simple and have no loops");
    fi;

    oldColours := List(Set(colours), 
        c -> Position(Set(colours), c)
    );

    # initialise colours according to their original values
    oldColours := List(colours,
        c -> Position(Set(colours), c)
    );

    history := [ShallowCopy(oldColours)];

    while true do

        newColours := WLRefineColours(D, oldColours);

        stable := ForAll([1..n], i ->
            ForAll([1..n], j ->
                (oldColours[i] = oldColours[j]) =
                (newColours[i] = newColours[j])
            )
        );

        Add(history, ShallowCopy(newColours));

        if stable then
            break;
        fi;

        oldColours := newColours;

    od;

    return rec(
        colours := newColours,
        history := history,
        multiset := Collected(newColours)
    );
end;