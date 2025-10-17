#include <tree_sitter/parser.h>
#include <algorithm>
#include <vector>
#include <string>
#include <cwctype>
#include <cstring>

namespace {

using std::vector;
using std::string;

enum TokenType {
  SCHEMA_START_TAG_NAME,
  DATA_START_TAG_NAME,
  TEMPLATE_START_TAG_NAME,
  LOGIC_START_TAG_NAME,
  END_TAG_NAME,
  RAW_TEXT,
  COMMENT
};

enum SectionType {
  SCHEMA,
  DATA,
  TEMPLATE,
  LOGIC
};

struct Scanner {
  Scanner() {}

  unsigned serialize(char *buffer) {
    uint16_t tag_count = tags.size() > UINT16_MAX ? UINT16_MAX : tags.size();
    uint16_t serialized_tag_count = 0;

    unsigned i = sizeof(tag_count);
    std::memcpy(&buffer[i], &tag_count, sizeof(tag_count));
    i += sizeof(tag_count);

    for (; serialized_tag_count < tag_count; serialized_tag_count++) {
      if (i + 1 >= TREE_SITTER_SERIALIZATION_BUFFER_SIZE) break;
      buffer[i++] = static_cast<char>(tags[serialized_tag_count]);
    }

    std::memcpy(&buffer[0], &serialized_tag_count, sizeof(serialized_tag_count));
    return i;
  }

  void deserialize(const char *buffer, unsigned length) {
    tags.clear();
    if (length > 0) {
      unsigned i = 0;
      uint16_t tag_count, serialized_tag_count;

      std::memcpy(&serialized_tag_count, &buffer[i], sizeof(serialized_tag_count));
      i += sizeof(serialized_tag_count);

      std::memcpy(&tag_count, &buffer[i], sizeof(tag_count));
      i += sizeof(tag_count);

      tags.resize(tag_count);
      for (unsigned j = 0; j < serialized_tag_count; j++) {
        tags[j] = static_cast<SectionType>(buffer[i++]);
      }
    }
  }

  string scan_tag_name(TSLexer *lexer) {
    string tag_name;
    while (iswalnum(lexer->lookahead) || lexer->lookahead == '-') {
      tag_name += towupper(lexer->lookahead);
      lexer->advance(lexer, false);
    }
    return tag_name;
  }

  bool scan_comment(TSLexer *lexer) {
    if (lexer->lookahead != '-') return false;
    lexer->advance(lexer, false);
    if (lexer->lookahead != '-') return false;
    lexer->advance(lexer, false);

    unsigned dashes = 0;
    while (lexer->lookahead) {
      switch (lexer->lookahead) {
        case '-':
          ++dashes;
          break;
        case '>':
          if (dashes >= 2) {
            lexer->result_symbol = COMMENT;
            lexer->advance(lexer, false);
            lexer->mark_end(lexer);
            return true;
          }
        default:
          dashes = 0;
      }
      lexer->advance(lexer, false);
    }
    return false;
  }

  bool scan_raw_text(TSLexer *lexer) {
    if (tags.empty()) return false;

    bool has_content = false;
    const string end_delimiter = get_end_delimiter(tags.back());

    unsigned delimiter_index = 0;
    while (lexer->lookahead) {
      if (towupper(lexer->lookahead) == end_delimiter[delimiter_index]) {
        delimiter_index++;
        if (delimiter_index == end_delimiter.size()) {
          // We found the end delimiter
          if (has_content) {
            lexer->result_symbol = RAW_TEXT;
            return true;
          }
          return false;
        }
        lexer->advance(lexer, false);
      } else {
        delimiter_index = 0;
        lexer->advance(lexer, false);
        lexer->mark_end(lexer);
        has_content = true;
      }
    }

    // Reached end of file
    if (has_content) {
      lexer->result_symbol = RAW_TEXT;
      return true;
    }
    return false;
  }

  string get_end_delimiter(SectionType type) {
    switch (type) {
      case SCHEMA: return "</SCHEMA";
      case DATA: return "</DATA";
      case TEMPLATE: return "</TEMPLATE";
      case LOGIC: return "</LOGIC";
      default: return "";
    }
  }

