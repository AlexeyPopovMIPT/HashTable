#include <iostream>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>

typedef unsigned short hash_t;

const char* alloc_n_copy(const char* src)
{
    int len = strlen(src);
    char* dest = (char*) calloc(len + 1, sizeof(char));
    if (dest == nullptr) return nullptr;
    memcpy(dest, src, len + 1);
    return dest;
}


int GetFileSize(const char* const filename) {

    struct stat fileProperties;
    int errcode = stat(filename, &fileProperties);

    if (errcode != 0) return -1;
    return fileProperties.st_size;

}


unsigned char* GetDataAndCountStrings(FILE* Source, unsigned char* data, int* datacpc, int* stringscnt) {

    int localstrcnt = 0;
    if (stringscnt == NULL) {
        stringscnt = &localstrcnt;
    }

    fread(data, sizeof(unsigned char), *datacpc, Source);

    *stringscnt = 0;

    unsigned char* writer = data;
    unsigned char current_wrote = '\0';

    for (int i = 0; i < *datacpc; i++) {
        if (*(data+i) == '\r') continue;

        if (*(data + i) == '\n') {
            if (current_wrote == '\0') continue;
            
            *writer = '\0';
            writer++;
            current_wrote = '\0';
            ++*stringscnt;
        }
        else {
            *writer = *(data + i);
            writer++;
            current_wrote = *(data + i);
        }

    }
    if (current_wrote != '\0')
    {
        *writer = '\0';
        writer++;
        ++*stringscnt;
    }

    data = (unsigned char*) realloc(data, writer - data);
    *datacpc = writer - data;
    return data;

}



    hash_t Polinomial(const char* str)
    {
        const hash_t A = (hash_t)38276213; //89
        hash_t ret = 0;
        int idx = 0;
        while (str[idx])
            ret = A * ret + str[idx++];
        return ret;
    }

    hash_t Const (const char* str)
    {
        return (hash_t)1;
    }

    hash_t Length (const char* str)
    {
        return (hash_t)strlen(str);
    }

    hash_t Sum (const char* str)
    {
        hash_t ret = 0;
        int idx = 0;
        while (str[idx])
            ret += str[idx++];
        return ret;
    }

    hash_t Average (const char* str)
    {
        __int64_t sum = 0;
        int idx = 0;
        while (str[idx])
            sum += str[idx++];
        return (hash_t)(sum / strlen(str));
    }

    hash_t Leader (const char* str)
    {
        return (hash_t)str[0];
    }

    hash_t RorHash (const char* str)
    {
        hash_t ret = 0;
        int idx = 0;
        while (str[idx])
            ret = ((ret << (8 * sizeof(hash_t) - 1)) | (ret >> 1)) ^ str[idx++];
        return ret;
    }

    hash_t RolHash (const char* str)
    {
        hash_t ret = 0;
        int idx = 0;
        while (str[idx])
            ret = ((ret >> (8 * sizeof(hash_t) - 1)) | (ret << 1)) ^ str[idx++];
        return ret;
    }


class HashTable
{

    struct TableElement;
    struct ChainElement;

    struct TableElement
    {
        const char* value; /* блок динамической памяти со значением по ключу */

        ChainElement* next; /* указатель на структуру с парой ключ-значение с таким же хешем (начало односвязного списка) */

        TableElement():
            value(nullptr),
            next(nullptr)
        {}

        TableElement (const char* init_value):
            value(alloc_n_copy(init_value)),
            next(nullptr)
        {}

        void makeFrom (const char* value)
        {
            this->value = alloc_n_copy(value);
        }

        ~TableElement()
        {
            free ((void*)value);
        }

        int Hang(const char* val, const char* key)
        {
            ChainElement* iter = nullptr;
            if (this->next == nullptr)
            {
                this->next = new ChainElement(val, key);
                iter = this->next;
            }

            else
            {
                for (iter = this->next; iter->next; iter = iter->next);
                iter->next = new ChainElement(val, key);
                iter = iter->next;
            }

            return ! (iter->isValueDefined() && iter->isKeyDefined());

        }

        bool isValueDefined() { return this->value; }
        bool isNextDefined()  { return this->next; }

    };

    struct ChainElement : TableElement
    {
        const char* key; /* блок динамической памяти со значением ключа */

        ChainElement():
            TableElement(),
            key (nullptr)
        {}

        ChainElement(const char* init_value, const char* init_key):
            TableElement(init_value),
            key (alloc_n_copy(init_key))
        {}

        ~ChainElement()
        {
            free((void*)key);
        }

        void makeFrom(const char* value, const char* key)
        {
            this->value = alloc_n_copy(value);
            this->key = alloc_n_copy(key);
        }

        bool isKeyDefined() { return this->key; }



    };



    static const hash_t hashMaxValue = (1 << (8 * sizeof(hash_t))) - 1;

    TableElement table[hashMaxValue + 1];
    
    hash_t (*getHashCode)(const char*) = nullptr;

public:


    ~HashTable()
    {
        Clear();
    }

    int Add (const char* new_elem, const char* key)
    {
        hash_t hash = (*getHashCode)(key);                                            /* хешируем ключ */

        if (!table[hash].isValueDefined())                                            /* если хеш раньше не встречался, изменяем TableElement в массиве */
        {
            table[hash].makeFrom(new_elem);
            return ! table[hash].isValueDefined();
        }

        return table[hash].Hang(new_elem, key);                                      /* иначе прицепляем ChainElement к TableElement */

    }

