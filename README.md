
# Fractal Tree/Bε-tree (B-epsilon Tree) Implementation

## Overview

This project is an **in-memory implementation of the Bε-tree (B-epsilon tree)** data structure, a write-optimized variant of B-trees. The Bε-tree improves insert and update performance by buffering updates in internal nodes and flushing them in batches down the tree, while maintaining efficient query performance.

The implementation follows the design described by Bender et al. in their seminal paper ["An Introduction to Bε-trees and Write-Optimization"](https://www.usenix.org/publications/login/oct15/bender), and supports the core operations:

- Insert
- Update (only if key exists)
- Delete
- Point Query

It is designed to be modular, configurable, and extensible, with clear separation of tree and node logic.

---

## Features

- **Write-optimized:** Buffers updates in internal nodes to batch writes and reduce overhead.
- **Balanced tree:** Splits leaves and internal nodes to maintain logarithmic height.
- **Upsert support:** Handles insert, update, and delete messages as buffered operations.
- **Configurable parameters:** Node size, buffer size, and epsilon parameter can be tuned.
- **Query correctness:** Queries apply buffered updates along the root-to-leaf path.
- **Real-time interaction:** Includes an interactive CLI mode for live operations.
- **Graphical visualization:** Supports exporting tree structure to Graphviz DOT format for visualization.

---

## Requirements

- C++ compiler with C++11 support or higher (tested with `g++`)
- [Graphviz](https://graphviz.org/) (optional, for graphical visualization)
- Standard C++ libraries

---

## Installation and Compilation

1. Clone or download the project source code.
2. Navigate to the project directory.
3. Compile all source files together:
```bash
g++ *.cpp -o betree
```


---

## Usage

Run the compiled executable:

```bash
./betree
```

You will enter an interactive prompt supporting the following commands:

- `insert` — Insert a key-value pair.
- `update` — Update the value of an existing key.
- `delete` — Delete a key.
- `query` — Query the value of a key.
- `exit` — Exit the program.

Example:

```
> insert
key: 10
value: 100
Inserted (10, 100)
> query
key: 10
Key 10 has value 100
> update
key: 10
value: 200
Updated (10, 200)
> query
key: 10
Key 10 has value 200
> delete
key: 10
Deleted key 10
> query
key: 10
Key 10 not found
> exit
```


---

## Configuration

You can tune the tree’s behavior by modifying constants in `be_tree_config.hpp`:

- `NUM_DATA_PAIRS` — Maximum key-value pairs in a leaf node.
- `NUM_PIVOTS` — Number of pivots in internal nodes (controls fanout).
- `NUM_UPSERTS` — Size of the upsert buffer in internal nodes.
- `FLUSH_THRESHOLD` and `LEAF_FLUSH_THRESHOLD` — Flush batch sizes.

These parameters correspond to the **node size (B)** and **epsilon (ε)** in Bε-tree theory:

- `NUM_PIVOTS` ≈ \$ B^\epsilon \$
- `NUM_UPSERTS` ≈ \$ B - B^\epsilon \$

Adjust these to balance query and insert performance.

---

## Graphical Visualization

The tree can export its structure in [Graphviz DOT format](https://graphviz.org/doc/info/lang.html) for graphical visualization:

- After each operation, the tree writes `betree.dot`.
- Use Graphviz tools to render:

```bash
dot -Tpng betree.dot -o betree.png
xdot betree.dot  # for interactive viewing
```


---

## Code Structure

- `be_tree_config.hpp` — Configuration constants and types.
- `be_tree_node.hpp/cpp` — Node class and internal node/leaf logic.
- `be_tree.hpp/cpp` — Tree class and high-level operations.
- `be_tree_insert.cpp` — Insert operation.
- `be_tree_update.cpp` — Update operation with presence check.
- `be_tree_delete.cpp` — Delete operation.
- `be_tree_query.cpp` — Query operation.
- `main.cpp` — Interactive CLI program.

---

## Background and References

- Michael A. Bender et al., *An Introduction to Bε-trees and Write-Optimization*, USENIX Login, Oct 2015.
[https://www.usenix.org/publications/login/oct15/bender](https://www.usenix.org/publications/login/oct15/bender)
- Bε-tree theory balances write-optimization and query efficiency by tuning ε between 0 and 1.
- Compared to B-trees and LSM-trees, Bε-trees offer superior insert throughput and comparable query performance.

---

## Contributing

Contributions, bug reports, and feature requests are welcome. Please open issues or pull requests on the project repository.

---

## License

Specify your license here (e.g., MIT, Apache 2.0) or remove this section if proprietary.

---

## Contact

For questions or support, contact the maintainer or open an issue on the repository.

---

This README provides users and developers a concise but comprehensive overview to understand, build, use, and extend your Bε-tree implementation.

<div style="text-align: center">⁂</div>

[^1]: paste.txt

[^2]: login_oct15_05_bender.pdf

[^3]: https://github.com/KaminariOS/b-epsilon-tree

[^4]: https://dev.to/erikaheidi/documentation-101-creating-a-good-readme-for-your-software-project-cf8

[^5]: https://github.com/rahulyesantharao/b-epsilon-tree

[^6]: https://news.ycombinator.com/item?id=29403320

[^7]: https://www.cs.williams.edu/~jannen/teaching/s19/cs333/meetings/betrees-slides.pdf

[^8]: https://web2.qatar.cmu.edu/~mhhammou/15415-f16/projects/project3/P3_Handout.pdf

[^9]: https://www3.cs.stonybrook.edu/~bender/newpub/2015-BenderFaJa-login-wods.pdf

[^10]: https://www.scribd.com/document/834378353/B-Epsilor-tree

[^11]: https://ro.scribd.com/document/834378353/B-Epsilor-tree

[^12]: https://en.wikipedia.org/wiki/B-tree

