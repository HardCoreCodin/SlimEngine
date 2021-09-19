#pragma once

void setString(String *string, char *char_ptr) {
    string->char_ptr = char_ptr;
    string->length = 0;
    if (char_ptr)
        while (char_ptr[string->length])
            string->length++;
}

u32 getStringLength(char *string) {
    char *char_ptr = string;
    u32 length = 0;
    if (char_ptr) while (char_ptr[length]) length++;
    return length;
}

u32 getDirectoryLength(char *path) {
    u32 path_len = getStringLength(path);
    u32 dir_len = path_len;
    while (path[dir_len] != '/' && path[dir_len] != '\\') dir_len--;
    return dir_len + 1;
}

void copyToString(String *string, char* char_ptr, u32 offset) {
    string->length = offset;
    char *source_char = char_ptr;
    char *string_char = string->char_ptr + offset;
    while (source_char[0]) {
        *string_char = *source_char;
        string_char++;
        source_char++;
        string->length++;
    }
    *string_char = 0;
}

void mergeString(String *string, char* first, char* second, u32 offset) {
    copyToString(string, first, 0);
    copyToString(string, second, offset);
}

void printNumberIntoString(i32 number, NumberString *number_string) {
    initNumberString(number_string);
    char *buffer = number_string->_buffer;
    buffer[12] = 0;

    bool is_negative = number < 0;
    if (is_negative) number = -number;

    if (number) {
        u32 temp;
        buffer += 11;
        number_string->string.char_ptr = buffer;
        number_string->string.length = 0;

        for (u8 i = 0; i < 11; i++) {
            temp = number;
            number /= 10;
            number_string->string.length++;
            *buffer-- = (char)('0' + temp - number * 10);
            if (!number) {
                if (is_negative) {
                    *buffer = '-';
                    number_string->string.char_ptr--;
                    number_string->string.length++;
                }

                break;
            }
            number_string->string.char_ptr--;
        }
    } else {
        buffer[11] = '0';
        number_string->string.length = 1;
        number_string->string.char_ptr = buffer + 11;
    }
}

void printFloatIntoString(f32 number, NumberString *number_string, u8 float_digits_count) {
    f32 factor = 1;
    for (u8 i = 0; i < float_digits_count; i++) factor *= 10;
    i32 int_num = (i32)(number * factor);
    if (int_num == 0) {
        printNumberIntoString((i32)factor, number_string);
        number_string->string.length++;
        number_string->string.char_ptr[0] = '.';
        number_string->string.char_ptr--;
        number_string->string.char_ptr[0] = '0';
        return;
    }

    bool is_negative = number < 0;
    bool is_fraction = is_negative ? number > -1 : number < 1;

    printNumberIntoString(int_num, number_string);

    if (is_fraction) {
        u32 len = number_string->string.length;
        number_string->string.length++;
        number_string->string.char_ptr--;
        if (is_negative) {
            number_string->string.char_ptr[0] = '-';
            number_string->string.char_ptr[1] = '0';
        } else {
            number_string->string.char_ptr[0] = '0';
        }
        if (len < float_digits_count) {
            for (u32 i = 0; i < (float_digits_count - len); i++) {
                number_string->string.length++;
                number_string->string.char_ptr--;
                number_string->string.char_ptr[0] = '0';
            }
        }
    }

    static char tmp[13];
    tmp[number_string->string.length + 1] = 0;
    for (u8 i = 0; i < (u8)number_string->string.length; i++) {
        u8 char_count_from_right_to_left = (u8)number_string->string.length - i - 1;
        if (char_count_from_right_to_left >= float_digits_count) tmp[i] = number_string->string.char_ptr[i];
        else                                                     tmp[i + 1] = number_string->string.char_ptr[i];
    }
    tmp[number_string->string.length - float_digits_count] = '.';
    copyToString(&number_string->string, tmp, 0);
    if (is_negative) number_string->string.length++;
}