    const char* Find (const char* key)
    {
        hash_t hash = getHashCode(key); 
        if (! this->table[hash].isValueDefined())
            return nullptr;

        if (! this->table[hash].isNextDefined())
            return this->table[hash].value;

        for (ChainElement* iter = this->table[hash].next; iter; iter = iter->next)
        {
            if (strcmp(key, iter->key) == 0)
                return iter->value;
        }

        return this->table[hash].value;

    }

    int RawImport (const char* filename)
    {
        FILE* in = fopen(filename, "r");
        if (in == nullptr)
            return 1;

        int size = GetFileSize(filename);

        char* rawIn = (char*) calloc(size, sizeof(char));
        if (!rawIn) return 1;

        rawIn = (char*)GetDataAndCountStrings(in, (unsigned char*)rawIn, &size, nullptr);

        int idx = 0;

        while (idx < size)
        {
            char* key_ptr = rawIn + idx;
            char* value_ptr = strchr(rawIn + idx, ':') + 2;
            idx = idx + strlen(rawIn + idx) + 1;
            value_ptr[-2] = '\0';
            this->Add(value_ptr, key_ptr);
        }

        free(rawIn);
        fclose(in);
        return 0;
        
    }

    int Export (const char* filename, const char* comma)
    {
        FILE* out = fopen(filename, "w");
        if (out == nullptr)
            return 1;

        fprintf(out, 
        "{"                          "\n"
        "    \"comma\" : \"%s\","    "\n"
        "    \"hashMaxValue\" : %u," "\n"
        "    \"distribution\" :"     "\n"
        "    ["                      "\n", comma, this->hashMaxValue);

        for (int i = 0; true; i++)
        {
            int count = 0;
            if (this->table[i].isValueDefined())
            {
                count = 1;
                for (ChainElement* iter = this->table[i].next; iter; iter = iter->next)
                    count++;
            }
            fprintf(out,
            "        %d", count);
            if (i == hashMaxValue)
                break;

            fprintf(out, ",\n");
        }

        fprintf(out, "\n    ]\n}\n");
        fclose(out);

        return 0;

    }

    void setHashFunc(hash_t (*func)(const char*))
    {
        this->getHashCode = func;
    }


    void Dump(bool fullDump)
    {
        printf("╔══════════════════════════════════╗""\n"
               "║ Dumping table [%16p] "          "║""\n"
               "╚══════════════════════════════════╝""\n", this);
        for (int i = 0; i < hashMaxValue + 1; i++)
        {
            const char* value_now = this->table[i].value;
            if (!fullDump && value_now == nullptr) continue;

            printf("%d:\n    [%p]", i, value_now);

            if (value_now == nullptr)
            {
                printf("\n");
                continue;
            }

            printf(" \"%s\"\n", value_now);

            for (ChainElement* iter = this->table[i].next; iter; iter = iter->next)
            {
                printf("    [%p] \"%s\", key=[%p] \"%s\"\n", iter->value, iter->value, iter->key, iter->key);
            }
            
        }
    }
    void Clear()
    {
        for (int i = 0; i < hashMaxValue + 1; i++)
        {
            ChainElement* iter = table[i].next;
            ChainElement* tail = iter;
            while (iter)
            {
                iter = iter->next;
                delete tail;
                tail = iter;
            }

            free((void*)table[i].value);
            table[i].value = nullptr;
            table[i].next = nullptr;
        }
    }

};


int main()
{
    HashTable table;

    table.setHashFunc(Const);
    table.RawImport("data/dict.txt");
    table.Export("table.json", "Константа");
    putchar('1'); fflush(stdout);
    system("python3 plot.py");
    putchar('2'); fflush(stdout);

    table.Clear();
    table.setHashFunc(Length);
    table.RawImport("data/dict.txt");
    table.Export("table.json", "Длина строки");
    putchar('1'); fflush(stdout);
    system("python3 plot.py");
    putchar('2'); fflush(stdout);

    table.Clear();
    table.setHashFunc(Sum);
    table.RawImport("data/dict.txt");
    table.Export("table.json", "Сумма символов");
    putchar('1'); fflush(stdout);
    system("python3 plot.py");
    putchar('2'); fflush(stdout);

    table.Clear();
    table.setHashFunc(Average);
    table.RawImport("data/dict.txt");
    table.Export("table.json", "Средняя величина символа");
    putchar('1'); fflush(stdout);
    system("python3 plot.py");
    putchar('2'); fflush(stdout);

    table.Clear();
    table.setHashFunc(Leader);
    table.RawImport("data/dict.txt");
    table.Export("table.json", "0-й символ");
    putchar('1'); fflush(stdout);
    system("python3 plot.py");
    putchar('2'); fflush(stdout);

    table.Clear();
    table.setHashFunc(Polinomial);
    table.RawImport("data/dict.txt");
    table.Export("table.json", "Полиномиальный хеш");
    putchar('1'); fflush(stdout);
    system("python3 plot.py");
    putchar('2'); fflush(stdout);

    table.Clear();
    table.setHashFunc(RorHash);
    table.RawImport("data/dict.txt");
    table.Export("table.json", "Ror");
    putchar('1'); fflush(stdout);
    system("python3 plot.py");
    putchar('2'); fflush(stdout);

    table.Clear();
    table.setHashFunc(RolHash);
    table.RawImport("data/dict.txt");
    table.Export("table.json", "Rol");
    putchar('1'); fflush(stdout);
    system("python3 plot.py");
    putchar('2'); fflush(stdout);

}
