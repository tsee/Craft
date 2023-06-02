#include "config.h"
#include "noise.h"
#include "world.h"
#include "item.h"

// base height of ground
#define GROUND_HEIGHT 64

static void maybe_add_deciduous_tree(world_func func, void *arg,
                                     int x, int z, int h, int dx, int dz) {

    // determine tree size based on some arbitrary distribution
    float rnd = simplex2(x*0.5, z*0.5, 2, 0.5, 2);
    int tree_size;
    if (rnd > 0.85) {
        tree_size = 5;
    } else if (rnd > 0.55) {
        tree_size = 4;
    } else {
        tree_size = 3;
    }

    // do we have enough space in this chunk to place a tree?
    if (dx - tree_size < 0 || dz - tree_size < 0 ||
        dx + tree_size >= CHUNK_SIZE || dz + tree_size >= CHUNK_SIZE)
    {
        return;
    }

    if (simplex2(x, z, 6, 0.5, 2) > 0.84) {
        const int half_width = tree_size - 1;
        // leaves
        for (int y = h + tree_size-1; y < h + tree_size*2; y++) {
            for (int ox = -half_width; ox <= half_width; ox++) {
                for (int oz = -half_width; oz <= half_width; oz++) {
                    int d = (ox * ox) + (oz * oz) +
                            (y - (h + tree_size)) * (y - (h + tree_size));
                    if (d < tree_size * tree_size - 4) {
                        func(x + ox, y, z + oz, LEAVES, arg);
                    }
                }
            }
        }
        // trunk
        for (int y = h; y < h + 2*tree_size - 1; y++) {
            func(x, y, z, WOOD, arg);
        }
    }
}

static void maybe_add_coniferous_tree(world_func func, void *arg,
                                      int x, int z, int h, int dx, int dz) {
    // determine tree size based on some arbitrary distribution
    float rnd = simplex2(x*0.5, z*0.5, 2, 0.5, 2);
    int tree_size;
    if (rnd > 0.85) {
        tree_size = 4;
    } else if (rnd > 0.55) {
        tree_size = 5;
    } else if (rnd > 0.45) {
        tree_size = 6;
    } else {
        tree_size = 7;
    }

    // do we have enough space in this chunk to place a tree?
    if (dx - tree_size < 0 || dz - tree_size < 0 ||
        dx + tree_size >= CHUNK_SIZE || dz + tree_size >= CHUNK_SIZE)
    {
        return;
    }

    if (simplex2(x, z, 6, 0.5, 2) > 0.84) {
        const int half_width = tree_size / 2;
        const int crown_height = tree_size + 1;
        const int crown_bottom = h + 2;
        const int crown_top = crown_bottom + crown_height;
        const int max_width = 2 * half_width - 2;

        // leaves
        for (int dy = 0; dy < crown_height; dy++) {
            const int y = crown_bottom + dy;
            float radius_sq = max_width * (1.-(float)dy/(float)crown_height);
            radius_sq = radius_sq * radius_sq + .3;

            for (int ox = -half_width; ox <= half_width; ox++) {
                for (int oz = -half_width; oz <= half_width; oz++) {
                    const int d_sq = (ox * ox) + (oz * oz);
                    if (d_sq <= radius_sq) {
                        func(x + ox, y, z + oz, LEAVES, arg);
                    }
                }
            }
        }
        func(x, crown_top-1, z, LEAVES, arg);

        // trunk
        for (int y = h; y < crown_top - 2; y++) {
            func(x, y, z, WOOD, arg);
        }
    }
}

// evaluate based on space available in CHUNK and simplex noise whether
// or not we can place a tree (of some type and size) here and then
// potentially do so.
static void maybe_add_tree(world_func func, void *arg,
                           int x, int z, int h, int dx, int dz) {
    if (!SHOW_TREES) return;

    if (simplex2(x*0.5, z*0.5, 2, 0.5, 2) >= 0.6) {
        maybe_add_deciduous_tree(func, arg, x, z, h, dx, dz);
    }
    else {
        maybe_add_coniferous_tree(func, arg, x, z, h, dx, dz);
    }
}

// generate the cloud layer
#define CLOUD_HEIGHT 120
#define CLOUD_THICKNESS 3
// Larger number == fewer clouds
#define CLOUD_DENSITY_SCALE 0.70

