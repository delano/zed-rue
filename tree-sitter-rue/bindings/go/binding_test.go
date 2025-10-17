package tree_sitter_rue_test

import (
	"testing"

	tree_sitter "github.com/smacker/go-tree-sitter"
	"github.com/tree-sitter/tree-sitter-rue"
)

func TestCanLoadGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_rue.Language())
	if language == nil {
		t.Errorf("Error loading Rue grammar")
	}
}
