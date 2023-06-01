#ifndef _world_h_
#define _world_h_

// World function callback (apparently used to modify a world's blocks)
// x/y/z = coordinates of block
// w = block type
// arg = passthrough parameter as provided to create_world()
typedef void (*world_func)(int x, int y, int z, int w, void *arg);

void create_world(int p, int q, world_func func, void *arg);

#endif
