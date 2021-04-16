#include <iostream>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>


//#include <immintrin.h>
//#include <xmmintrin.h>

//измерение функций после каждого изменения


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

} //выходной файл в tmp


    hash_t Polinomial(const char* str)
    {
        const hash_t A = (hash_t)2999;
        hash_t ret = 0;
        int idx = 0;
        while (str[idx])
            ret = A * ret + str[idx++];
        return ret;
    }

    /*unsigned short PolinomialSSE (const char* str)
    {
        if (sizeof(hash_t) != sizeof(unsigned short))
        {
            printf("Не буду считать\n");
            return 0xDEAD;
        }

        unsigned short ret = *(str++), new_ret = 0;

        if (ret == 0) return ret;

        const __m128i factor = _mm_set_epi16(
            (__uint16_t)(1), 
            (__uint16_t)(2999), 
            (__uint16_t)(2999*2999), 
            (__uint16_t)(1203205223), 
            (__uint16_t)(639935137), 
            (__uint16_t)(-684905449), 
            (__uint16_t)(-1037074063), 
            (__uint16_t)(-628792633));
        // Числа - short-степени 3189. Любое из этих чисел, что существенно, равно 0 при
        // short-умножении на char, если и только если char == '\0'.

        union
        {   __m128i pack;
            unsigned short arr [8];
        } sample, mult; 


        while (true)
        {
            
            sample.pack = _mm_set_epi16(str[6], str[5], str[4], str[3], str[2], str[1], str[0], ret);
            mult.pack = _mm_mullo_epi16(sample.pack, factor);

            new_ret = mult.arr[0];

            #define STEP(i)                                                              \
                new_ret += mult.arr[i];                                                  \
                if (mult.arr[i] == 0)                                                    \
                {                                                                        \
                    sample.pack = _mm_bslli_si128(sample.pack, 16 - 2*i);                \
                    mult.pack = _mm_mullo_epi16(sample.pack, factor);                    \
                    ret = 0;                                                             \
                    for (int j = 8 - i; j < 8; j++)                                      \
                        ret += mult.arr[j];                                              \
                    return ret;                                                          \
                }

            STEP(1)
            STEP(2)
            STEP(3)
            STEP(4)
            STEP(5)
            STEP(6)
            STEP(7)

            ret = new_ret;
            str += 7;
        }

        printf("Why am I here?\n");
        return 0xDEAD;
    }*/

   

    //hash_t HashSHA (const char* str)
    //{

    //}


class HashTable
{

    struct TableElement;
    struct ChainElement;

    struct ListElement
    {
        const char* value; /* блок динамической памяти со значением по ключу */

        ChainElement* next; /* указатель на структуру с парой ключ-значение с таким же хешем (начало односвязного списка) */

        ListElement (const char* init_value):
            value(alloc_n_copy(init_value)),
            next(nullptr)
        {}

        ListElement() {}

        void makeFrom (const char* value)
        {
            this->value = alloc_n_copy(value);
        }

    };

    struct TableElement : ListElement
    {
        int chainLength;
        ChainElement* tail;

        TableElement() {}
        TableElement (const char* init_value):
            ListElement(init_value),
            chainLength(0),
            tail(nullptr)
        {}

        int Hang(const char* val, const char* key)
        {

            if (this->next == nullptr)
            {
                this->next = new ChainElement(val, key);
                this->tail = this->next;
            }

            else
            {
                this->tail->next = new ChainElement(val, key);
                this->tail = this->tail->next;
            }

            this->chainLength++;

            return ! (this->tail->value && this->tail->key);

        }


    };

    struct ChainElement : ListElement
    {
        const char* key; /* блок динамической памяти со значением ключа */

        ChainElement()
        {
            key = nullptr;
            value = nullptr;
            next = nullptr;
        }

        ChainElement(const char* init_value, const char* init_key):
            ListElement(init_value),
            key (alloc_n_copy(init_key))
        {}

        ~ChainElement()
        {
            free ((void*)key);
            free ((void*)value);
        }

        void makeFrom(const char* value, const char* key)
        {
            this->value = alloc_n_copy(value);
            this->key = alloc_n_copy(key);
        }

    };



    static const hash_t hashMaxValue = (1 << (8 * sizeof(hash_t))) - 1;

    TableElement table[hashMaxValue + 1];
    
    hash_t (*getHashCode)(const char*) = nullptr;

public:

    HashTable()
    {
        memset(table, 0, (hashMaxValue + 1) * sizeof(*table));
    }
    ~HashTable()
    {
        Clear();
    }

    int Add (const char* new_elem, const char* key)
    {
        TableElement* target = table + getHashCode(key);

        if (! target->value)                                                       /* если хеш раньше не встречался, изменяем TableElement в массиве */
        {
            target->makeFrom(new_elem);
            return ! target->value;
        }

        return target->Hang(new_elem, key);                                        /* иначе прицепляем ChainElement к TableElement */
 
    }

