/*
** $Id: textops.c 9925 2008-03-25 02:14:01Z malin $
**
** textops.c: The text operations based on logical font.
**
** Copyright (C) 2005 ~ 2007 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2005/05/17
*/

#include <string.h>
#include <platform/yl_sys.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>

/************************* Text parse support ********************************/
int GUIAPI GetTextMCharInfo (PLOGFONT log_font, const char* mstr, int len,
                int* pos_chars)
{
    DEVFONT* sbc_devfont = log_font->sbc_devfont;
    DEVFONT* mbc_devfont = log_font->mbc_devfont;
    int count = 0;
    int left_bytes = len;
    int len_cur_char;

    while (left_bytes > 0) {
        if (pos_chars)
            pos_chars [count] = len - left_bytes;

        if (mbc_devfont) {
            len_cur_char = (*mbc_devfont->charset_ops->len_first_char)
                    ((const unsigned char*)mstr, left_bytes);
            if (len_cur_char > 0) {
                count ++;
                left_bytes -= len_cur_char;
                mstr += len_cur_char;
                continue;
            }
        }

        len_cur_char = (*sbc_devfont->charset_ops->len_first_char)
                ((const unsigned char*)mstr, left_bytes);
        if (len_cur_char > 0) {
            count ++;
            left_bytes -= len_cur_char;
            mstr += len_cur_char;
        }
    }

    return count;
}

int GUIAPI GetTextWordInfo (PLOGFONT log_font, const char* mstr, int len,
                int* pos_words, WORDINFO* info_words)
{
    DEVFONT* sbc_devfont = log_font->sbc_devfont;
    DEVFONT* mbc_devfont = log_font->mbc_devfont;
    WORDINFO word_info;
    int count = 0;
    int left_bytes = len;
    int mbc_sub_len, sbc_sub_len, word_len;

    while (left_bytes > 0) {
        sbc_sub_len = left_bytes;

        if (mbc_devfont) {
            int mbc_pos;

            mbc_pos = (*mbc_devfont->charset_ops->pos_first_char)
                    ((const unsigned char*)mstr, left_bytes);
            if (mbc_pos == 0) {
                mbc_sub_len = (*mbc_devfont->charset_ops->len_first_substr)
                        ((const unsigned char*)mstr, left_bytes);

                while (mbc_sub_len) {
                    (*mbc_devfont->charset_ops->get_next_word)
                            ((const unsigned char*)mstr, mbc_sub_len,
                             &word_info);

                    if (pos_words)
                        pos_words [count] = len - left_bytes;
                    if (info_words)
                        memcpy (info_words + count, &word_info,
                                        sizeof (WORDINFO));

                    count ++;
                    word_len = word_info.len + word_info.nr_delimiters;
                    mbc_sub_len -= word_len;
                    left_bytes -= word_len;
                    mstr += word_len;
                }

                continue;
            }
            else if (mbc_pos > 0)
                sbc_sub_len = mbc_pos;
        }

        while (sbc_sub_len) {
            (*sbc_devfont->charset_ops->get_next_word)
                    ((const unsigned char*)mstr, sbc_sub_len, &word_info);
            if (pos_words)
                pos_words [count] = len - left_bytes;
            if (info_words)
                memcpy (info_words + count, &word_info, sizeof (WORDINFO));

            count ++;
            word_len = word_info.len + word_info.nr_delimiters;
            sbc_sub_len -= word_len;
            left_bytes -= word_len;
            mstr += word_len;
        }
    }

    return count;
}

int GUIAPI GetFirstMCharLen (PLOGFONT log_font, const char* mstr, int len)
{
    DEVFONT* sbc_devfont = log_font->sbc_devfont;
    DEVFONT* mbc_devfont = log_font->mbc_devfont;
    int len_cur_char = 0;

    if (mbc_devfont) {
        len_cur_char = (*mbc_devfont->charset_ops->len_first_char)
                ((const unsigned char*)mstr, len);
        if (len_cur_char > 0)
            return len_cur_char;
    }

    len_cur_char = (*sbc_devfont->charset_ops->len_first_char)
            ((const unsigned char*)mstr, len);
    if (len_cur_char > 0)
        return len_cur_char;

    return len_cur_char;
}

int GUIAPI GetLastMCharLen (PLOGFONT log_font, const char* mstr, int len)
{
    DEVFONT* sbc_devfont = log_font->sbc_devfont;
    DEVFONT* mbc_devfont = log_font->mbc_devfont;
    int left_bytes = len;
    int len_cur_char;
    int lastlen = 0;

    while (left_bytes > 0) {
        if (mbc_devfont) {
            len_cur_char = (*mbc_devfont->charset_ops->len_first_char)
                    ((const unsigned char*)mstr, left_bytes);
            if (len_cur_char > 0) {
                left_bytes -= len_cur_char;
                mstr += len_cur_char;
                lastlen = len_cur_char;
                continue;
            }
        }

        len_cur_char = (*sbc_devfont->charset_ops->len_first_char)
                ((const unsigned char*)mstr, left_bytes);
        if (len_cur_char > 0) {
            left_bytes -= len_cur_char;
            mstr += len_cur_char;
            lastlen = len_cur_char;
        }
    }

    return lastlen;
}

