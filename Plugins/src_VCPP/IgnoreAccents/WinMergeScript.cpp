// SPDX-License-Identifier: GPL-2.0-or-later

#include "stdafx.h"
#include "IgnoreAccents.h"
#include "WinMergeScript.h"

#include <windows.h>
#include <winnls.h>
#include <cstdio>
#include <cstddef>

#include <cstring>

#pragma comment(lib, "Normaliz.lib")

// ------------------------------------------------------------
// Buffered input: 64 KB
// ------------------------------------------------------------

class BufferedReader
{
public:
    explicit BufferedReader(FILE* file)
        : m_file(file), m_pos(0), m_size(0)
    {
    }

    bool ReadByte(unsigned char& value, bool& eof)
    {
        eof = false;

        if (m_pos >= m_size)
        {
            m_size = fread(m_buffer, 1, sizeof(m_buffer), m_file);
            m_pos = 0;

            if (m_size == 0)
            {
                if (ferror(m_file))
                    return false;

                eof = true;
                return true;
            }
        }

        value = m_buffer[m_pos++];
        return true;
    }

private:
    FILE* m_file;
    unsigned char m_buffer[64 * 1024];
    size_t m_pos;
    size_t m_size;
};

// ------------------------------------------------------------
// Buffered output: 64 KB
// ------------------------------------------------------------

class BufferedWriter
{
public:
    explicit BufferedWriter(FILE* file)
        : m_file(file), m_size(0)
    {
    }

    ~BufferedWriter()
    {
        Flush();
    }

    bool Write(const unsigned char* data, size_t length)
    {
        while (length > 0)
        {
            size_t available = sizeof(m_buffer) - m_size;

            if (available == 0)
            {
                if (!Flush())
                    return false;

                available = sizeof(m_buffer);
            }

            size_t amount =
                (length < available) ? length : available;

            memcpy(m_buffer + m_size, data, amount);

            m_size += amount;
            data += amount;
            length -= amount;
        }

        return true;
    }

    bool Flush()
    {
        if (m_size == 0)
            return true;

        if (fwrite(m_buffer, 1, m_size, m_file) != m_size)
            return false;

        m_size = 0;
        return true;
    }

private:
    FILE* m_file;
    unsigned char m_buffer[64 * 1024];
    size_t m_size;
};

// ------------------------------------------------------------
// Unicode normalization
// ------------------------------------------------------------

static bool NormalizeCharacter(
    const wchar_t* input,
    int inputLength,
    wchar_t* output,
    int& outputLength,
    bool& changed)
{
    wchar_t normalized[32];
    WORD types[32];

    int count = NormalizeString(
        NormalizationD,
        input,
        inputLength,
        normalized,
        32);

    if (count <= 0 || count > 32)
        return false;

    if (!GetStringTypeW(
        CT_CTYPE3,
        normalized,
        count,
        types))
        return false;

    outputLength = 0;

    for (int i = 0; i < count; ++i)
    {
        if (types[i] & (
            C3_NONSPACING |
            C3_DIACRITIC |
            C3_VOWELMARK))
        {
            changed = true;
            continue;
        }

        output[outputLength++] = normalized[i];
    }

    return true;
}

// ------------------------------------------------------------
// UTF-8 decoder
// ------------------------------------------------------------

static bool ReadUtf8(
    BufferedReader& reader,
    unsigned int& cp,
    bool& eof)
{
    unsigned char b0 = 0;

    if (!reader.ReadByte(b0, eof))
        return false;

    if (eof)
        return true;

    if (b0 < 0x80)
    {
        cp = b0;
        return true;
    }

    int extra;
    unsigned int minimum;

    if (b0 >= 0xC2 && b0 <= 0xDF)
    {
        extra = 1;
        cp = b0 & 0x1F;
        minimum = 0x80;
    }
    else if (b0 >= 0xE0 && b0 <= 0xEF)
    {
        extra = 2;
        cp = b0 & 0x0F;
        minimum = 0x800;
    }
    else if (b0 >= 0xF0 && b0 <= 0xF4)
    {
        extra = 3;
        cp = b0 & 0x07;
        minimum = 0x10000;
    }
    else
    {
        return false;
    }

    for (int i = 0; i < extra; ++i)
    {
        unsigned char b = 0;
        bool byteEof = false;

        if (!reader.ReadByte(b, byteEof) || byteEof)
            return false;

        if ((b & 0xC0) != 0x80)
            return false;

        cp = (cp << 6) | (b & 0x3F);
    }

    if (cp < minimum ||
        cp > 0x10FFFF ||
        (cp >= 0xD800 && cp <= 0xDFFF))
        return false;

    return true;
}