    const char* Find (const char* key)
    {
        TableElement* target = table + getHashCode(key);
        if (! target->value)
            return nullptr;

        if (! target->next)
            return target->value;

        for (ChainElement* iter = target->next; iter; iter = iter->next)
        {
            if (strcmp(key, iter->key) == 0)
                return iter->value;
        }

        return target->value;

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

    int ToJSON (const char* filename, const char* comma)
    {
        FILE* out = fopen(filename, "w");
        if (out == nullptr)
            return 1;

        /*const int B_SIZE = hashMaxValue * 7 + 200;
        char buffer[B_SIZE];
        setvbuf(out, buffer, _IOLBF, B_SIZE);*/

        fprintf(out, 
        "{"                          "\n"
        "    \"comma\" : \"%s\","    "\n"
        "    \"hashMaxValue\" : %u," "\n"
        "    \"dispersion\" : %g,"   "\n"
        "    \"distribution\" :"     "\n"
        "    ["                      "\n", comma, this->hashMaxValue, CalcDispersion());


        
        TableElement* now = table;
        TableElement* end = table + hashMaxValue;
        for (;; now++)
        {
            fprintf(out,
            "        %d", now->value ? now->chainLength + 1 : 0);
            

            if (now == end)
                break;

            fprintf(out, ",\n");

        }

        fprintf(out, "  \n    ]\n}\n");
        fclose(out);

        return 0;

    }


    double CalcDispersion()
    {
        __int64_t sum = 0, sum_sqr = 0;

        TableElement* now = table;
        TableElement* end = table + hashMaxValue;
        for (; now <= end; now++)
        {
            int count = now->chainLength;
            sum += count;
            sum_sqr += (__int64_t)count * count;
            if (sum_sqr < 0) 
            {
                #ifdef MY_DEBUG
                printf("Overflow in CalcDispersion()\n");
                #endif
                return -1;
            }
        }

        return (double)sum_sqr / (hashMaxValue + 1) - (double)sum * sum / (hashMaxValue + 1) / (hashMaxValue + 1);

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
        TableElement* now = table;
        TableElement* end = table + hashMaxValue;
        for (; now <= end; now++)
        {
            if (! now->value)
                continue;
                
            ChainElement* iter = now->next;
            ChainElement* tail = iter;
            while (iter)
            {
                iter = iter->next;
                delete tail;
                tail = iter;
            }

            free((void*)now->value);
            now->value = nullptr;
            now->next = nullptr;
            now->chainLength = 0;
        }
    }

};

extern "C" hash_t Polinomial_asm(const char*);

int main()
{
    HashTable table;
    table.setHashFunc(Polinomial);

    struct timespec begin = { }, end = { };
    
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin);
    #include "data/add.txt"
    #include "data/find.txt"
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    double time_sec = (end.tv_sec - begin.tv_sec) + 1e-9 * (end.tv_nsec - begin.tv_nsec);
    printf("%g\n", time_sec);
}
























/*

void PlotFunc (HashTable &table, hash_t (*getHashCode)(const char*), const char* comma, const char* input, const char* output)
{
    table.Clear();
    table.setHashFunc(getHashCode);
    table.RawImport(input);
    table.ToJSON(output, comma);
    system("python3 plot_graph.py");
}

void TestFunc (HashTable &table, hash_t (*getHashCode)(const char*), const char* comma, const char* input)
{
    table.Clear();
    table.setHashFunc(getHashCode);
    table.RawImport(input);
    printf(
        "Функция %s:"      "\n"
        "    дисперсия %f" "\n", comma, table.CalcDispersion());

}

void PlotFuncs (HashTable &table, const char* input, const char* output)
{
    PlotFunc(table, Const, "Константа", input, output);
    PlotFunc(table, Length, "Длина строки", input, output);
    PlotFunc(table, Sum, "Сумма символов", input, output);
    PlotFunc(table, Average, "Средняя величина символа", input, output);
    PlotFunc(table, Polinomial, "Полиномиальный хеш", input, output);
    PlotFunc(table, RorHash, "Ror", input, output);
    PlotFunc(table, RolHash, "Rol", input, output);
    PlotFunc(table, Leader, "0-й символ", input, output);
}

void TestFuncs (HashTable &table, const char* input)
{
    TestFunc(table, Const, "Константа", input);
    TestFunc(table, Length, "Длина строки", input);
    TestFunc(table, Sum, "Сумма символов", input);
    TestFunc(table, Average, "Средняя величина символа", input);
    TestFunc(table, Polinomial, "Полиномиальный хеш", input);
    TestFunc(table, RorHash, "Ror", input);
    TestFunc(table, RolHash, "Rol", input);
    TestFunc(table, Leader, "0-й символ", input);
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

        int len = strlen(str);
        return len ? (hash_t)(sum / len) : ('a'+'z') / 2;
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
*/
// valgrind --tool=callgrind ./Table
// 
