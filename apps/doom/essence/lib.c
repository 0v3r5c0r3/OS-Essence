#include "include.h"

static char ES_LOG_BUFFER[4096];

void ES_vlogf(const char *label, const char *format, va_list args) {
    size_t label_size = ES_strlen(label);
    ES_memcpy(ES_LOG_BUFFER, label, label_size);

    size_t data_size = ES_vsnprintf(ES_LOG_BUFFER + label_size, sizeof(ES_LOG_BUFFER), format, args);

    ES_LOG_BUFFER[sizeof(ES_LOG_BUFFER) - 1] = '\0';

    EsPrintDirect(ES_LOG_BUFFER, data_size + label_size);
}

void ES_logf(const char *label, const char *format, ...) {
    va_list args;
    va_start(args, format);
    ES_vlogf(label, format, args);
    va_end(args);
}

char ES_toupper(char c) {
    return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
}

int ES_strcasecmp(const char *s1, const char *s2) {
    int c1, c2;

    for (;;) {
        c1 = ES_tolower(*s1++);
        c2 = ES_tolower(*s2++);

        if (c1 != c2) {
            return c1 - c2;
        }

        if (c1 == '\0') break;
    }

    return 0;
}

int ES_strncasecmp(const char *s1, const char *s2, register size_t n) {
    int c1, c2;

    for (; n != 0; --n) {
        c1 = ES_tolower(*s1++);
        c2 = ES_tolower(*s2++);

        if (c1 != c2) {
            return c1 - c2;
        }

        if (c1 == '\0') break;
    }

    return 0;
}

ES_File *ES_fopen(const char *path, ES_FileMode mode) {
    EsFileOpenFlags flags;
    if (mode & ES_WRITE_MODE) flags = ES_FILE_WRITE;
    else flags = ES_FILE_READ;

    if (flags & ES_FILE_READ) {
        EsNodeType nodeType;

        if (!EsPathExists(path, ES_strlen(path), &nodeType) || nodeType != ES_NODE_FILE) {
            return NULL;
        }
    }

    EsFileInformation data = EsFileOpen(path, ES_strlen(path), flags);
    if (data.error != ES_SUCCESS) {
        EsHandleClose(data.handle);
        return NULL;
    }

    ES_File *file = ES_malloc(sizeof(ES_File));
    if (file == NULL) {
        EsHandleClose(data.handle);
        return NULL;
    }

    file->ptr = 0;
    file->data = data;

    return file;
}

int ES_fvalid(ES_File *file) {
    return file->data.error == ES_SUCCESS;
}

size_t ES_fread(void *buffer, size_t size, size_t count, ES_File *file) {
    size_t offset = EsFileReadSync(file->data.handle, file->ptr, size * count, buffer);

    file->ptr += offset;

    return offset;
}

size_t ES_fwrite(const void *buffer, size_t size, size_t count, ES_File *file) {
    size_t offset = EsFileWriteSync(file->data.handle, file->ptr, size * count, buffer);
    file->ptr += offset;

    return offset;
}

int ES_fseek(ES_File *file, size_t offset, ES_Seek origin) {
    switch (origin) {
    case ES_SEEK_SET:
        if (offset >= file->data.size) return -1;
        file->ptr = offset;
        return 0;

    case ES_SEEK_END:
        file->ptr = file->data.size + 1;
        return 0;

    default: return -1;
    }
}

size_t ES_ftell(ES_File *file) {
    return file->ptr;
}

size_t ES_fprintf(ES_File *file, const char *format, ...) {
    size_t max_size = ES_strlen(format) * 4;
    void *buffer = ES_malloc(max_size);

    va_list args;
    va_start(args, format);
    ES_vsnprintf(buffer, max_size, format, args);
    va_end(args);

    size_t size = ES_strlen(buffer);

    size_t write_size = ES_fwrite(buffer, size, 1, file);

    ES_free(buffer);

    return write_size;
}

void ES_fclose(ES_File *file) {
    EsHandleClose(file->data.handle);
    ES_free(file);
}

int ES_remove(const char *path) {
    EsFileInformation file = EsFileOpen(path, ES_strlen(path), ES_FILE_WRITE);
    if (file.error != ES_SUCCESS) {
        EsHandleClose(file.handle);
        return -1;
    }

    EsFileDelete(file.handle);
    EsHandleClose(file.handle);

    return 0;
}

int ES_rename(const char *old, const char *new) {
    EsFileInformation oldFile = EsFileOpen(old, ES_strlen(old), ES_FILE_READ | ES_FILE_WRITE);
    if (oldFile.error != ES_SUCCESS) {
        EsHandleClose(oldFile.handle);
        return -1;
    }

    if (EsPathCreate(new, ES_strlen(new), ES_NODE_FILE, true) != ES_SUCCESS) {
        EsHandleClose(oldFile.handle);
        return -1;
    }

    EsFileInformation newFile = EsFileOpen(new, ES_strlen(new), ES_FILE_WRITE);
    if (newFile.error != ES_SUCCESS) {
        EsHandleClose(oldFile.handle);
        EsHandleClose(newFile.handle);
        return -1;
    }

    void *buffer = ES_malloc(oldFile.size);
    EsFileReadSync(oldFile.handle, 0, oldFile.size, buffer);
    EsFileWriteSync(newFile.handle, 0, newFile.size, buffer);
    ES_free(buffer);

    EsFileDelete(oldFile.handle);

    EsHandleClose(oldFile.handle);
    EsHandleClose(newFile.handle);

    return 0;
}

int ES_mkdir(const char *path) {
    return EsPathCreate(path, ES_strlen(path), ES_NODE_DIRECTORY, true) == ES_SUCCESS;
}
