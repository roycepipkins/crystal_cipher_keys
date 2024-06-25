#ifndef CRYSTAL_HOLDER_IR_ITEMS
#define CRYSTAL_HOLDER_IR_ITEMS
#include <driver/rmt.h>

const int space_length = 500;
const int zero_length = 500;
const int one_length = 1000;
const int delimit_length = 2000;
const int tolerance_length = 250;

const rmt_item32_t zero_bit = {{{ zero_length, 1, space_length, 0 }}};
const rmt_item32_t one_bit = {{{ one_length, 1, space_length, 0 }}};
const rmt_item32_t delimit_item = {{{ delimit_length, 1, space_length, 0 }}};

enum item_type
{
    Zero,
    One,
    Delimit,
    Unknown
};

bool is_within_tolerance(const int value, const int desired_value, const int tolerance);

item_type getItemType(const rmt_item32_t& item);

#endif