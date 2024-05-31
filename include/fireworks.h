#ifndef FIREWORKS_H_INCLUDED
#define FIREWORKS_H_INCLUDED
#include <rafgl.h>
#include <stdlib.h>

#define NUM_FIREWORKS 10
#define NUM_PARTICLES 200

typedef struct {
  float x, y;
  float xspeed, yspeed;
  rafgl_pixel_rgb_t color;
  float ssize;
  int age;
  int lifespan;
} particle_t;

particle_t fireworks[NUM_FIREWORKS][NUM_PARTICLES];

void initFireworks(rafgl_raster_t* raster) {
  for (int i = 0; i < NUM_FIREWORKS; i++) {
    for (int j = 0; j < NUM_PARTICLES; j++) {
      fireworks[i][j].x = raster->width/10 * (rand()%11 );
      fireworks[i][j].y = raster->height/10 * (rand()%11 ) ;
      fireworks[i][j].xspeed = rand() % 100 / 100.0 - 0.3;
      fireworks[i][j].yspeed = rand() % 100 / 100.0 - 0.3;
        fireworks[i][j].color.rgba=rafgl_RGB(rand()%256,rand()%256,rand()%256);
      fireworks[i][j].ssize = rand() % 100 / 100.0 + 0.1;
      fireworks[i][j].age = 0;
      fireworks[i][j].lifespan = rand() % 500 + 500;
    }
  }
}

void drawFireworks(rafgl_raster_t* raster,float xrand) {

  for (int i = 0; i < NUM_FIREWORKS; i++) {
    for (int j = 0; j < NUM_PARTICLES; j++) {
      if (fireworks[i][j].age > 0) {
        rafgl_raster_draw_line(raster,fireworks[i][j].x,(fireworks[i][j].y),fireworks[i][j].x+fireworks[i][j].ssize,
                               (fireworks[i][j].y+fireworks[i][j].ssize), fireworks[i][j].color.rgba*(1+xrand));
      }
    }
  }
}

void updateFireworks(rafgl_raster_t* raster) {
  for (int i = 0; i < NUM_FIREWORKS; i++) {
    for (int j = 0; j < NUM_PARTICLES; j++) {
      if (fireworks[i][j].age>-1) {
            if(fireworks[i][j].x+fireworks[i][j].xspeed>raster->width)
                fireworks[i][j].xspeed=-fireworks[i][j].xspeed;
            if(fireworks[i][j].x+fireworks[i][j].xspeed<0)
                fireworks[i][j].xspeed=-fireworks[i][j].xspeed;
            if(fireworks[i][j].y+fireworks[i][j].yspeed>raster->height)
                fireworks[i][j].yspeed=-fireworks[i][j].yspeed;
            if(fireworks[i][j].y+fireworks[i][j].yspeed<0)
                fireworks[i][j].yspeed=-fireworks[i][j].yspeed;


        fireworks[i][j].x += fireworks[i][j].xspeed;
        fireworks[i][j].y += fireworks[i][j].yspeed;
        fireworks[i][j].age++;
        if (fireworks[i][j].age > fireworks[i][j].lifespan) {
          fireworks[i][j].age = -1;
        }
      }
    }
  }
}



#endif // FIREWORKS_H_INCLUDED
