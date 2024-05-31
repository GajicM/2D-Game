#ifndef RAINDROP_H_INCLUDED
#define RAINDROP_H_INCLUDED
#include <rafgl.h>
#include <stdlib.h>

#define NUM_RAINDROPS 2000

typedef struct {
  float x, y;
  float  yspeed;
  float ssize;
} Raindrop;

Raindrop raindrops[NUM_RAINDROPS];

void initRain(rafgl_raster_t *raster) {
  for (int i = 0; i < NUM_RAINDROPS; i++) {
    raindrops[i].x = rand() % raster->width;
    raindrops[i].y = rand() % raster->height;

    raindrops[i].yspeed = rand() % 100 / 100.0 + 0.4;
    raindrops[i].ssize = rand() % 100 / 100.0 + 0.1;
  }
}

void drawRain(rafgl_raster_t* raster) {
  for (int i = 0; i < NUM_RAINDROPS; i++) {
     rafgl_raster_draw_line(raster,raindrops[i].x,(raindrops[i].y),raindrops[i].x,(raindrops[i].y+(rand()%5+1)), rafgl_RGB(255, 255,  255));
  }

}

void updateRain(rafgl_raster_t *raster) {
  for (int i = 0; i < NUM_RAINDROPS; i++) {
    raindrops[i].y += raindrops[i].yspeed;
    if(raindrops[i].y>raster->height)
        raindrops[i].y=0;
  }
}

#endif // RAINDROP_H_INCLUDED
