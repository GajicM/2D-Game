#ifndef GHOST_SPRITE_H_INCLUDED
#define GHOST_SPRITE_H_INCLUDED
#include <rafgl.h>
#define MAZE_SIZE 33
#define WORLD_SIZE 45
#define NUMBER_OF_TILES 6
#define TILE_SIZE 80
struct ghost{
rafgl_spritesheet_t sprite;
int pos_x;
int pos_y;
int speed;
int spawn_x[4];
int spawn_y[4];
};

void init_ghost(struct ghost* ghost1){
ghost1->pos_x=0;
ghost1->pos_y=3300;
ghost1->speed=250;
ghost1->spawn_x[0]=0;
ghost1->spawn_x[1]=0;
ghost1->spawn_x[2]=TILE_SIZE*WORLD_SIZE;
ghost1->spawn_x[3]=TILE_SIZE*WORLD_SIZE;

ghost1->spawn_y[0]=0;
ghost1->spawn_y[1]=TILE_SIZE*WORLD_SIZE;
ghost1->spawn_y[2]=0;
ghost1->spawn_y[3]=TILE_SIZE*WORLD_SIZE;
}
#endif // GHOST_SPRITE_H_INCLUDED
