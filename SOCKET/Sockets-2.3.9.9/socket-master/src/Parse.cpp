#include <cstdlib>
#include <cstring>

#include "Parse.h"

namespace dai {

Parse::Parse():
    pa_the_str(""),
    pa_splits(""),
    pa_ord(""),
    pa_the_ptr(0),
    pa_breakchar(0),
    pa_enable(0),
    pa_disable(0),
    pa_nospace(0),
    pa_quote(false)
{
}

Parse::Parse(const std::string& s):
    pa_the_str(s),
    pa_splits(""),
    pa_ord(""),
    pa_the_ptr(0),
    pa_breakchar(0),
    pa_enable(0),
    pa_disable(0),
    pa_nospace(0),
    pa_quote(false)
{
}

Parse::Parse(const std::string& s, const std::string& sp):
    pa_the_str(s),
    pa_splits(sp),
    pa_ord(""),
    pa_the_ptr(0),
    pa_breakchar(0),
    pa_enable(0),
    pa_disable(0),
    pa_nospace(0),
    pa_quote(false)
{
}

Parse::Parse(const std::string& s, const std::string& sp, short nospace):
    pa_the_str(s),
    pa_splits(sp),
    pa_ord(""),
    pa_the_ptr(0),
    pa_breakchar(0),
    pa_enable(0),
    pa_disable(0),
    pa_nospace(1),
    pa_quote(false)
{
}

Parse::~Parse()
{
}

#define C ((pa_the_ptr<pa_the_str.size()) ? pa_the_str[pa_the_ptr] : 0)

short Parse::issplit(const char c)
{
    for (char pa_split : pa_splits)
        if (pa_split == c)
            return 1;
    return 0;
}

void Parse::getsplit()
{
    size_t x;
    if (C == '=')
    {
        x = pa_the_ptr++;
    }
    else
    {
        while (C && (issplit(C)))
            pa_the_ptr++;
        x = pa_the_ptr;
        while (C && !issplit(C) && C != '=')
            pa_the_ptr++;
    }
    if (x == pa_the_ptr && C == '=')
        pa_the_ptr++;
    pa_ord = (x < pa_the_str.size()) ? pa_the_str.substr(x, pa_the_ptr - x) : "";
}

std::string Parse::getword()
{
    size_t x;
    int    disabled = 0;
    int    quote    = 0;
    int    rem      = 0;

    if (pa_nospace)
    {
        while (C && issplit(C))
            pa_the_ptr++;
        x = pa_the_ptr;
        while (C && !issplit(C) && (C != pa_breakchar || !pa_breakchar || disabled))
        {
            if (pa_breakchar && C == pa_disable)
                disabled = 1;
            if (pa_breakchar && C == pa_enable)
                disabled = 0;
            if (pa_quote && C == '"')
                quote = 1;
            pa_the_ptr++;
            while (quote && C && C != '"')
            {
                pa_the_ptr++;
            }
            if (pa_quote && C == '"')
            {
                pa_the_ptr++;
            }
            quote = 0;
        }
    }
    else
    {
        if (C == pa_breakchar && pa_breakchar)
        {
            x = pa_the_ptr++;
            rem = 1;
        }
        else
        {
            while (C && (C == ' ' || C == 9 || C == 13 || C == 10 || issplit(C)))
                pa_the_ptr++;
            x = pa_the_ptr;
            while (C && C != ' ' && C != 9 && C != 13 && C != 10 && !issplit(C) &&
                   (C != pa_breakchar || !pa_breakchar || disabled))
            {
                if (pa_breakchar && C == pa_disable)
                    disabled = 1;
                if (pa_breakchar && C == pa_enable)
                    disabled = 0;
                if (pa_quote && C == '"')
                {
                    quote = 1;
                    pa_the_ptr++;
                    while (quote && C && C != '"')
                    {
                        pa_the_ptr++;
                    }
                    if (pa_quote && C == '"')
                    {
                        pa_the_ptr++;
                    }
                }
                else
                    pa_the_ptr++;
                quote = 0;
            }
            pa_the_ptr++;
            rem = 1;
        }
        if (x == pa_the_ptr && C == pa_breakchar && pa_breakchar)
            pa_the_ptr++;
    }
    if (x < pa_the_str.size())
    {
        pa_ord = pa_the_str.substr(x, pa_the_ptr - x - rem);
    }
    else
    {
        pa_ord = "";
    }
    return pa_ord;
}

void Parse::getword(std::string& s)
{
    s = Parse::getword();
}

void Parse::getsplit(std::string& s)
{
    Parse::getsplit();
    s = pa_ord;
}

void Parse::getword(std::string& s, std::string& fill, int l)
{
    Parse::getword();
    s = "";
    while (s.size() + pa_ord.size() < (size_t) l)
        s += fill;
    s += pa_ord;
}

std::string Parse::getrest()
{
    std::string s;
    while (C && (C == ' ' || C == 9 || issplit(C)))
        pa_the_ptr++;
    s = (pa_the_ptr < pa_the_str.size()) ? pa_the_str.substr(pa_the_ptr) : "";
    return s;
}

void Parse::getrest(std::string& s)
{
    while (C && (C == ' ' || C == 9 || issplit(C)))
        pa_the_ptr++;
    s = (pa_the_ptr < pa_the_str.size()) ? pa_the_str.substr(pa_the_ptr) : "";
}

long Parse::getvalue()
{
    Parse::getword();
    return atol(pa_ord.c_str());
}

void Parse::setbreak(const char c)
{
    pa_breakchar = c;
}

int Parse::getwordlen()
{
    size_t x, y = pa_the_ptr, len;

    if (C == pa_breakchar && pa_breakchar)
    {
        x = pa_the_ptr++;
    }
    else
    {
        while (C && (C == ' ' || C == 9 || C == 13 || C == 10 || issplit(C)))
            pa_the_ptr++;
        x = pa_the_ptr;
        while (C && C != ' ' && C != 9 && C != 13 && C != 10 && !issplit(C) && (C != pa_breakchar || !pa_breakchar))
            pa_the_ptr++;
    }
    if (x == pa_the_ptr && C == pa_breakchar && pa_breakchar)
        pa_the_ptr++;
    len = pa_the_ptr - x;
    pa_the_ptr = y;
    return (int) len;
}

int Parse::getrestlen()
{
    size_t y = pa_the_ptr;
    size_t len;

    while (C && (C == ' ' || C == 9 || issplit(C)))
        pa_the_ptr++;
    len = strlen(pa_the_str.c_str() + pa_the_ptr);
    pa_the_ptr = y;
    return (int) len;
}

void Parse::getline()
{
    size_t x;

    x = pa_the_ptr;
    while (C && C != 13 && C != 10)
        pa_the_ptr++;
    pa_ord = (x < pa_the_str.size()) ? pa_the_str.substr(x, pa_the_ptr - x) : "";
    if (C == 13)
        pa_the_ptr++;
    if (C == 10)
        pa_the_ptr++;
}

void Parse::getline(std::string& s)
{
    getline();
    s = pa_ord;
}

}//namespace dai


