#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "ptrie.h"

// Structure to represent a node in the prefix trie
struct ptrie_node {
    struct ptrie_node *children[128]; // Array for child nodes (ASCII characters)
    int count; // Frequency count for string
    int isEndOfWord; // Flag to indicate end of a word
};

// Structure to represent the prefix trie
struct ptrie {
    struct ptrie_node *root; // Root node of the trie
};

// Helper function to create a new node
static struct ptrie_node *ptrie_createNode(void) {
    struct ptrie_node *node = malloc(sizeof(struct ptrie_node));
    if (node != NULL) {
        node->count = 0;
        node->isEndOfWord = 0;
        for (int i = 0; i < 128; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

// Helper function to convert character to array index
static int ptrie_char2off(char c) {
    return (int) c;
}

// Helper function to check if a character is valid
static int ptrie_isValidChar(char c) {
    if (c < 32) {
        return 0;
    }
    return 1;
}

struct ptrie *ptrie_allocate(void) {
    struct ptrie *pt = malloc(sizeof(struct ptrie));
    if (pt != NULL) {
        pt->root = ptrie_createNode();
        if (pt->root == NULL) {
            free(pt);
            return NULL;
        }
    }
    return pt;
}

void ptrie_freeNode(struct ptrie_node *node) {
    if (node == NULL) {
        return;
    }
    for (int i = 0; i < 128; i++) {
        ptrie_freeNode(node->children[i]);
    }
    free(node);
}

void ptrie_free(struct ptrie *pt) {
    if (pt == NULL) {
        return;
    }
    ptrie_freeNode(pt->root);
    free(pt);
}

int ptrie_add(struct ptrie *pt, const char *str) {
    if (pt == NULL || str == NULL) {
        return -1;
    }

    struct ptrie_node *current = pt->root;
    int len = strlen(str);

    for (int i = 0; i < len; i++) {
        char c = str[i];
        if (!ptrie_isValidChar(c)) {
            return -1;
        }

        int index = ptrie_char2off(c);
        if (current->children[index] == NULL) {
            current->children[index] = ptrie_createNode();
            if (current->children[index] == NULL) {
                return -1;
            }
        }
        current = current->children[index];
    }

    if (current->isEndOfWord == 0) {
        current->isEndOfWord = 1;
    }
    current->count++;

    return 0;
}

void ptrie_autocompleteHelper(struct ptrie_node *node, char *prefix, char **result, int *freq) {
    if (node == NULL) {
        return;
    }

    if (node->isEndOfWord && node->count > *freq) {
        free(*result); // If reassignment is needed, must free first; otherwise, memory leak occurs
        *result = strdup(prefix);
        *freq = node->count; 
    }

    int prefixLen = strlen(prefix); // Store the length of the prefix string
    char nextPrefix[128];
    for (int i = 0; i < 128; i++) {
        if (node->children[i] != NULL) {
            strcpy(nextPrefix, prefix);
            nextPrefix[prefixLen] = (char) i; // Append the character at the end of the prefix
            nextPrefix[prefixLen + 1] = '\0'; // Add the null-terminating character
            ptrie_autocompleteHelper(node->children[i], nextPrefix, result, freq);
        }
    }
}

char *ptrie_autocomplete(struct ptrie *pt, const char *str) {
    if (pt == NULL || str == NULL) {
        return NULL;
    }

    struct ptrie_node *current = pt->root;
    int len = strlen(str);

    for (int i = 0; i < len; i++) {
        char c = str[i];
        if (!ptrie_isValidChar(c)) {
            return NULL;
        }

        int index = ptrie_char2off(c);
        if (current->children[index] == NULL) {
            return strdup(str);
        }
        current = current->children[index];
    }

    char *result = NULL;
    int freq = -1; // Initialize freq as -1 to ensure a completion gets selected if available
    char prefix[128];
    memset(prefix, 0, sizeof(prefix));
    strcpy(prefix, str);

    ptrie_autocompleteHelper(current, prefix, &result, &freq);

    if(result == NULL) {
        return strdup(str);
    } else {
        return result;
    }
}

static void ptrie_printHelper(struct ptrie_node *node, char *buffer, int level) {
    if (node->isEndOfWord) {
        buffer[level] = '\0';
        printf("%s - %d\n", buffer, node->count);
    }

    for (int i = 0; i < 128; i++) {
        if (node->children[i] != NULL) {
            buffer[level] = (char) i;
            ptrie_printHelper(node->children[i], buffer, level + 1);
        }
    }
}

void ptrie_print(struct ptrie *pt) {
    if (pt == NULL) {
        return;
    }

    char *buffer = malloc(256 * sizeof(char));
    if (buffer == NULL) {
        return;
    }

    ptrie_printHelper(pt->root, buffer, 0);

    free(buffer);
}
