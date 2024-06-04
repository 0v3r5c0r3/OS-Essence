//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 1993-2008 Raven Software
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//      Miscellaneous.
//

#include "essence/include.h"

#include "config.h"

#include "doomtype.h"

#include "i_swap.h"
#include "i_system.h"
#include "i_video.h"
#include "m_misc.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

//
// Create a directory
//

void M_MakeDirectory(char *path)
{
    ES_mkdir(path);
}

// Check if a file exists

boolean M_FileExists(char *filename)
{
    ES_File *fstream;

    fstream = ES_fopen(filename, ES_READ_MODE);
    if (fstream != NULL)
    {
        ES_fclose(fstream);
        return true;
    }
    else
    {
        return false;
    }
}

//
// Determine the length of an open file.
//

long M_FileLength(ES_File *handle)
{ 
    long savedpos;
    long length;

    // save the current position in the file
    savedpos = ES_ftell(handle);
    
    // jump to the end and find the length
    ES_fseek(handle, 0, ES_SEEK_END);
    length = ES_ftell(handle);

    // go back to the old location
    ES_fseek(handle, savedpos, ES_SEEK_SET);

    return length;
}

//
// M_WriteFile
//

boolean M_WriteFile(char *name, void *source, int length)
{
    ES_File *handle;
    int	count;

    handle = ES_fopen(name, ES_WRITE_MODE | ES_BYTE_MODE);
    if (handle == NULL) return false;

    count = ES_fwrite(source, length, 1, handle);
    ES_fclose(handle);
	
    if (count < length)
	return false;
		
    return true;
}


//
// M_ReadFile
//

int M_ReadFile(char *name, byte **buffer)
{
    ES_File *handle;
    int	count, length;
    byte *buf;

    handle = ES_fopen(name, ES_READ_MODE | ES_BYTE_MODE);
    if (handle == NULL) I_Error ("Couldn't read file %s", name);

    // find the size of the file by seeking to the end and
    // reading the current position

    length = M_FileLength(handle);
    
    buf = Z_Malloc (length, PU_STATIC, NULL);
    count = ES_fread(buf, length, 1, handle);
    ES_fclose (handle);
	
    if (count < length)
	I_Error ("Couldn't read file %s", name);
		
    *buffer = buf;
    return length;
}

// Returns the path to a temporary file of the given name, stored
// inside the system temporary directory.
//
// The returned value must be freed with Z_Free after use.

char *M_TempFile(char *s)
{
    return M_StringJoin(TMP_DIR, DIR_SEPARATOR_S, s, NULL);
}

boolean M_StrToInt(const char *str, int *result)
{
    char *next = 0;

    if (str[1] == '0' && (str[2] == 'x' || str[2] == 'X')) {
        *result = (int)ES_strtol(str+3, &next, 16);
    } else if (str[1] == '0') {
        *result = (int)ES_strtol(str+3, &next, 8);
    } else {
        *result = (int)ES_strtol(str+3, &next, 10);
    }

    return next != 0 && next != str;
}

void M_ExtractFileBase(char *path, char *dest)
{
    char *src;
    char *filename;
    int length;

    src = path + ES_strlen(path) - 1;

    // back up until a \ or the start
    while (src != path && *(src - 1) != DIR_SEPARATOR)
    {
	src--;
    }

    filename = src;

    // Copy up to eight characters
    // Note: Vanilla Doom exits with an error if a filename is specified
    // with a base of more than eight characters.  To remove the 8.3
    // filename limit, instead we simply truncate the name.

    length = 0;
    ES_memset(dest, 0, 8);

    while (*src != '\0' && *src != '.')
    {
        if (length >= 8)
        {
            ES_warnf("Truncated '%s' lump name to '%.8s'.\n",
                   filename, dest);
            break;
        }

	dest[length++] = ES_toupper((int)*src++);
    }
}

//---------------------------------------------------------------------------
//
// PROC M_ForceUppercase
//
// Change string to uppercase.
//
//---------------------------------------------------------------------------

void M_ForceUppercase(char *text)
{
    char *p;

    for (p = text; *p != '\0'; ++p)
    {
        *p = ES_toupper(*p);
    }
}

//
// M_StrCaseStr
//
// Case-insensitive version of strstr()
//

char *M_StrCaseStr(char *haystack, char *needle)
{
    unsigned int haystack_len;
    unsigned int needle_len;
    unsigned int len;
    unsigned int i;

    haystack_len = ES_strlen(haystack);
    needle_len = ES_strlen(needle);

    if (haystack_len < needle_len)
    {
        return NULL;
    }

    len = haystack_len - needle_len;

    for (i = 0; i <= len; ++i)
    {
        if (!ES_strncasecmp(haystack + i, needle, needle_len))
        {
            return haystack + i;
        }
    }

    return NULL;
}