// ------------------------------------------------------------
// UTF-8 encoder
// ------------------------------------------------------------

static bool WriteUtf8(
    BufferedWriter& writer,
    const wchar_t* text,
    int length)
{
    unsigned char bytes[4];

    for (int i = 0; i < length; ++i)
    {
        unsigned int cp = (unsigned int)text[i];

        if (cp >= 0xD800 && cp <= 0xDBFF)
        {
            if (i + 1 >= length)
                return false;

            unsigned int low = (unsigned int)text[++i];

            if (low < 0xDC00 || low > 0xDFFF)
                return false;

            cp = 0x10000 +
                ((cp - 0xD800) << 10) +
                (low - 0xDC00);
        }
        else if (cp >= 0xDC00 && cp <= 0xDFFF)
        {
            return false;
        }

        int count;

        if (cp < 0x80)
        {
            bytes[0] = (unsigned char)cp;
            count = 1;
        }
        else if (cp < 0x800)
        {
            bytes[0] = (unsigned char)(0xC0 | (cp >> 6));
            bytes[1] = (unsigned char)(0x80 | (cp & 0x3F));
            count = 2;
        }
        else if (cp < 0x10000)
        {
            bytes[0] = (unsigned char)(0xE0 | (cp >> 12));
            bytes[1] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
            bytes[2] = (unsigned char)(0x80 | (cp & 0x3F));
            count = 3;
        }
        else
        {
            bytes[0] = (unsigned char)(0xF0 | (cp >> 18));
            bytes[1] = (unsigned char)(0x80 | ((cp >> 12) & 0x3F));
            bytes[2] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
            bytes[3] = (unsigned char)(0x80 | (cp & 0x3F));
            count = 4;
        }

        if (!writer.Write(bytes, count))
            return false;
    }

    return true;
}

// ------------------------------------------------------------
// UTF-8 processing
// ------------------------------------------------------------

static bool ProcessUtf8(
    FILE* src,
    BufferedWriter& dst,
    bool& changed)
{
    BufferedReader reader(src);

    for (;;)
    {
        unsigned int cp = 0;
        bool eof = false;

        if (!ReadUtf8(reader, cp, eof))
            return false;

        if (eof)
            return dst.Flush();

        wchar_t input[2];
        int inputLength = 1;

        if (cp <= 0xFFFF)
        {
            input[0] = (wchar_t)cp;
        }
        else
        {
            cp -= 0x10000;

            input[0] =
                (wchar_t)(0xD800 + (cp >> 10));

            input[1] =
                (wchar_t)(0xDC00 + (cp & 0x3FF));

            inputLength = 2;
        }

        wchar_t output[32];
        int outputLength = 0;

        if (!NormalizeCharacter(
            input,
            inputLength,
            output,
            outputLength,
            changed))
            return false;

        if (!WriteUtf8(dst, output, outputLength))
            return false;
    }
}

// ------------------------------------------------------------
// UTF-16 decoder
// ------------------------------------------------------------

static bool ReadUtf16Unit(
    BufferedReader& reader,
    bool bigEndian,
    unsigned short& value,
    bool& eof)
{
    unsigned char a = 0;

    if (!reader.ReadByte(a, eof))
        return false;

    if (eof)
        return true;

    unsigned char b = 0;
    bool secondEof = false;

    if (!reader.ReadByte(b, secondEof) || secondEof)
        return false;

    if (bigEndian)
        value = (unsigned short)(((unsigned short)a << 8) | b);
    else
        value = (unsigned short)(a | ((unsigned short)b << 8));

    return true;
}

