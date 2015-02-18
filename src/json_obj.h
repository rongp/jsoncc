#ifndef JSON_OBJ_H_INCLUDED_
#define JSON_OBJ_H_INCLUDED_

#include "json_parser.h"

enum json_obj_type {
    JSON_OBJ_NULL = 0,
    JSON_OBJ_OBJ,
    JSON_OBJ_ARRAY,
    JSON_OBJ_NUMBER,
    JSON_OBJ_STRING,
    JSON_OBJ_TRUE,
    JSON_OBJ_FALSE
};

struct json_obj_pos {
    unsigned int start;
    unsigned int end;
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

struct json_obj* json_obj_new(int type, unsigned int start, unsigned int end);
struct json_obj* json_obj_new_obj(int type, struct json_obj* obj,
                                  unsigned int start, unsigned int end);

void json_obj_free(struct json_obj* obj);

void json_obj_set_key(struct json_obj* obj, struct json_obj_pos key);

#endif /* JSON_OBJ_H_INCLUDED_ */
