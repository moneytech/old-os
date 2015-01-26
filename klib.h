#include <stdbool.h> 
#include <stddef.h>
#include <stdint.h>

size_t kstrlen(const char* str);
int kstrcmp(const char *s1, const char *s2);
size_t kstrspn(const char *s, const char *accept);
char *kstrcpy(char *dst, const char *src);

uint32_t katou(char *nptr);
void* kmemset(void *s, uint8_t c, size_t n);
void* kmemmove(void *dst, const void *src, size_t n);