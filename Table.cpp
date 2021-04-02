#include <iostream>
#include <limits.h>
#include "SmartBuffer.h"

class HashTable
{

    struct TableElement;
    struct ChainElement;

    struct TableElement
    {
        size_t value;
        ChainElement* next;

        TableElement():
            value(-1),
            next(nullptr)
        {}

        TableElement(size_t init_value):
            value(init_value),
            next(nullptr)
        {}

        void Hang(size_t val, size_t key)
        {
            if (this->next == nullptr)
            {
                this->next = new ChainElement(val, key);
            }
            else
            {
                ChainElement* iter = this->next;
                while (iter->next) iter = iter->next;
                iter->next = new ChainElement(val, key);
            }
        }
    };

    struct ChainElement : TableElement
    {
        size_t key;

        ChainElement():
            TableElement(),
            key (-1)
        {}

        ChainElement(size_t init_value, size_t init_key):
            TableElement(init_value),
            key (init_key)
        {}

    };

    typedef unsigned short hash_t;



    static const hash_t hashMaxValue = (1 << (8 * sizeof(hash_t))) - 1;

    TableElement table[hashMaxValue + 1];
    Buffer strings;

    

    hash_t getHashCode(const char* elem)
    {
        const hash_t A = (hash_t)38276213;
        hash_t ret = 0;
        int idx = 0;
        while (elem[idx])
            ret = A * ret + elem[idx++];
        return ret;

        /*static const ret_t hashFunctions[] = 
        {
            [](const char* str) { return (hash_t)1; },
            [](const char* str) { return (hash_t)strlen(str); },
            [](const char* str)
            {
                hash_t ret = 0;
                int idx = 0;
                while (str[idx])
                    ret += str[idx++];
                return ret;
            },
            [](const char* str)
            {
                hash_t ret = 0;
                int idx = 0;
                while (str[idx])
                    ret += str[idx++];
                return ret / strlen(str);
            },
            [](const char* str) { return (hash_t)str[0]; },
            [](const char* str)
            {
                const hash_t A = (hash_t)38276213;
                hash_t ret = 0;
                int idx = 0;
                while (str[idx])
                    ret = A * ret + str[idx++];
                return ret;
            }
        };*/
    }

public:

    HashTable()
    {
        if (MakeBuffer(&strings, 0)) 
        {
            printf("Error while allocating data buffer for HashTable object [0x%p]\n", this);
        }
    }

    ~HashTable()
    {
        for (int i = 0; i < hashMaxValue + 1; i++)
        {
            TableElement* iter = table[i].next;
            TableElement* tail = iter;
            while (iter)
            {
                iter = iter->next;
                delete tail;
                tail = iter;
            }
        }

        BufferDistruct(&strings);
    }

    int Add (const char* new_elem, const char* key)
    {
        size_t new_elem_idx = AddToEnd(&(this->strings), strlen(new_elem), new_elem); /* сохраняем значение */
        if (new_elem_idx == buffer_errors::ERR_NO_MEM)
            return 1;
        if (AddChar(&(this->strings), '\0', -1) == buffer_errors::ERR_NO_MEM)
            return 1;

        //hash_t hash = getHashCode(new_elem);
        hash_t hash = getHashCode(key);                                               /* хешируем ключ */

        if (table[hash].value == -1)                                                  /* если хеш раньше не встречался, изменяем TableElement в массиве */
        {
            table[hash].value = new_elem_idx;
            return 0;
        }

        size_t key_idx =  AddToEnd(&(this->strings), strlen(key), key);
        if (AddChar(&(this->strings), '\0', -1) == buffer_errors::ERR_NO_MEM)
            return 1;

        table[hash].Hang(new_elem_idx, key_idx);                                      /* иначе прицепляем ChainElement к TableElement */

        return 0;
    }


    void Dump(bool fullDump)
    {
        for (int i = 0; i < hashMaxValue + 1; i++)
        {
            if (!fullDump && this->table[i].value == -1) continue;
            printf("%d:\n", i);
            for (TableElement* iter = &(this->table[i]); true; iter = iter->next)
            {
                printf("    %p", iter);
                if (iter == nullptr)
                {
                    printf("\n");
                    break;
                }

                printf(" index %d", (signed)iter->value);
                if (iter->value == -1)
                {
                    printf("\n");
                    continue;
                }

                char* str = GetElemByInd(&(this->strings), iter->value);

                printf(", string [%p]", str);
                if (str == nullptr)
                {
                    printf("\n");
                    continue;
                }

                fflush(stdout);
                printf(" %s\n", str);


            }
        }
    }

};

int main()
{
    HashTable table;
    printf("%d ", table.Add("89", "John"));
    fflush(stdout);
    printf("%d ", table.Add("90", "John"));
    fflush(stdout);
    printf("%d ", table.Add("91", "John"));
    fflush(stdout);
    printf("%d ", table.Add("92", "Helen"));
    fflush(stdout);
    printf("%d ", table.Add("89", "EVANGELION"));
    fflush(stdout);
    
    table.Dump(false);

}

