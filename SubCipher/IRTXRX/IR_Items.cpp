#include "IR_Items.h"

bool is_within_tolerance(const int value, const int desired_value, const int tolerance)
{
    return (value < (desired_value + tolerance) && (value > (desired_value - tolerance)));
}

item_type getItemType(const rmt_item32_t& item)
{
    if (item.level0 != 1 && item.level1 != 0) return item_type::Unknown;
    //Final Delimit has zero-duration level 0 part becuase it times out or whatever
    //So we'll accept a Delimit without testing 2nd part
    if (is_within_tolerance(item.duration0, delimit_length, tolerance_length)) return item_type::Delimit;
    if (!is_within_tolerance(item.duration1, space_length, tolerance_length)) return item_type::Unknown;
    if (is_within_tolerance(item.duration0, zero_length, tolerance_length)) return item_type::Zero;
    if (is_within_tolerance(item.duration0, one_length, tolerance_length)) return item_type::One;
    
    return item_type::Unknown;
}