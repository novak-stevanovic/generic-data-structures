#include "gds_vector.h"
#include "gds_hash_map.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct GDSString
{
    char* _string;
    size_t len;
};

void gds_string_init(struct GDSString* gds_string, char* string)
{
    size_t str_len = strlen(string);

    gds_string->_string = malloc(str_len);
    gds_string->len = str_len;

    strcpy(gds_string->_string, string);
}

size_t hash_func_example(const void* key, size_t max_value)
{
    struct GDSString* _key = (struct GDSString*)key;

    size_t sum = 0;
    size_t i;
    for(i = 0; i < _key->len; i++)
        sum += _key->_string[i];

    return (sum % max_value);
}

bool key_compare_func_example(const void* key1, const void* key2)
{
    return strcmp(((struct GDSString*)key1)->_string, ((struct GDSString*)key2)->_string);
}

void init_hm(GDSHashMap* hm)
{
    struct GDSString str1;
    gds_string_init(&str1, "Emilija");
    int val1 = 29;

    struct GDSString str2;
    gds_string_init(&str2, "Novak");
    int val2 = 24;

    gds_hash_map_set(hm, &str1, &val1);
    gds_hash_map_set(hm, &str2, &val2);

    printf("%d\n", *(int*)gds_hash_map_get(hm, &str1));
    printf("%d\n", *(int*)gds_hash_map_get(hm, &str2));
}

int main(int argc, char *argv[])
{
    GDSHashMap* hm = gds_hash_map_create(sizeof(struct GDSString), sizeof(int), hash_func_example, key_compare_func_example);

    init_hm(hm);

    return 0;
}