  bool scan_start_tag_name(TSLexer *lexer) {
    string tag_name = scan_tag_name(lexer);
    if (tag_name.empty()) return false;

    if (tag_name == "SCHEMA") {
      tags.push_back(SCHEMA);
      lexer->result_symbol = SCHEMA_START_TAG_NAME;
      return true;
    } else if (tag_name == "DATA") {
      tags.push_back(DATA);
      lexer->result_symbol = DATA_START_TAG_NAME;
      return true;
    } else if (tag_name == "TEMPLATE") {
      tags.push_back(TEMPLATE);
      lexer->result_symbol = TEMPLATE_START_TAG_NAME;
      return true;
    } else if (tag_name == "LOGIC") {
      tags.push_back(LOGIC);
      lexer->result_symbol = LOGIC_START_TAG_NAME;
      return true;
    }

    return false;
  }

  bool scan_end_tag_name(TSLexer *lexer) {
    string tag_name = scan_tag_name(lexer);
    if (tag_name.empty()) return false;

    SectionType expected_type;
    if (tag_name == "SCHEMA") {
      expected_type = SCHEMA;
    } else if (tag_name == "DATA") {
      expected_type = DATA;
    } else if (tag_name == "TEMPLATE") {
      expected_type = TEMPLATE;
    } else if (tag_name == "LOGIC") {
      expected_type = LOGIC;
    } else {
      return false;
    }

    if (!tags.empty() && tags.back() == expected_type) {
      tags.pop_back();
      lexer->result_symbol = END_TAG_NAME;
      return true;
    }

    return false;
  }

  bool scan(TSLexer *lexer, const bool *valid_symbols) {
    while (iswspace(lexer->lookahead)) {
      lexer->advance(lexer, true);
    }

    // RAW_TEXT scanning - only when we're inside a tag and not looking for start/end tags
    if (valid_symbols[RAW_TEXT] && !valid_symbols[SCHEMA_START_TAG_NAME] &&
        !valid_symbols[DATA_START_TAG_NAME] && !valid_symbols[TEMPLATE_START_TAG_NAME] &&
        !valid_symbols[LOGIC_START_TAG_NAME] && !valid_symbols[END_TAG_NAME]) {
      return scan_raw_text(lexer);
    }

    switch (lexer->lookahead) {
      case '<':
        lexer->mark_end(lexer);
        lexer->advance(lexer, false);

        if (lexer->lookahead == '!') {
          lexer->advance(lexer, false);
          return scan_comment(lexer);
        }
        break;

      default:
        if ((valid_symbols[SCHEMA_START_TAG_NAME] || valid_symbols[DATA_START_TAG_NAME] ||
             valid_symbols[TEMPLATE_START_TAG_NAME] || valid_symbols[LOGIC_START_TAG_NAME] ||
             valid_symbols[END_TAG_NAME]) && !valid_symbols[RAW_TEXT]) {
          return valid_symbols[END_TAG_NAME]
            ? scan_end_tag_name(lexer)
            : scan_start_tag_name(lexer);
        }
    }

    return false;
  }

  vector<SectionType> tags;
};

}

extern "C" {

void *tree_sitter_rue_external_scanner_create() {
  return new Scanner();
}

void tree_sitter_rue_external_scanner_destroy(void *payload) {
  Scanner *scanner = static_cast<Scanner *>(payload);
  delete scanner;
}

unsigned tree_sitter_rue_external_scanner_serialize(void *payload, char *buffer) {
  Scanner *scanner = static_cast<Scanner *>(payload);
  return scanner->serialize(buffer);
}

void tree_sitter_rue_external_scanner_deserialize(void *payload, const char *buffer, unsigned length) {
  Scanner *scanner = static_cast<Scanner *>(payload);
  scanner->deserialize(buffer, length);
}

bool tree_sitter_rue_external_scanner_scan(void *payload, TSLexer *lexer, const bool *valid_symbols) {
  Scanner *scanner = static_cast<Scanner *>(payload);
  return scanner->scan(lexer, valid_symbols);
}

}
