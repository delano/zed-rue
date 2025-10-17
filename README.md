# Zed Rue Extension

A Zed extension for Ruby Single File Component (.rue) files used by the Rhales framework.

## Features

- Syntax highlighting for .rue files
- Language injection for multi-section components:
  - `<schema>` - Zod schema definitions (JavaScript/TypeScript)
  - `<data>` - JSON data (deprecated)
  - `<template>` - HTML with Handlebars syntax
  - `<logic>` - Ruby documentation

## Installation

1. Clone this repository
2. Build the tree-sitter grammar (see tree-sitter-rue repository)
3. Install in Zed via Extensions panel

## .rue File Format

See the [Rue Format Quick Reference](./RUE_FORMAT.md) for complete documentation.

## Development

### Prerequisites

- Rust toolchain
- tree-sitter-cli
- Zed editor

### Building

```bash
cargo build --release
```

### Testing

Open a .rue file in Zed to test syntax highlighting and language injection.

## Related Projects

- [tree-sitter-rue](https://github.com/yourusername/tree-sitter-rue) - Tree-sitter grammar for .rue files
- [Rhales](https://github.com/yourusername/rhales) - Ruby Single File Component framework

## License

MIT