int GUIAPI GetFirstWord (PLOGFONT log_font, const char* mstr, int len,
                    WORDINFO* word_info)
{
    DEVFONT* sbc_devfont = log_font->sbc_devfont;
    DEVFONT* mbc_devfont = log_font->mbc_devfont;

    if (mbc_devfont) {
        int mbc_pos;

        mbc_pos = (*mbc_devfont->charset_ops->pos_first_char)
                ((const unsigned char*)mstr, len);
        if (mbc_pos == 0) {
            len = (*mbc_devfont->charset_ops->len_first_substr)
                    ((const unsigned char*)mstr, len);

            (*mbc_devfont->charset_ops->get_next_word)
                    ((const unsigned char*)mstr, len, word_info);
            return word_info->len + word_info->nr_delimiters;
        }
        else if (mbc_pos > 0)
            len = mbc_pos;
    }

    (*sbc_devfont->charset_ops->get_next_word)
            ((const unsigned char*)mstr, len, word_info);
    return word_info->len + word_info->nr_delimiters;
}

#ifdef _UNICODE_SUPPORT

int GUIAPI MB2WCEx (PLOGFONT log_font, void* dest, BOOL wc32,
                const unsigned char* mchar, int n)
{
    DEVFONT* mbc_devfont = log_font->mbc_devfont;
    DEVFONT* sbc_devfont = log_font->sbc_devfont;
    DEVFONT* devfont;
    int len_cur_char = 0;

    if (mbc_devfont) {
        len_cur_char = mbc_devfont->charset_ops->len_first_char (mchar, n);
    }

    if (len_cur_char == 0) {
        devfont = sbc_devfont;
        len_cur_char = sbc_devfont->charset_ops->len_first_char (mchar, n);
    }
    else
        devfont = mbc_devfont;

    if (len_cur_char == 0)
        return 0;
    else {
        if (dest) {
            UChar32 uc32 = devfont->charset_ops->conv_to_uc32 (mchar);
            if (wc32) {
                UChar32* uchar32 = (UChar32*)dest;
                *uchar32 = uc32;
            }
            else {
                UChar16* uchar16 = (UChar16*)dest;
                if (uc32 > 0xFFFF)
                    *uchar16 = 0xFEFF;
                else
                    *uchar16 = (UChar16)uc32;
            }
        }
        return len_cur_char;
    }

    return -1;
}

int GUIAPI WC2MBEx (PLOGFONT log_font, unsigned char *s, UChar32 wc)
{
    DEVFONT* mbc_devfont = log_font->mbc_devfont;
    DEVFONT* sbc_devfont = log_font->sbc_devfont;
    unsigned char mchar [16];
    int len_cur_char = 0;

    if (s == NULL || wc == 0)
        return 0;

    if (mbc_devfont) {
        len_cur_char = mbc_devfont->charset_ops->conv_from_uc32 (wc, mchar);
    }

    if (len_cur_char <= 0)
        len_cur_char = sbc_devfont->charset_ops->conv_from_uc32 (wc, mchar);

    if (len_cur_char <= 0)
        return -1;

    if (s)
        memcpy (s, mchar, len_cur_char);

    return len_cur_char;
}

int GUIAPI MBS2WCSEx (PLOGFONT log_font, void* dest, BOOL wc32,
                const unsigned char* mstr, int mstr_len, int n,
                int* conved_mstr_len)
{
    DEVFONT* mbc_devfont = log_font->mbc_devfont;
    DEVFONT* sbc_devfont = log_font->sbc_devfont;
    DEVFONT* devfont;
    int left, count = 0;
    UChar32* uchar32;
    UChar16* uchar16;

    if (wc32) {
        uchar16 = NULL;
        uchar32 = (UChar32*)dest;
    }
    else {
        uchar16 = (UChar16*)dest;
        uchar32 = NULL;
    }

    if (mstr == NULL)
        return -1;

    if (mstr_len < 0)
        mstr_len = strlen ((char*)mstr) + 1;
    left = mstr_len;

    while (left > 0 && *mstr) {

        int len_cur_char = 0;

        if (mbc_devfont) {
            len_cur_char = mbc_devfont->charset_ops->len_first_char
                    (mstr, left);
            if (len_cur_char > 0) {
                devfont = mbc_devfont;
            }
            else if (left > 0 && count > 0 && *mstr > 0x7F) {
                break;
            }
        }

        if (len_cur_char == 0) {
            devfont = sbc_devfont;
            len_cur_char = sbc_devfont->charset_ops->len_first_char
                    (mstr, left);
        }
        else
            devfont = mbc_devfont;

        if (len_cur_char == 0) {
            break;
        }
        else {
            UChar32 uc32 = devfont->charset_ops->conv_to_uc32 (mstr);

            if (uchar32)
                uchar32 [count] = uc32;
            else if (uchar16) {
                if (uc32 > 0xFFFF)
                    uchar16 [count] = 0xFEFF;
                else
                    uchar16 [count] = (UChar16)uc32;
            }

            count++;
        }

        if (count == n) {
            break;
        }

        mstr += len_cur_char;
        left -= len_cur_char;
    }

    if (*mstr == '\0') {
        if (dest && count < n) {
            if (uchar32)
                uchar32 [count] = 0;
            else if (uchar16)
                uchar16 [count] = 0;
        }
        left --;
    }

    if (conved_mstr_len)
        *conved_mstr_len = mstr_len - left;
    return count;
}