//
// Safe version of strdup() that checks the string was successfully
// allocated.
//

char *M_StringDuplicate(const char *orig)
{
    char *result;

    result = ES_strdup(orig);

    if (result == NULL)
    {
        I_Error("Failed to duplicate string (length %i)\n",
                ES_strlen(orig));
    }

    return result;
}

//
// String replace function.
//

char *M_StringReplace(const char *haystack, const char *needle,
                      const char *replacement)
{
    char *result, *dst;
    const char *p;
    size_t needle_len = ES_strlen(needle);
    size_t result_len, dst_len;

    // Iterate through occurrences of 'needle' and calculate the size of
    // the new string.
    result_len = ES_strlen(haystack) + 1;
    p = haystack;

    for (;;)
    {
        p = ES_strstr(p, needle);
        if (p == NULL)
        {
            break;
        }

        p += needle_len;
        result_len += ES_strlen(replacement) - needle_len;
    }

    // Construct new string.

    result = ES_malloc(result_len);
    if (result == NULL)
    {
        I_Error("M_StringReplace: Failed to allocate new string");
        return NULL;
    }

    dst = result; dst_len = result_len;
    p = haystack;

    while (*p != '\0')
    {
        if (!ES_strncmp(p, needle, needle_len))
        {
            M_StringCopy(dst, replacement, dst_len);
            p += needle_len;
            dst += ES_strlen(replacement);
            dst_len -= ES_strlen(replacement);
        }
        else
        {
            *dst = *p;
            ++dst; --dst_len;
            ++p;
        }
    }

    *dst = '\0';

    return result;
}

// Safe string copy function that works like OpenBSD's strlcpy().
// Returns true if the string was not truncated.

boolean M_StringCopy(char *dest, const char *src, size_t dest_size)
{
    size_t len;

    if (dest_size >= 1)
    {
        dest[dest_size - 1] = '\0';
        ES_strncpy(dest, src, dest_size - 1);
    }
    else
    {
        return false;
    }

    len = ES_strlen(dest);
    return src[len] == '\0';
}

// Safe string concat function that works like OpenBSD's strlcat().
// Returns true if string not truncated.

boolean M_StringConcat(char *dest, const char *src, size_t dest_size)
{
    size_t offset;

    offset = ES_strlen(dest);
    if (offset > dest_size)
    {
        offset = dest_size;
    }

    return M_StringCopy(dest + offset, src, dest_size - offset);
}

// Returns true if 's' begins with the specified prefix.

boolean M_StringStartsWith(const char *s, const char *prefix)
{
    return ES_strlen(s) > ES_strlen(prefix)
        && ES_strncmp(s, prefix, ES_strlen(prefix)) == 0;
}

// Returns true if 's' ends with the specified suffix.

boolean M_StringEndsWith(const char *s, const char *suffix)
{
    return ES_strlen(s) >= ES_strlen(suffix)
        && ES_strcmp(s + ES_strlen(s) - ES_strlen(suffix), suffix) == 0;
}

// Return a newly-malloced string with all the strings given as arguments
// concatenated together.

char *M_StringJoin(const char *s, ...)
{
    char *result;
    const char *v;
    va_list args;
    size_t result_len;

    result_len = ES_strlen(s) + 1;

    va_start(args, s);
    for (;;)
    {
        v = va_arg(args, const char *);
        if (v == NULL)
        {
            break;
        }

        result_len += ES_strlen(v);
    }
    va_end(args);

    result = ES_malloc(result_len);

    if (result == NULL)
    {
        I_Error("M_StringJoin: Failed to allocate new string.");
        return NULL;
    }

    M_StringCopy(result, s, result_len);

    va_start(args, s);
    for (;;)
    {
        v = va_arg(args, const char *);
        if (v == NULL)
        {
            break;
        }

        M_StringConcat(result, v, result_len);
    }
    va_end(args);

    return result;
}

// Safe, portable vsnprintf().
int M_vsnprintf(char *buf, size_t buf_len, const char *s, va_list args)
{
    size_t result;

    if (buf_len < 1)
    {
        return 0;
    }

    // Windows (and other OSes?) has a vsnprintf() that doesn't always
    // append a trailing \0. So we must do it, and write into a buffer
    // that is one byte shorter; otherwise this function is unsafe.
    result = ES_vsnprintf(buf, buf_len, s, args);

    // If truncated, change the final char in the buffer to a \0.
    // A negative result indicates a truncated buffer on Windows.
    if (result < 0 || result >= buf_len)
    {
        buf[buf_len - 1] = '\0';
        result = buf_len - 1;
    }

    return result;
}

// Safe, portable snprintf().
int M_snprintf(char *buf, size_t buf_len, const char *s, ...)
{
    va_list args;
    int result;
    va_start(args, s);
    result = M_vsnprintf(buf, buf_len, s, args);
    va_end(args);
    return result;
}