// believe it or not, this appears to be a very large fraction (like 50% at the
// time of writing) of CPU time, due to the very many simplex3 calls.
// At that time, the y range used was 64 to 71, and the > threshold was 0.75.
// Moving that to 69-71 while lowering the threshold to 0.70 (ie. fewer layers
// to sample, but lower threshold == more clouds in smaller number of layers)
// cuts the CPU time by over 60%.
static void add_clouds(world_func func, void *arg, int x, int z, int flag) {
    if (SHOW_CLOUDS) {
        for (int y = CLOUD_HEIGHT; y < CLOUD_HEIGHT+CLOUD_THICKNESS; y++) {
            if (simplex3(x*0.01, y*0.1, z*0.01, 8, 0.5, 2) > CLOUD_DENSITY_SCALE) {
                func(x, y, z, CLOUD * flag, arg);
            }
        }
    }
}

static void add_plants(world_func func, void *arg, int x, int z, int h, int flag) {
    if (SHOW_PLANTS) {
        // grass
        if (simplex2(-x * 0.1, z * 0.1, 4, 0.8, 2) > 0.6) {
            func(x, h, z, TALL_GRASS * flag, arg);
        }
        // flowers
        if (simplex2(x * 0.05, -z * 0.05, 4, 0.8, 2) > 0.7) {
            int w = FLOWERS_BEGIN + simplex2(x * 0.1, z * 0.1, 4, 0.8, 2)
                    * (FLOWERS_END-FLOWERS_BEGIN+1);
            func(x, h, z, w * flag, arg);
        }
    }
}


#define SUBTERRANEAN_MATERIAL CEMENT
#define SURFACE_MATERIAL SAND
#define MOUNTAIN_SURFACE_MATERIAL GRASS
#define MOUNTAIN_FILL_MATERIAL DIRT

// Offset/factor aren't 100% accurate descriptions since both still get multiplied
// by one of the two simplex2 outputs. TODO: Need to revisit.
// shift total height
#define MOUNTAIN_HEIGHT_OFFSET GROUND_HEIGHT-16
// vertical scale factor for simplex generated heights
#define MOUNTAIN_HEIGHT_FACTOR 16
#define MOUNTAIN_HEIGHT_FACTOR2 32

void biome0(int p, int q, int x, int z, int dx, int dz, int flag, world_func func, void *arg) {
    (void) p; (void) q; // unused

    float f = simplex2(x * 0.01, z * 0.01, 4, 0.5, 2);
    float g = simplex2(-x * 0.01, -z * 0.01, 2, 0.9, 2);
    int mh = g * MOUNTAIN_HEIGHT_FACTOR + MOUNTAIN_HEIGHT_FACTOR2;
    int h = f * mh + MOUNTAIN_HEIGHT_OFFSET;

    // This biome is comprised of a subterranean material (SUBTERRANEAN_MATERIAL)
    // up to just below the minimal ground height, then covered with a single layer
    // of surface material (SURFACE_MATERIAL) OR, based on simplex noise, mountains.
    // Mountains in this biome are comprised of mountain fill material
    // (MOUNTAIN_FILL_MATERIAL) up to one layer below their surface, followed by one
    // later of mountain surface material (MOUNTAIN_SURFACE_MATERIAL). Those would
    // most obviously be DIRT and GRASS respectively buy can be changed in the above
    // #defines.

    // set minimal ground height
    if (h <= GROUND_HEIGHT) {
        h = GROUND_HEIGHT;
    }

    // a column of terrain
    // use subterranean material up to one layer below GROUND_HEIGHT
    for (int y = 0; y < GROUND_HEIGHT-1; y++) {
        func(x, y, z, SUBTERRANEAN_MATERIAL * flag, arg);
    }

    // fill mountain with dirt
    for (int y = GROUND_HEIGHT-1; y < h-1; y++) {
        func(x, y, z, MOUNTAIN_FILL_MATERIAL * flag, arg);
    }

    // add final surface layer, either mountain or plain type
    int w = (h == GROUND_HEIGHT ? SURFACE_MATERIAL : MOUNTAIN_SURFACE_MATERIAL);
    func(x, h-1, z, w * flag, arg);

    // place plants on grass surfaces
    // as written, this means that if a different material is chosen for mountains/ground,
    // it changes whether or not plants are placed.
    // this makes sense because it adds a minor amount of "realism" -- plants wouldn't sit on
    // stone/gravel mountains if that's what we're generating.
    if (w == GRASS) {
        add_plants(func, arg, x, z, h, flag);
        maybe_add_tree(func, arg, x, z, h, dx, dz);
    } // end if w == GRASS

    add_clouds(func, arg, x, z, flag);
}


