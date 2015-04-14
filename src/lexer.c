#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>

#include "lexer.h"

int lexer_init(struct lexer* self, const char* input)
{
    memset(self, 0, sizeof(*self));

    self->input = input;
    self->pos = input;
    self->accepted = 1;
    self->current_line = 1;
    self->line_start = input;

    /* TODO: Make buffer dynamic? */
    self->buffer_size = strlen(input) + 1;
    self->buffer = malloc(self->buffer_size);
    if(!self->buffer)
        return -1;

    return 0;
}

void lexer_cleanup(struct lexer* self)
{
    free(self->buffer);
}

void skip_whitespace(struct lexer* self)
{
    int i;
    const char* pos = self->pos;

    for(i = 0; pos[i]; ++i)
    {
        if(!isspace(pos[i]))
            break;

        if(pos[i] == '\n')
        {
            self->line_start = &pos[i+1];
            self->current_line++;
        }
    }

    self->pos += i;
}

size_t get_literal_length(const char* literal)
{
    if (!isalpha(*literal))
        return 0;

    size_t len;
    for(len = 1; literal[len] && isalnum(literal[len]); ++len);

    return len;
}

void copy_literal(struct lexer* self)
{
    size_t len = get_literal_length(self->pos);
    memcpy(self->buffer, self->pos, len);
    self->buffer[len] = 0;
    self->current_token.value.str = self->buffer;
}

int classify_number(struct lexer* self)
{
    double real;
    long long integer;
    char* endptr;

    size_t real_len, integer_len;

    errno = 0;
    endptr = 0;
    real = strtod(self->pos, &endptr);
    real_len = errno ? 0 : endptr - self->pos;
    if(errno)
        self->errno_ = errno;

    errno = 0;
    endptr = 0;
    integer = strtoll(self->pos, &endptr, 0);
    integer_len = errno ? 0 : endptr - self->pos;
    if(errno)
        self->errno_ = errno;

    if(real_len == 0 && integer_len == 0)
        return -1;

    if(real_len > integer_len)
    {
        self->current_token.type = TOK_REAL;
        self->current_token.value.real = real;
        self->next_pos = self->pos + real_len;
    }
    else
    {
        self->current_token.type = TOK_INTEGER;
        self->current_token.value.integer = integer;
        self->next_pos = self->pos + integer_len;
    }

    return 0;
}

int classify_string(struct lexer* self)
{
    assert(*self->pos == '"');

    char* dst = self->buffer;
    const char* src = self->pos;

    int is_escaped = 0;

    int i;
    for(i = 1; src[i]; ++i)
    {
        if(is_escaped)
        {
            switch(src[i])
            {
            case '"':
                *dst++ = '"';
                break;
            case '\\':
                *dst++ = '\\';
                break;
            case 'n':
                *dst++ = '\n';
                break;
            case 't':
                *dst++ = '\t';
                break;
            case 'b':
                *dst++ = '\b';
                break;
            case 'f':
                *dst++ = '\f';
                break;
            case 'r':
                *dst++ = '\r';
                break;
            default:
                goto error;
            }

            is_escaped = 0;
        }
        else
        {
            switch(src[i])
            {
            case '"':
                goto done;
            case '\\':
                is_escaped = 1;
                break;
            case '\n':
                self->line_start = &src[i+1];
                self->current_line++;
            default:
                *dst++ = src[i];
                break;
            }
        }
    }

error:
    self->pos += i;
    return -1;

done:
    *dst = 0;
    self->current_token.type = TOK_STRING;
    self->current_token.value.str = self->buffer;
    self->next_pos = self->pos + i + 1;

    return 0;
}

int classify_regex(struct lexer* self)
{
    assert(*self->pos == '/');

    char* dst = self->buffer;
    const char* src = self->pos;

    int is_escaped = 0;

    int i;
    for(i = 1; src[i]; ++i)
    {
        if(is_escaped)
        {
            switch(src[i])
            {
            case '/':
                *dst++ = '/';
                break;
            default:
                *dst++ = '\\';
                *dst++ = src[i];
                break;
            }

            is_escaped = 0;
        }
        else
        {
            switch(src[i])
            {
            case '/':
                goto done;
            case '\\':
                is_escaped = 1;
                break;
            default:
                *dst++ = src[i];
                break;
            }
        }
    }

    self->pos += i;
    return -1;

done:
    *dst = 0;
    self->current_token.type = TOK_REGEX;
    self->current_token.value.str = self->buffer;
    self->next_pos = self->pos + i + 1;

    return 0;
}

int classify_token(struct lexer* self)
{
    switch(*self->pos)
    {
    case '=':
        self->current_token.type = TOK_EQ;
        self->next_pos = self->pos + 1;
        return 0;
    case '|':
        self->current_token.type = TOK_PIPE;
        self->next_pos = self->pos + 1;
        return 0;
    case ',':
        self->current_token.type = TOK_COMMA;
        self->next_pos = self->pos + 1;
        return 0;
    case ':':
        self->current_token.type = TOK_COLON;
        self->next_pos = self->pos + 1;
        return 0;
    case ';':
        self->current_token.type = TOK_SEMICOMMA;
        self->next_pos = self->pos + 1;
        return 0;
    case '(':
        self->current_token.type = TOK_LPAREN;
        self->next_pos = self->pos + 1;
        return 0;
    case ')':
        self->current_token.type = TOK_RPAREN;
        self->next_pos = self->pos + 1;
        return 0;
    case '[':
        self->current_token.type = TOK_LBRACKET;
        self->next_pos = self->pos + 1;
        return 0;
    case ']':
        self->current_token.type = TOK_RBRACKET;
        self->next_pos = self->pos + 1;
        return 0;
    case '{':
        self->current_token.type = TOK_LBRACE;
        self->next_pos = self->pos + 1;
        return 0;
    case '}':
        self->current_token.type = TOK_RBRACE;
        self->next_pos = self->pos + 1;
        return 0;
    case '"':
        return classify_string(self);
    case '/':
        return classify_regex(self);
    case '\0':
        self->current_token.type = TOK_EOF;
        self->next_pos = self->pos + 1;
        return 0;
    default:
        break;
    }

    if(isalpha(*self->pos))
    {
        self->current_token.type = TOK_LITERAL;
        copy_literal(self);
        self->next_pos = self->pos + get_literal_length(self->pos);
        return 0;
    }

    if(classify_number(self) >= 0)
        return 0;

    return -1;
}

struct token* lexer_next_token(struct lexer* self)
{
    if(self->current_token.type == TOK_EOF)
        return &self->current_token;

    if(!self->accepted)
        return &self->current_token;

    if(self->next_pos)
        self->pos = self->next_pos;

    skip_whitespace(self);

    if(classify_token(self) < 0)
        return NULL;

    self->accepted = 0;

    return &self->current_token;
}

void lexer_accept_token(struct lexer* self)
{
    self->accepted = 1;
}
