#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"

#define MAX_WORD_LEN 101
#define HASH_SIZE 100

enum boolean
{
    FALSE,
    TRUE
};

enum directive
{
    NONE = 0,
    DEFINE = 1,
    UNDEF = 2
};

typedef struct list_node
{
    char *name;
    char *definition;
    struct list_node *next;
} list_node;

size_t get_word(char *str, int len);

list_node *lookup(char *name);
void consume_between(char begin, char end);
void consume_comments();
void consume_char_literal();
void consume_string_literal();
void consume_preproc();
void consume_word(char *word, char *Error_str);
void consume_blanks();
int hash(char *str);
list_node *install(char *name, char *definition);
enum boolean undef(char *name);
size_t get_alnum_str(char *str, size_t max_str_len);

static list_node *hash_table[HASH_SIZE];

int hash(char *str)
{
    size_t hash_value = 0;
    while (*str != '\0')
    {
        hash_value = *str + 31 * hash_value;
        ++str;
    }
    return hash_value % HASH_SIZE;
}

int main()
{
    int c;
    char word[MAX_WORD_LEN];
    while ((c = get_word(word, MAX_WORD_LEN)) != EOF)
    {
        if (isalpha(c))
        {
            list_node *node_p = lookup(word);
            if (node_p != NULL)
            {
                printf("%s", node_p->definition);
            }
            else
            {
                printf("%s", word);
            }
        }
        else
        {
            if (c == '/')
            {
                ungetc(c, stdin);
                consume_comments();
            }
            else if (c == '\'')
            {
                ungetc(c, stdin);
                consume_char_literal();
            }
            else if (c == '\"')
            {
                ungetc(c, stdin);
                consume_string_literal();
            }
            else if (c == '#')
            {
                ungetc(c, stdin);
                consume_preproc();
            }
            else
            {
                putc(c, stdout);
            }
        }
    }
    return EXIT_SUCCESS;
}

void consume_between(char begin, char end)
{
    int c = getc(stdin);
    if (c == begin)
    {
        putc(c, stdout);
        while ((c = getc(stdin)) != EOF)
        {
            putc(c, stdout);
            if (c == '\\')
            {
                putc(c, stdout);
                c = getc(stdin);
                if (c == EOF)
                {
                    break;
                }
                putc(c, stdout);
            }
            else if (c == end)
            {
                return;
            }
        }
    }
    ungetc(c, stdin);
}

void consume_comments()
{
    int c = getc(stdin);
    if (c == '/')
    {
        putc(c, stdout);
        c = getc(stdin);
        if (c == '/')
        {
            putc(c, stdout);
            while ((c = getc(stdin)) != '\n' && c != EOF)
                putc(c, stdout);
        }
        else if (c == '*')
        {
            putc(c, stdout);
            while ((c = getc(stdin)) != EOF)
            {
                putc(c, stdout);
                if (c == '*')
                {
                    c = getc(stdin);
                    putc(c, stdout);
                    if (c == '/')
                    {
                        break;
                    }
                }
            }

            c = getc(stdin);
            if (c == '/')
            {
                putc(c, stdout);
                return;
            }
        }
    }
    ungetc(c, stdin);
}


size_t get_word(char *str, int len)
{
    size_t i = 0;
    int c = getc(stdin);
    if (c != EOF)
    {
        str[i++] = c;
    }
    if (!isalnum(c) && c != '_')
    {
        str[i] = '\0';
        return str[0];
    }

    while ((isalnum(c = getc(stdin)) || c == '_') && i < len)
    {
        str[i++] = c;
    }
    ungetc(c, stdin);
    str[i] = '\0';
    return str[0];
}

list_node *lookup(char *name)
{
    int hash_val = hash(name);
    for (list_node *pnode = hash_table[hash_val]; pnode != NULL; pnode = pnode->next)
    {
        if (strcmp(name, pnode->name) == 0)
        {
            return pnode;
        }
    }
    return NULL;
}

void consume_char_literal()
{
    consume_between('\'', '\'');
}

void consume_string_literal()
{
    consume_between('\"', '\"');
}

void consume_word(char *word, char *Error_str)
{
    int c;
    if ((c = get_word(word, MAX_WORD_LEN)) == EOF)
    {
        ungetc(c, stdin);
        return;
    }
    else if (!isalpha(c))
    {
        puts(Error_str);
    }
    printf("%s", word);
}

void consume_blanks()
{
    int c;
    while (isblank(c = getc(stdin)))
    {
        putc(c, stdout);
    }
    ungetc(c, stdin);
}

list_node *install(char *name, char *definition)
{
    list_node *pnode;
    if ((pnode = lookup(name)) == NULL)
    {
        pnode = (list_node *)malloc(sizeof(*pnode));
        if (pnode == NULL || (pnode->name = strdup(name)) == NULL)
        {
            return NULL;
        }
        // pnode->definition = strdup(definition);
        int hash_val = hash(name);
        pnode->next = hash_table[hash_val];
        hash_table[hash_val] = pnode;
    }
    else
    {
        free(pnode->definition);
    }
    if ((pnode->definition = strdup(definition)) == NULL)
    {
        return NULL;
    }
    return pnode;
}

enum boolean undef(char *name)
{
    int hash_val = hash(name);
    list_node *pnode, *prev_node;
    for (pnode = hash_table[hash_val], prev_node = NULL; pnode != NULL; prev_node = pnode, pnode = pnode->next)
    {
        if ((strcmp(name, pnode->name)) == 0)
        {
            free(pnode->name);
            free(pnode->definition);
            if (prev_node == NULL)
            {
                hash_table[hash_val] = pnode->next;
            }
            else
            {
                prev_node->next = pnode->next;
            }
            free(pnode);
            return TRUE;
        }
    }
    return FALSE;
}

size_t get_alnum_str(char *str, size_t max_str_len)
{
    size_t i = 0;
    int c;
    while (isalnum(c = getc(stdin)) && i < max_str_len)
    {
        str[i++] = c;
    }
    str[i] = '\0';
    ungetc(c, stdin);
    return i;
}

void consume_preproc()
{
    int c;
    c = getc(stdin);
    if (c == '#')
    {
        putc(c, stdout);
        enum directive directive_type = NONE;
        char directive_name[MAX_WORD_LEN];
        consume_word(directive_name, "Error: expected preprocessor directive");
        if (strcmp(directive_name, "define") == 0)
        {
            directive_type = DEFINE;
        }
        else if (strcmp(directive_name, "undef") == 0)
        {
            directive_type = UNDEF;
        }

        char name[MAX_WORD_LEN];
        char definition[MAX_WORD_LEN];
        if (directive_type)
        {
            consume_blanks();
            consume_word(name, "Error, invalid name.");
        }
        if (directive_type == DEFINE)
        {

            consume_blanks();
            get_word(definition, MAX_WORD_LEN);
            printf("%s", definition);
            install(name, definition);
        }
        else if (directive_type == UNDEF)
        {
            undef(name);
        }
    }
    else
    {
        ungetc(c, stdin);
    }
}
