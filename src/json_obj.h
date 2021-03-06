/*
 * Copyright (c) 2015, Marel hf
 * Copyright (c) 2015, Andri Yngvason
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef JSON_OBJ_H_INCLUDED_
#define JSON_OBJ_H_INCLUDED_

enum json_obj_type {
    JSON_OBJ_NULL = 0,
    JSON_OBJ_OBJ,
    JSON_OBJ_ARRAY,
    JSON_OBJ_NUMBER,
    JSON_OBJ_STRING,
    JSON_OBJ_TRUE,
    JSON_OBJ_FALSE,
    JSON_OBJ_BOOL
};

struct json_obj_pos {
    unsigned int start;
    unsigned int end;
};

struct json_obj_any {
    enum json_obj_type type;
    long long integer;
    double real;
    char* string_;
    int bool_;
    unsigned int obj_start;
    unsigned int obj_end;
};

struct json_obj {
    struct json_obj* next;
    struct json_obj* children;

    enum json_obj_type type;

    struct json_obj_pos key;
    struct json_obj_pos value;
};

struct json_obj_state {
    struct json_obj* obj;
    int line;
};

struct json_obj* json_lexer(const char* json);

struct json_obj* json_obj_new(int type, unsigned int start, unsigned int end);
struct json_obj* json_obj_new_obj(int type, struct json_obj* obj,
                                  unsigned int start, unsigned int end);

void json_obj_free(struct json_obj* obj);

void json_obj_set_key(struct json_obj* obj, struct json_obj_pos key);

char* json_string_decode(const char* input, size_t len);
char* json_string_encode(const char* input, size_t len);

#endif /* JSON_OBJ_H_INCLUDED_ */