// ------------------------------------------------------------
// UTF-16 encoder
// ------------------------------------------------------------

static bool WriteUtf16(
    BufferedWriter& writer,
    const wchar_t* text,
    int length,
    bool bigEndian)
{
    unsigned char bytes[2];

    for (int i = 0; i < length; ++i)
    {
        unsigned short c = (unsigned short)text[i];

        if (bigEndian)
        {
            bytes[0] = (unsigned char)(c >> 8);
            bytes[1] = (unsigned char)c;
        }
        else
        {
            bytes[0] = (unsigned char)c;
            bytes[1] = (unsigned char)(c >> 8);
        }

        if (!writer.Write(bytes, 2))
            return false;
    }

    return true;
}

// ------------------------------------------------------------
// UTF-16 processing
// ------------------------------------------------------------

static bool ProcessUtf16(
    FILE* src,
    BufferedWriter& dst,
    bool bigEndian,
    bool& changed)
{
    BufferedReader reader(src);

    for (;;)
    {
        unsigned short c = 0;
        bool eof = false;

        if (!ReadUtf16Unit(
            reader, bigEndian, c, eof))
            return false;

        if (eof)
            return dst.Flush();

        wchar_t input[2];
        int inputLength = 1;

        if (c >= 0xD800 && c <= 0xDBFF)
        {
            unsigned short low = 0;
            bool lowEof = false;

            if (!ReadUtf16Unit(
                reader, bigEndian, low, lowEof) ||
                lowEof ||
                low < 0xDC00 ||
                low > 0xDFFF)
                return false;

            input[0] = (wchar_t)c;
            input[1] = (wchar_t)low;
            inputLength = 2;
        }
        else if (c >= 0xDC00 && c <= 0xDFFF)
        {
            return false;
        }
        else
        {
            input[0] = (wchar_t)c;
        }

        wchar_t output[32];
        int outputLength = 0;

        if (!NormalizeCharacter(
            input,
            inputLength,
            output,
            outputLength,
            changed))
            return false;

        if (!WriteUtf16(
            dst,
            output,
            outputLength,
            bigEndian))
            return false;
    }
}

// ------------------------------------------------------------
// Main file processing
// ------------------------------------------------------------

static bool TransformFile(
    const wchar_t* source,
    const wchar_t* destination,
    bool& changed)
{
    FILE* src = nullptr;
    FILE* dst = nullptr;

    if (_wfopen_s(&src, source, L"rb") != 0 || !src)
        return false;

    if (_wfopen_s(&dst, destination, L"wb") != 0 || !dst)
    {
        fclose(src);
        return false;
    }

    bool success = false;
    changed = false;

    unsigned char bom[3] = { 0, 0, 0 };
    // size_t count = fread(bom, 1, 3, src);

    // if (ferror(src))
    //     goto cleanup;

    // bool utf8 = false;
    // bool utf16LE = false;
    // bool utf16BE = false;

    size_t count = fread(bom, 1, 3, src);

    bool utf8 = false;
    bool utf16LE = false;
    bool utf16BE = false;

    if (ferror(src))
        goto cleanup;

    {
        BufferedWriter writer(dst);

        if (count >= 3 &&
            bom[0] == 0xEF &&
            bom[1] == 0xBB &&
            bom[2] == 0xBF)
        {
            utf8 = true;

            if (!writer.Write(bom, 3))
                goto cleanup;
        }
        else if (count >= 2 &&
            bom[0] == 0xFF &&
            bom[1] == 0xFE)
        {
            utf16LE = true;

            if (!writer.Write(bom, 2))
                goto cleanup;

            if (fseek(src, 2, SEEK_SET) != 0)
                goto cleanup;
        }
        else if (count >= 2 &&
            bom[0] == 0xFE &&
            bom[1] == 0xFF)
        {
            utf16BE = true;

            if (!writer.Write(bom, 2))
                goto cleanup;

            if (fseek(src, 2, SEEK_SET) != 0)
                goto cleanup;
        }
        else
        {
            if (fseek(src, 0, SEEK_SET) != 0)
                goto cleanup;

            utf8 = true;
        }

        if (utf16LE)
            success = ProcessUtf16(src, writer, false, changed);
        else if (utf16BE)
            success = ProcessUtf16(src, writer, true, changed);
        else if (utf8)
            success = ProcessUtf8(src, writer, changed);

        if (success)
            success = writer.Flush();
    }

cleanup:

    if (ferror(src) || ferror(dst))
        success = false;

    if (fclose(src) != 0)
        success = false;

    if (fclose(dst) != 0)
        success = false;

    if (!success)
        DeleteFileW(destination);

    return success;
}

