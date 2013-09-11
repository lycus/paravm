#include <string.h>

#include <glib-2.0/glib.h>

#include "lexer.h"

#define CHAR_LINE_FEED 0x0000000A
#define CHAR_SLASH 0x0000002F
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

ParaVMError paravm_lex_string(const char *str, ParaVMToken **tokens,
                              uint32_t *line, uint32_t *column)
{
    assert(str);
    assert(tokens);
    assert(line);
    assert(column);

    __block const char *lstr = str;

    *tokens = null;
    *line = 0;
    *column = 0;

    if (!g_utf8_validate(lstr, -1, null))
        return PARAVM_ERROR_BAD_UTF8;

    uint32_t (^ peek_char)(void) = ^(void)
    {
        if (!*lstr)
            return UINT32_MAX;

        return g_utf8_get_char(lstr);
    };

    __block uint32_t lline;
    __block uint32_t lcolumn;

    uint32_t (^ next_char)(void) = ^(void)
    {
        if (!*lstr)
            return UINT32_MAX;

        uint32_t ch = g_utf8_get_char(lstr);
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
    GArray *arr = g_array_new(false, false, sizeof(ParaVMToken));

    uint32_t c;

    while ((c = next_char()) != UINT32_MAX)
    {
        // Skip all forms of white space.
        if (g_unichar_isspace(c))
            continue;

        bool is_instruction = false;
        bool is_directive = false;

        ParaVMTokenType tt;
        GArray *str_arr = g_array_new(false, false, sizeof(uint32_t));
        uint32_t tline = *line;
        uint32_t tcol = *column;

        if (c != CHAR_APOSTROPHE && c != CHAR_QUOTE)
            g_array_append_val(str_arr, c);

        switch (c)
        {
            case CHAR_SLASH:
                tt = PARAVM_TOKEN_TYPE_COMMENT;

                while (true)
                {
                    c = peek_char();

                    if (c == UINT32_MAX || c == CHAR_LINE_FEED)
                        break;

                    c = next_char();
                    g_array_append_val(str_arr, c);
                }

                break;
            case CHAR_APOSTROPHE:
            case CHAR_QUOTE:
                tt = PARAVM_TOKEN_TYPE_STRING;

                uint32_t term = c;

                while (true)
                {
                    c = peek_char();

                    if (c == UINT32_MAX)
                    {
                        result = PARAVM_ERROR_SYNTAX;
                        break;
                    }

                    if (c != term)
                    {
                        c = next_char();
                        g_array_append_val(str_arr, c);
                    }
                    else
                        break;
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
            case CHAR_SEMICOLON:
                tt = PARAVM_TOKEN_TYPE_SEMICOLON;

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
                    is_instruction = true;
                    tt = PARAVM_TOKEN_TYPE_INSTRUCTION;

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

                            bool have_exp;

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
            const char *ustr = g_ucs4_to_utf8((uint32_t *)str_arr->data, str_arr->len, null, null, null);

            if (is_directive)
            {
                // Make sure it's actually a correct directive.
                if (!strcmp(ustr, ".mod"))
                    tt = PARAVM_TOKEN_TYPE_MOD;
                else if (!strcmp(ustr, ".glb"))
                    tt = PARAVM_TOKEN_TYPE_GLB;
                else if (!strcmp(ustr, ".loc"))
                    tt = PARAVM_TOKEN_TYPE_LOC;
                else if (!strcmp(ustr, ".fun"))
                    tt = PARAVM_TOKEN_TYPE_FUN;
                else if (!strcmp(ustr, ".arg"))
                    tt = PARAVM_TOKEN_TYPE_ARG;
                else if (!strcmp(ustr, ".reg"))
                    tt = PARAVM_TOKEN_TYPE_REG;
                else if (!strcmp(ustr, ".blk"))
                    tt = PARAVM_TOKEN_TYPE_BLK;
                else
                    result = PARAVM_ERROR_SYNTAX;
            }
            else if (is_instruction)
            {
                // It might be a true/false literal.
                if (!strcmp(ustr, "true"))
                    tt = PARAVM_TOKEN_TYPE_TRUE;
                else if (!strcmp(ustr, "false"))
                    tt = PARAVM_TOKEN_TYPE_FALSE;
                else
                {
                    /* TODO Check instruction against table. */
                }
            }

            // Double-check since the result may have changed due to
            // the directive and instruction checks above.
            if (result == PARAVM_ERROR_OK)
            {
                ParaVMToken tok = (ParaVMToken) { tt, ustr, tline, tcol };
                g_array_append_val(arr, tok);
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
        ParaVMToken dummy = (ParaVMToken) { 0, null, 0, 0 };
        g_array_append_val(arr, dummy);

        *tokens = (ParaVMToken *)arr->data;
    }

    g_array_free(arr, result != PARAVM_ERROR_OK);

    *line = lline;
    *column = lcolumn;

    return result;
}
