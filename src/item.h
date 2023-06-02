#ifndef _item_h_
#define _item_h_

#include <stddef.h>
#include <stdint.h>

typedef int blk_type_t;

// numeric range of flower type blocks - needs to be contiguous
#define FLOWERS_BEGIN YELLOW_FLOWER
#define FLOWERS_END BLUE_FLOWER

#define EMPTY 0
#define GRASS 1
#define SAND 2
#define STONE 3
#define BRICK 4
#define WOOD 5
#define CEMENT 6
#define DIRT 7
#define PLANK 8
#define SNOW 9
#define GLASS 10
#define COBBLE 11
#define LIGHT_STONE 12
#define DARK_STONE 13
#define CHEST 14
#define LEAVES 15
#define CLOUD 16
#define TALL_GRASS 17
// Flowers begin (see FLOWERS_BEGIN and FLOWERS_END)
#define YELLOW_FLOWER 18
#define RED_FLOWER 19
#define PURPLE_FLOWER 20
#define SUN_FLOWER 21
#define WHITE_FLOWER 22
#define BLUE_FLOWER 23
// Flowers end
#define COLOR_00 32
#define COLOR_01 33
#define COLOR_02 34
#define COLOR_03 35
#define COLOR_04 36
#define COLOR_05 37
#define COLOR_06 38
#define COLOR_07 39
#define COLOR_08 40
#define COLOR_09 41
#define COLOR_10 42
#define COLOR_11 43
#define COLOR_12 44
#define COLOR_13 45
#define COLOR_14 46
#define COLOR_15 47
#define COLOR_16 48
#define COLOR_17 49
#define COLOR_18 50
#define COLOR_19 51
#define COLOR_20 52
#define COLOR_21 53
#define COLOR_22 54
#define COLOR_23 55
#define COLOR_24 56
#define COLOR_25 57
#define COLOR_26 58
#define COLOR_27 59
#define COLOR_28 60
#define COLOR_29 61
#define COLOR_30 62
#define COLOR_31 63
#define LANTERN 64

#define HARDCODED_ITEM_LIST_SIZE 256

extern const blk_type_t items[];
extern const size_t item_count;
extern const int blocks[HARDCODED_ITEM_LIST_SIZE][6];
extern const int plants[HARDCODED_ITEM_LIST_SIZE];

int is_plant(blk_type_t w);
int is_obstacle(blk_type_t w);
int is_transparent(blk_type_t w);
int is_destructable(blk_type_t w);
int is_lantern(blk_type_t w);

#endif
