//Dynamic Array For Hash Table
typedef struct {
    char** array;
    size_t used;
    size_t size;
} Array;

static uint64_t hash_key(const char* key);
void initArray(Array *a, size_t initialSize);
void rehash(Array *a);
void collisionResolve(Array *a, char *element, int problemIndex);
void insertArray(Array *a, char *element);
void freeArray(Array *a);
void printArray(Array *a);
bool startsWith(const char *pre, const char *str);
void find(Array *a, char *target);
void delete(Array *a, char *target);