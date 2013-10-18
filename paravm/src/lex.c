#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <glib.h>

#include "lex.h"
#include "opcode.h"

ParaVMToken *paravm_create_token(ParaVMTokenType type, char *value,
                                 uint32_t line, uint32_t column)
{
    assert(value);
    assert(line);

    ParaVMToken *tok = g_new(ParaVMToken, 1);

    tok->type = type;
    tok->value = g_strdup(value);
    tok->line = line;
    tok->column = column;

    return tok;
}

void paravm_destroy_token(ParaVMToken *tok)
{
    if (tok)
        g_free(tok->value);

    g_free(tok);
}

#define CHAR_LINE_FEED 0x0000000A
#define CHAR_BACKSLASH 0x0000005C
#define CHAR_APOSTROPHE 0x00000027
#define CHAR_QUOTE 0x00000022
#define CHAR_PAREN_OPEN 0x00000028
#define CHAR_PAREN_CLOSE 0x00000029
#define CHAR_BRACKET_OPEN 0x0000005B
#define CHAR_BRACKET_CLOSE 0x0000005D
#define CHAR_SEMICOLON 0x0000003B
#define CHAR_PERIOD 0x0000002E
#define CHAR_E_LOWER 0x00000065
#define CHAR_E_UPPER 0x00000045
#define CHAR_PLUS 0x0000002B
#define CHAR_MINUS 0x0000002D
#define CHAR_COLON 0x0000003A
#define CHAR_ONE 0x00000031
#define CHAR_ZERO 0x00000030

