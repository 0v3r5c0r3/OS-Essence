#ifndef __ES_INCLUDE__
#define __ES_INCLUDE__

#include <essence.h>

#include "../config.h"

/* Logging */

void ES_logf(const char *label, const char *format, ...);

#ifdef FEATURE_CONSOLE
#define ES_infof(format, ...) ES_logf("[Info:DOOM] ", format, ## __VA_ARGS__)
#define ES_debugf(format, ...) ES_logf("[Debug:DOOM] ", format, ## __VA_ARGS__)
#define ES_warnf(format, ...) ES_logf("[Warn:DOOM] ", format, ## __VA_ARGS__)
#define ES_errorf(format, ...) ES_logf("[Error:DOOM] ", format, ## __VA_ARGS__)
#define ES_crashf(format, ...) ES_logf("[Crash:DOOM] ", format, ## __VA_ARGS__)
#else
#define ES_infof(format, ...)
#define ES_debugf(format, ...)
#define ES_warnf(format, ...)
#define ES_errorf(format, ...)
#define ES_crashf(format, ...)
#endif

/* Process */

__attribute__((noreturn))
void ES_Crash(const char *message);

__attribute__((noreturn))
void ES_Exit();

/* Math */

int ES_atoi(const char *str);
#define ES_atoi EsCRTatoi

float ES_atof(const char *str);
#define ES_atof EsCRTatof

int ES_abs(int n);
#define ES_abs EsCRTabs

float ES_fabs(float n);
#define ES_fabs EsCRTfabs

/* String */

int ES_strcasecmp(const char *s1, const char *s2);
int ES_strncasecmp(const char *s1, const char *s2, size_t count);
char ES_toupper(char c);

char ES_tolower(char c);
#define ES_tolower EsCRTtolower

void *ES_memcpy(void *destination, const void *source, size_t num);
#define ES_memcpy EsCRTmemcpy

void *ES_memset(void *ptr, int value, size_t num);
#define ES_memset EsCRTmemset

const char *ES_strchr(const char *str, int character);
#define ES_strchr EsCRTstrchr

size_t ES_strlen(const char *str);
#define ES_strlen EsCRTstrlen

int ES_strcmp(const char *s1, const char *s2);
#define ES_strcmp EsCRTstrcmp

int ES_strncmp(const char *s1, const char *s2, size_t count);
#define ES_strncmp EsCRTstrncmp

int ES_strstr(const char *haystack, const char *needle);
#define ES_strstr EsCRTstrstr

int ES_strcpy(char *dest, const char *src);
#define ES_strcpy EsCRTstrcpy

int ES_strncpy(char *dest, const char *src, size_t n);
#define ES_strncpy EsCRTstrncpy

int ES_isspace(int c);
#define ES_isspace EsCRTisspace

int ES_vsnprintf(char *buffer, size_t size, const char *format, va_list arguments);
#define ES_vsnprintf EsCRTvsnprintf

char *ES_strdup(const char *str);
#define ES_strdup EsCRTstrdup

long ES_strtol(const char *ptr, char **endptr, int base);
#define ES_strtol EsCRTstrtol

/* File */

typedef struct {
    EsFileInformation data;
    size_t ptr;
} ES_File;

typedef enum {
    ES_BYTE_MODE = 0b1,
    ES_READ_MODE = 0b10,
    ES_WRITE_MODE = 0b100
} ES_FileMode;

typedef enum {
    ES_SEEK_END,
    ES_SEEK_SET
} ES_Seek;

ES_File *ES_fopen(const char *path, ES_FileMode mode);
int ES_fvalid(ES_File *file);
size_t ES_fread(void *buffer, size_t size, size_t count, ES_File *file);
size_t ES_fwrite(const void *buffer, size_t size, size_t count, ES_File *file);
int ES_fseek(ES_File *file, size_t offset, ES_Seek origin);
size_t ES_ftell(ES_File *file);
size_t ES_fprintf(ES_File *file, const char *format, ...);
void ES_fclose(ES_File *file);

int ES_mkdir(const char *path);

int ES_remove(const char *path);
int ES_rename(const char *old, const char *new);

/* Heap */

void *ES_malloc(size_t size);
#define ES_malloc EsCRTmalloc

void *ES_calloc(size_t num, size_t size);
#define ES_calloc EsCRTcalloc

void *ES_realloc(void *ptr, size_t size);
#define ES_realloc EsCRTrealloc

void ES_free(void *ptr);
#define ES_free EsCRTfree

/* Graphics */

#ifdef CMAP256
typedef uint8_t pixel_t;
#else
typedef uint32_t pixel_t;
#endif

extern pixel_t* ES_ScreenBuffer;

/* Game */

void ES_Loop(void);
void ES_SleepMs(uint32_t ms);
uint32_t ES_GetTicksMs(void);
uint8_t ES_GetKey(int *pressed, unsigned char *key);

#endif
