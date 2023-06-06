#ifndef _config_h_
#define _config_h_

// app parameters
#define DEBUG 0
#define FULLSCREEN 0
#define WINDOW_WIDTH 1920 // resolution to use for windowed mode
#define WINDOW_HEIGHT 1080
#define FULLSCREEN_WIDTH 1920 // resolution for fullscreen. Set to 0 for auto detect.
#define FULLSCREEN_HEIGHT 1080
#define VSYNC 1
#define SCROLL_THRESHOLD 0.1
#define MAX_MESSAGES 4
#define DB_PATH "craft.db"
#define USE_CACHE 1
#define DAY_LENGTH 600

// control/movement parameters
#define INVERT_MOUSE 0
#define MOUSE_SENSITIVITY 0.0025
#define BASE_SPEED 5.0f // base movement speed
#define BASE_FLYING_SPEED 20.0f // base movement speed while flying
#define RUNNING_SPEED_MODIFIER 3.0f // speedup factor hen running enabled

// rendering options
#define SHOW_LIGHTS 1
#define SHOW_PLANTS 1
#define SHOW_CLOUDS 1
#define SHOW_TREES 1
#define SHOW_ITEM 1
#define SHOW_CROSSHAIRS 1
#define SHOW_WIREFRAME 1
#define SHOW_INFO_TEXT 1
#define SHOW_CHAT_TEXT 1
#define SHOW_PLAYER_NAMES 1
#define PLAYER_NAME_DISTANCE 96 // Maximum distance to display a player's name at
#define USE_24H_CLOCK 1 // 0 = use 12h (am/pm) clock, 1 = use 24h clock
#define FOV_DEFAULT 65 // default, non-zoom FOV
#define FOV_ZOOM 15 // FOV when it zoom mode


// key bindings
//#define CRAFT_KEY_FORWARD 'W'
#define CRAFT_KEY_FORWARD 'W'
#define CRAFT_KEY_BACKWARD 'S'
#define CRAFT_KEY_LEFT 'A'
#define CRAFT_KEY_RIGHT 'D'
#define CRAFT_KEY_JUMP GLFW_KEY_SPACE
#define CRAFT_KEY_FLY GLFW_KEY_TAB
#define CRAFT_KEY_OBSERVE 'O'
#define CRAFT_KEY_OBSERVE_INSET 'P'
#define CRAFT_KEY_ITEM_NEXT 'E'
#define CRAFT_KEY_ITEM_PREV 'R'
#define CRAFT_KEY_ZOOM GLFW_KEY_LEFT_SHIFT
#define CRAFT_KEY_RUN GLFW_KEY_LEFT_CONTROL
#define CRAFT_KEY_ORTHO 'F'
#define CRAFT_KEY_CHAT 't'
#define CRAFT_KEY_COMMAND '/'
#define CRAFT_KEY_SIGN '`'
#define CRAFT_KEY_FULLSCREEN GLFW_KEY_F11

// advanced parameters
#define CREATE_CHUNK_RADIUS 10
#define RENDER_CHUNK_RADIUS 10
#define RENDER_SIGN_RADIUS 4
#define DELETE_CHUNK_RADIUS 14
#define CHUNK_SIZE 32
// database commit interval in seconds
#define COMMIT_INTERVAL 5

#endif