ParaVMError paravm_lex_string(const char *str, ParaVMToken ***tokens,
                              uint32_t *line, uint32_t *column)
{
    assert(str);
    assert(tokens);
    assert(line);
    assert(column);

    *tokens = null;
    *line = 1;
    *column = 0;

    __block const char *lstr = str;

    if (!g_utf8_validate(lstr, -1, null))
        return PARAVM_ERROR_BAD_UTF8;

    gunichar (^ peek_char)(void) = ^(void)
    {
        if (!*lstr)
            return UINT32_MAX;

        return g_utf8_get_char(lstr);
    };

    __block uint32_t lline = 1;
    __block uint32_t lcolumn = 0;

    gunichar (^ next_char)(void) = ^(void)
    {
        if (!*lstr)
            return UINT32_MAX;

        gunichar ch = g_utf8_get_char(lstr);
        lstr = g_utf8_next_char(lstr); // Note: This handles the null terminator properly.

        if (ch == CHAR_LINE_FEED)
        {
            lline++;
            lcolumn = 0;
        }
        else
            lcolumn++;

        return ch;
    };

    ParaVMError result = PARAVM_ERROR_OK;
    GPtrArray *arr = g_ptr_array_new();

    gunichar c;

    while ((c = next_char()) != UINT32_MAX)
    {
        // Skip all forms of white space.
        if (g_unichar_isspace(c))
            continue;

        // Skip comments starting with `;`.
        if (c == CHAR_SEMICOLON)
        {
            while (true)
            {
                c = peek_char();

                if (c == UINT32_MAX || c == CHAR_LINE_FEED)
                    break;

                next_char();
            }

            continue;
        }

        bool is_directive = false;

        ParaVMTokenType tt;
        GArray *str_arr = g_array_new(false, false, sizeof(gunichar));
        uint32_t tline = lline;
        uint32_t tcol = lcolumn;

        if (c != CHAR_APOSTROPHE && c != CHAR_QUOTE && c != CHAR_COLON)
            g_array_append_val(str_arr, c);

        switch (c)
        {
            case CHAR_APOSTROPHE:
            case CHAR_QUOTE:
                tt = c == CHAR_APOSTROPHE ? PARAVM_TOKEN_TYPE_ATOM : PARAVM_TOKEN_TYPE_STRING;

                gunichar term = c;

                while (true)
                {
                    c = peek_char();

                    if (c == UINT32_MAX)
                    {
                        result = PARAVM_ERROR_SYNTAX;
                        break;
                    }

                    // Handle escape sequences.
                    if (c == CHAR_BACKSLASH)
                    {
                        c = next_char();

                        gunichar c2 = peek_char();

                        switch (c2)
                        {
                            case CHAR_BACKSLASH:
                            case CHAR_APOSTROPHE:
                            case CHAR_QUOTE:
                                // Drop c, and only save c2.
                                c2 = next_char();
                                g_array_append_val(str_arr, c2);
                                break;
                            default:
                                g_array_append_val(str_arr, c);
                                break;
                        }
                    }
                    else if (c != term)
                    {
                        c = next_char();
                        g_array_append_val(str_arr, c);
                    }
                    else
                    {
                        next_char(); // Drop terminator character.
                        break;
                    }
                }

                break;
            case CHAR_COLON:
                tt = PARAVM_TOKEN_TYPE_BINARY;

                while (true)
                {
                    c = peek_char();

                    if (c == CHAR_ZERO || c == CHAR_ONE)
                    {
                        c = next_char();
                        g_array_append_val(str_arr, c);
                    }
                    else if (c == CHAR_COLON)
                    {
                        next_char(); // Drop terminator character.
                        break;
                    }
                    else
                    {
                        result = PARAVM_ERROR_SYNTAX;
                        break;
                    }
                }

                break;
            case CHAR_PAREN_OPEN:
                tt = PARAVM_TOKEN_TYPE_PAREN_OPEN;

                break;
            case CHAR_PAREN_CLOSE:
                tt = PARAVM_TOKEN_TYPE_PAREN_CLOSE;

                break;
            case CHAR_BRACKET_OPEN:
                tt = PARAVM_TOKEN_TYPE_BRACKET_OPEN;

                break;
            case CHAR_BRACKET_CLOSE:
                tt = PARAVM_TOKEN_TYPE_BRACKET_CLOSE;

                break;
            case CHAR_PERIOD:
                is_directive = true;

                while (true)
                {
                    c = peek_char();

                    if (g_unichar_isalpha(c))
                    {
                        c = next_char();
                        g_array_append_val(str_arr, c);
                    }
                    else
                        break;
                }

                break;
            default:
                if (g_unichar_isalpha(c))
                {
                    tt = PARAVM_TOKEN_TYPE_OPCODE;

                    while (true)
                    {
                        c = peek_char();

                        if (g_unichar_isalpha(c) || c == CHAR_PERIOD)
                        {
                            c = next_char();
                            g_array_append_val(str_arr, c);
                        }
                        else
                            break;
                    }
                }
                else if (g_unichar_isdigit(c))
                {
                    // Must be an integer so far.
                    tt = PARAVM_TOKEN_TYPE_INTEGER;

                    while (true)
                    {
                        c = peek_char();

                        if (g_unichar_isdigit(c))
                        {
                            c = next_char();
                            g_array_append_val(str_arr, c);
                            continue;
                        }

                        if (c == CHAR_PERIOD)
                        {
                            // Looks like it's actually a float.
                            tt = PARAVM_TOKEN_TYPE_FLOAT;

                            c = next_char();
                            g_array_append_val(str_arr, c);

                            bool have_exp = false;

                            while (true)
                            {
                                c = peek_char();

                                if (g_unichar_isdigit(c))
                                {
                                    c = next_char();
                                    g_array_append_val(str_arr, c);
                                }
                                else if (!have_exp && (c == CHAR_E_LOWER || c == CHAR_E_UPPER))
                                {
                                    have_exp = true;

                                    c = next_char();
                                    g_array_append_val(str_arr, c);

                                    c = peek_char();

                                    if (c == CHAR_PLUS || c == CHAR_MINUS)
                                    {
                                        c = next_char();
                                        g_array_append_val(str_arr, c);
                                    }

                                    while (true)
                                    {
                                        c = peek_char();

                                        if (g_unichar_isdigit(c))
                                        {
                                            c = next_char();
                                            g_array_append_val(str_arr, c);
                                        }
                                        else
                                            break;
                                    }
                                }
                                else
                                    break;
                            }
                        }

                        break;
                    }
                }
                else
                    result = PARAVM_ERROR_SYNTAX;

                break;
        }

        if (result == PARAVM_ERROR_OK)
        {
            gchar *ustr = g_ucs4_to_utf8((gunichar *)str_arr->data, str_arr->len, null, null, null);

            if (is_directive)
            {
                // Make sure it's actually a correct directive.
                if (!strcmp(ustr, ".fun"))
                    tt = PARAVM_TOKEN_TYPE_FUN;
                else if (!strcmp(ustr, ".arg"))
                    tt = PARAVM_TOKEN_TYPE_ARG;
                else if (!strcmp(ustr, ".reg"))
                    tt = PARAVM_TOKEN_TYPE_REG;
                else if (!strcmp(ustr, ".blk"))
                    tt = PARAVM_TOKEN_TYPE_BLK;
                else if (!strcmp(ustr, ".unw"))
                    tt = PARAVM_TOKEN_TYPE_UNW;
                else
                    result = PARAVM_ERROR_SYNTAX;
            }
            else if (tt == PARAVM_TOKEN_TYPE_OPCODE)
            {
                if (!paravm_get_opcode_by_name(ustr))
                    result = PARAVM_ERROR_SYNTAX;
            }
            else if (tt == PARAVM_TOKEN_TYPE_FLOAT)
            {
                errno = 0;
                strtod(ustr, null);

                if (errno)
                    result = PARAVM_ERROR_OVERFLOW;
            }

            // Double-check since the result may have changed due to
            // the directive and opcode checks above.
            if (result == PARAVM_ERROR_OK)
            {
                ParaVMToken *tok = paravm_create_token(tt, ustr, tline, tcol);
                g_ptr_array_add(arr, tok);

                g_free(ustr);
            }
        }

        g_array_free(str_arr, true);

        if (result != PARAVM_ERROR_OK)
        {
            // Try to give the actual location of the error.
            *line = tline;
            *column = tcol;

            break;
        }
    }

    if (result == PARAVM_ERROR_OK)
    {
        g_ptr_array_add(arr, null);

        *tokens = (ParaVMToken **)arr->pdata;
        *line = lline;
        *column = lcolumn;
    }

    g_ptr_array_free(arr, result != PARAVM_ERROR_OK);

    return result;
}
