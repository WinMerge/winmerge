#ifdef _DEBUG
void* operator new(size_t size, const char *file, int line);
void* operator new[](size_t size, const char *file, int line);
#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
void operator delete(void* p, const char *file, int line);
void operator delete[](void* p, const char *file, int line);
#endif
