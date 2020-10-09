#include <stdio.h>


typedef struct{
    char key[32];
    int ValueLength;
    char Value[1000000];
    kv_list_t *prev;
    kv_list_t *next;
} kv_list_t;


int kv_add(kv_list_t *lst, const char* key, const char *Value)
{
    kv_list_t *tmp = (kv_list_t*)malloc(sizeof(kv_list_t));
    strncpy(tmp->key, key, strlen(key));
    tmp->ValueLength = strlen(Value) - 1;
    strncpy(tmp->Value, Value, strlen(Value));
    

    kv_list_t *current = lst;
    for(;current->next != NULL;current = current->next)
    {
        
    }
    
    current->next = tmp;
    tmp->prev = current;
    tmp->next = NULL;
}


int kv_remove(kv_list_t *lst, char* key)
{
    kv_list_t *current = lst;
    for(;current->next != NULL;current = current->next)
    {
        if(strcmp(current->key, key) == 0)
        {
            break;
        }
    }

    if(strcmp(current->key, key) != 0)
    {
        return -1;
    }

    current->next->prev = current->prev;
    current->prev->next = current->next;
    current->next = NULL;
    current->prev = NULL;
    free(current);
    return 0;
}

int kv_show(kv_list_t *lst)
{
    
}

int main(void)
{
    kv_list_t head;
    kv_add(&head, "key1", "12345678");
    kv_
}