int GUIAPI WCS2MBSEx (PLOGFONT log_font, unsigned char* dest,
                const void *wcs, int wcs_len, BOOL wc32, int n,
                int* conved_wcs_len)
{
    DEVFONT* mbc_devfont = log_font->mbc_devfont;
    DEVFONT* sbc_devfont = log_font->sbc_devfont;
    int wc_count = 0, count = 0;
    unsigned char mchar [16] = {0};
    const UChar32* uchar32;
    const UChar16* uchar16;
    UChar32 uc32;

    if (wcs == NULL)
        return -1;

    if (wc32) {
        uchar16 = NULL;
        uchar32 = (UChar32*)wcs;
    }
    else {
        uchar16 = (UChar16*)wcs;
        uchar32 = NULL;
    }

    if (wcs_len < 0) {
        wcs_len = 0;
        if (uchar16) {
            while (uchar16 [wcs_len])
                wcs_len++;
        }
        else {
            while (uchar32 [wcs_len])
                wcs_len++;
        }
        wcs_len++;
    }

    while (wc_count < wcs_len) {
        int len_cur_char = 0;

        if (uchar16)
            uc32 = *uchar16;
        else
            uc32 = *uchar32;

        if (uc32 == 0)
            break;

        if (mbc_devfont) {
            len_cur_char = mbc_devfont->charset_ops->conv_from_uc32
                    (uc32, mchar);
       }

        if (len_cur_char <= 0) {
            len_cur_char = sbc_devfont->charset_ops->conv_from_uc32
                (uc32, mchar);
        }

        if (len_cur_char <= 0) {
            if (conved_wcs_len)
                *conved_wcs_len = wc_count;

            return -1;
        }

        if (count + len_cur_char > n)
            break;

        if (dest) {
            memcpy (dest, mchar, len_cur_char);
            dest += len_cur_char;
        }

        count += len_cur_char;
        wc_count++;

        if (uchar16)
            uchar16++;
        else
            uchar32++;
    }

    if (uchar16)
        uc32 = *uchar16;
    else
        uc32 = *uchar32;

    if (uc32 == 0) {
        if (dest && count < n)
            *dest = '\0';
        wc_count ++;
    }

    if (conved_wcs_len)
        *conved_wcs_len = wc_count;

    return count;
}

size_t __mg_strlen (PLOGFONT log_font, const char* mstr)
{
    DEVFONT* mbc_devfont = log_font->mbc_devfont;

    if (mbc_devfont && strstr (mbc_devfont->charset_ops->name, "UTF-16")) {
        size_t len = 0;

        while (mstr [len] || mstr [len + 1]) {
            len += 2;
        }

        return len;
    }
    else {
        return strlen (mstr);
    }
}

char* __mg_strnchr (PLOGFONT log_font, const char* s, size_t n, int c,
                int* chlen)
{
    size_t i;
    unsigned char c1, c2;
    DEVFONT* mbc_devfont = log_font->mbc_devfont;

    if (mbc_devfont) {
        *chlen = 2;
        if (strstr (mbc_devfont->charset_ops->name, "UTF-16LE")) {
            c1 = c;
            c2 = '\0';
        }
        else if (strstr (mbc_devfont->charset_ops->name, "UTF-16BE")) {
            c1 = '\0';
            c2 = c;
        }
        else {
            char* str = strnchr (s, n, c);
            if (str)
                *chlen = 1;
            else
                *chlen = 0;
            return str;
        }

        for (i=0; i<n; i+=2) {
            if (s[0] == c1 && s[1] == c2)
                return (char *)s;

            s += 2;
        }
    }

    return NULL;
}

int __mg_substrlen (PLOGFONT log_font, const char* text, int len,
                int delimiter, int* nr_delim)
{
    char* substr;

    *nr_delim = 0;
    if ((substr = __mg_strnchr (log_font, text,
                    len, delimiter, nr_delim)) == NULL)
        return len;

    len = substr - text;
    if (*nr_delim == 2) {
        DEVFONT* mbc_devfont = log_font->mbc_devfont;
        unsigned char c1, c2;

        if (strstr (mbc_devfont->charset_ops->name, "UTF-16LE")) {
            c1 = delimiter;
            c2 = '\0';
        }
        else {
            c1 = '\0';
            c2 = delimiter;
        }

        *nr_delim = 0;
        while (substr[0] == c1 && substr [1] == c2) {
            (*nr_delim) += 2;
            substr += 2;
        }
    }
    else {
        *nr_delim = 0;
        while (*substr == delimiter) {
            (*nr_delim) ++;
            substr ++;
        }
    }

    return len;
}

#endif /* _UNICODE_SUPPORT */