// ------------------------------------------------------------
// COM plugin implementation
// ------------------------------------------------------------

STDMETHODIMP CWinMergeScript::get_PluginEvent(BSTR* pVal)
{
    if (!pVal)
        return E_POINTER;

    *pVal = SysAllocString(L"FILE_PREDIFF");

    return *pVal ? S_OK : E_OUTOFMEMORY;
}

STDMETHODIMP CWinMergeScript::get_PluginDescription(BSTR* pVal)
{
    if (!pVal)
        return E_POINTER;

    *pVal = SysAllocString(
        L"Ignore Unicode accents, preserve case");

    return *pVal ? S_OK : E_OUTOFMEMORY;
}

STDMETHODIMP CWinMergeScript::get_PluginFileFilters(BSTR* pVal)
{
    if (!pVal)
        return E_POINTER;

    *pVal = SysAllocString(
        L"*.txt;*.csv;*.log;*.xml;*.json;*.ini;*.cfg;*.md;"
        L"*.html;*.htm;*.css;*.js;*.ts;*.cpp;*.h;*.cs;*.sql");

    return *pVal ? S_OK : E_OUTOFMEMORY;
}

STDMETHODIMP CWinMergeScript::get_PluginIsAutomatic(VARIANT_BOOL* pVal)
{
    if (!pVal)
        return E_POINTER;

    *pVal = VARIANT_FALSE;
    return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginArguments(BSTR* pVal)
{
    if (!pVal)
        return E_POINTER;

    *pVal = m_bstrArguments.Copy();

    return S_OK;
}

STDMETHODIMP CWinMergeScript::put_PluginArguments(BSTR val)
{
    m_bstrArguments = val;
    return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginExtendedProperties(BSTR* pVal)
{
    if (!pVal)
        return E_POINTER;

    *pVal = SysAllocString(L"MenuCaption=Ignore Accents");

    return *pVal ? S_OK : E_OUTOFMEMORY;
}

STDMETHODIMP CWinMergeScript::ShowSettingsDialog(VARIANT_BOOL* pbHandled)
{
    if (!pbHandled)
        return E_POINTER;

    *pbHandled = VARIANT_FALSE;
    return S_OK;
}

STDMETHODIMP CWinMergeScript::PluginOnEvent(
    int iEventType,
    IDispatch* pDispatch)
{
    m_pMergeApp = pDispatch;
    return S_OK;
}

STDMETHODIMP CWinMergeScript::PrediffFile(
    BSTR fileSrc,
    BSTR fileDst,
    VARIANT_BOOL* pbChanged,
    VARIANT_BOOL* pbSuccess)
{
    if (!fileSrc || !fileDst ||
        !pbChanged || !pbSuccess)
        return E_POINTER;

    *pbChanged = VARIANT_FALSE;
    *pbSuccess = VARIANT_FALSE;

    bool changed = false;

    bool success = TransformFile(
        fileSrc,
        fileDst,
        changed);

    if (!success)
        return S_OK;

    *pbChanged = changed
        ? VARIANT_TRUE
        : VARIANT_FALSE;

    *pbSuccess = VARIANT_TRUE;

    return S_OK;
}