/*
void biome2(int p, int q, int x, int z, int dx, int dz, int flag, world_func func, void *arg) {
    float f = simplex2(x * 0.01, z * 0.01, 2, 0.5, 1);
    float g = simplex2(-x * 0.01, -z * 0.01, 1, 0.9, 1);
    int mh = g * MOUNTAIN_HEIGHT_FACTOR/4 + MOUNTAIN_HEIGHT_OFFSET;
    int h = f * mh;
    // w = block id
    int w = SAND; // mountains
    int t = GROUND_HEIGHT;
    if (h <= t) {
        h = t;
        w = SAND; // ground
    }

    // a column of terrain
    for (int y = 0; y < h; y++) {
        func(x, y, z, w * flag, arg);
    }

    // place plants on grass surfaces
    // as written, this means that if a different material is chosen for mountains/ground,
    // it changes whether or not plants are placed.
    // this makes sense because it adds a minor amount of "realism" -- plants wouldn't sit on
    // stone/gravel mountains if that's what we're generating.
    if (w == SAND) {
        // grass only
        if (simplex2(-x * 0.1, z * 0.1, 4, 0.8, 2) > 0.9) {
            func(x, h, z, TALL_GRASS * flag, arg);
        }
    }

    //add_clouds(func, arg, x, z, flag);
}
*/

/*
void biome1(int p, int q, int x, int z, int dx, int dz, int flag, world_func func, void *arg) {
    int lo = simplex2(x * 0.01, z * 0.01, 4, 0.5, 2) * 8 + 8;
    int hi = simplex2(-x * 0.01, -z * 0.01, 4, 0.5, 2) * 32 + 32;
    int lookup[] = {3, 6, 11, 12, 13};
    for (int y = 0; y < lo; y++) {
        func(x, y, z, 6 * flag, arg);
    }
    for (int y = lo; y < hi; y++) {
        int i = simplex3(-x * 0.01, -y * 0.01, -z * 0.01, 4, 0.5, 2) * 10;
        int w = lookup[i % 5];
        if (simplex3(x * 0.01, y * 0.01, z * 0.01, 4, 0.5, 2) > 0.5) {
            func(x, y, z, w * flag, arg);
        }
    }
    add_clouds(func, arg, x, z, flag);
}
*/





// Main terrain generation function
// Parameters:
// - p: chunk p location
// - q: chunk q location
// - func: function callback to modify blocks in the world (see world.h)
// - arg: last argument to be used for the function callback
void create_world(int p, int q, world_func func, void *arg) {
    // GUESS: the inclusion of the extra pad locations is for tree generation across chunk borders.
    int pad = 1;
    // Loop for each (x, z) location in chunk (p, q):
    for (int dx = -pad; dx < CHUNK_SIZE + pad; dx++) {
        for (int dz = -pad; dz < CHUNK_SIZE + pad; dz++) {
            // TODO don't fully grok the meaning of the negative flag. It gets used to swap
            // the w type parameter from positive to negative, but I don't yet understand the
            // significance of that. The condition appears to be looking for "are we horizontally
            // outside of this chunk".
            int flag = 1;
            if (dx < 0 || dz < 0 || dx >= CHUNK_SIZE || dz >= CHUNK_SIZE) {
                flag = -1;
            }
            int x = p * CHUNK_SIZE + dx; // convert p (chunk x) and dx to world x
            int z = q * CHUNK_SIZE + dz; // convert q (chunk z) and dz to world z

            //int i = simplex2(-x * 0.01, -z * 0.01, 8, 0.5, 2) * 2;
            //if (i == 0)
            biome0(p, q, x, z, dz, dz, flag, func, arg);
            //else biome2(p, q, x, z, dx, dz, flag, func, arg);
            //else biome1(p, q, x, z, dx, dz, flag, func, arg);
        }
    }
}

