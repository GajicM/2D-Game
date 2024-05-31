#include <main_state.h>
#include <glad/glad.h>
#include <math.h>
#include <maze.h>
#include <rafgl.h>
#include <game_constants.h>
#include <raindrop.h>
#include <fireworks.h>
struct ghost{
    rafgl_spritesheet_t sprite;
    int pos_x;
    int pos_y;
    int speed;
    int spawn_x[4];
    int spawn_y[4];
};

static rafgl_raster_t raster, raster2,raster3;
static rafgl_raster_t heart;
int mazeMatrix[2000];
static rafgl_texture_t texture;
static rafgl_spritesheet_t hero;
#define MAZE_SIZE 33
#define NUMBER_OF_TILES 8

rafgl_raster_t tiles[NUMBER_OF_TILES];
#define WORLD_SIZE 45
int tile_world[WORLD_SIZE][WORLD_SIZE];

#define TILE_SIZE 80

static int raster_width = RASTER_WIDTH, raster_height = RASTER_HEIGHT;
void paint();
void update_alive(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args);
void init_map();
void set_ghost_spawn();




//kamera x i kamera y je postavljeno na 600,600 na pocetku
int camx=0, camy=WORLD_SIZE*(TILE_SIZE-3);
int hero_health=3;
int selected_x, selected_y;
int hero_pos_y=0;
int port_pos_x=800;
int port_pos_y=1600;
struct ghost ghost;
static char save_file[256];
int save_file_no = 0;


void main_state_init(GLFWwindow *window, void *args, int width, int height)
{
    /* inicijalizacija */
    /* raster init nam nije potreban ako radimo load from image */

    raster_width = width;
    raster_height = height;
    camy-=raster_height;
    rafgl_raster_init(&raster, raster_width, raster_height);
    rafgl_raster_init(&raster2, raster_width, raster_height);
    rafgl_raster_init(&raster3, raster_width, raster_height);

    GenerateMaze(&mazeMatrix,MAZE_SIZE,MAZE_SIZE);
    ShowMaze(mazeMatrix,MAZE_SIZE,MAZE_SIZE);



    rafgl_spritesheet_init(&hero, "res/images/character.png", 10, 4);
    rafgl_spritesheet_init(&ghost.sprite, "res/images/ghost.png", 1, 1);

    init_map(mazeMatrix,MAZE_SIZE,MAZE_SIZE);

    char tile_path[256];

    for(int i = 0; i < NUMBER_OF_TILES; i++){
        sprintf(tile_path, "res/maze-tiles/set%d.png", i+1);
        rafgl_raster_load_from_image(&tiles[i], tile_path);
    }

    ///setovanje duha
    ghost.pos_x=TILE_SIZE*WORLD_SIZE;
    ghost.pos_y=0;
    ghost.speed=200;
    set_ghost_spawn(&ghost);

//setovanje spawn heroja
    hero_pos_y =raster_height-2*TILE_SIZE;

    rafgl_texture_init(&texture);
    rafgl_raster_load_from_image(&heart,"res/images/heart.png");
    initRain(&raster);
    initFireworks(&raster);
}

int pressed;
float location = 0;
float selector = 0;
int animation_running = 0;
int animation_frame = 0;
int direction = 0;
int hero_pos_x =0;
int hero_speed = 155;
int speed=0;
int hover_frames = 0;

void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args)
{
  /*  if(speed>0){
    draw_endscreen(game_data);
    } else {
    speed=50;
    update_alive(window,delta_time,game_data,args);
    }*/

    if(hero_health>0&&(camy+hero_pos_y)<=(4*TILE_SIZE) && (camx+hero_pos_x)>=(TILE_SIZE*(WORLD_SIZE-12))){
        draw_winscreen(game_data);}
    else if(hero_health>0){
        update_alive(window,delta_time,game_data,args);

    }
    else{
         draw_endscreen(game_data);
    }
 /* */
}


void main_state_render(GLFWwindow *window, void *args)
{
    /* prikazi teksturu */
    rafgl_texture_load_from_raster(&texture, &raster);
    rafgl_texture_show(&texture, 0);
}


void main_state_cleanup(GLFWwindow *window, void *args)
{
    rafgl_raster_cleanup(&raster);
    rafgl_raster_cleanup(&raster2);
    rafgl_texture_cleanup(&texture);

}

#define NUMBER_OF_TORCHES 5
int torch_pos_x[ NUMBER_OF_TORCHES];
int torch_pos_y[ NUMBER_OF_TORCHES];

void init_map(const char *maze, int width, int height) {
    int x, y;
    int flag=1;
    int pots=9;
    int pots1=4;
    int hole=1;
    int torches=NUMBER_OF_TORCHES;
    int waittimepots=0;
    int waittimetorches=0;
    //hardkodovano za kraj
    for(y=0;y<5;y++){
        for(x=0;x<width;x++){
            if(y==0)
                tile_world[y][x]=1;
            if(x==0||x==width-1)
                tile_world[y][x]=1;
            if(x==width-1 && y==3)
                tile_world[y][x]=0;
        }
    }


    for(y = 0; y < height; y++) {
        for(x = 0; x < width; x++) {
            switch(maze[y * width + x]) {
                case 1: tile_world[y+5][x]=1;  break;
                case 2: tile_world[y+5][x]=0;  break;
                default: tile_world[y+5][x]=0;  break;
            }
            if (tile_world[y+5][x]==0 && pots>=0 && rand()%4==1 && waittimepots<1 ){
                tile_world[y+5][x]=2;
                pots--;
                waittimepots=50;

            }else if(tile_world[y+5][x]==0 && pots1>=0 && rand()%4==1 && waittimepots<1){
                tile_world[y+5][x]=3;
                pots1--;
                waittimepots=50;

            }
            else  if(tile_world[y+5][x]==0 &&  rand()%150==1 && flag>0){
                printf("%d %d port\n",x,y+5);
                port_pos_x=x*TILE_SIZE+40;
                port_pos_y=(y+5)*TILE_SIZE+40;
                flag=-1;
            }else if (tile_world[y+5][x]==1 && torches>=0 && rand()%5==1 && waittimetorches<1 ){
                tile_world[y+5][x]=4;
                torch_pos_x[ NUMBER_OF_TORCHES-torches]=x*TILE_SIZE+40;
                torch_pos_y[NUMBER_OF_TORCHES-torches]=(y+5)*TILE_SIZE+40;
                torches--;

                waittimetorches=120;
            }else if(tile_world[y+5][x]==0 && hole>0 && rand()%250==1){
                tile_world[y+5][x]=5; printf("%d %d\n",x,y+5);
                hole--;}
            else if(tile_world[y+5][x]==0 && rand()%25==0){
                tile_world[y+5][x]=rand()%2+6;


            }
            else {waittimetorches--;waittimepots--;}
        }
    }
    //hardkodovano za pocetak
    for(y=height+5;y<height+10;y++){
        for(x=0;x<width;x++){
            if(y==height+9)
                tile_world[y][x]=1;
            if(x==0||x==width-1)
                tile_world[y][x]=1;
            if(x==0 && y==height+8)
                tile_world[y][x]=0;
        }
    }




}


void render_tilemap(rafgl_raster_t *raster)
{

    int x, y;
    int x0 = camx / TILE_SIZE;

    int x1 = x0 + (raster_width / TILE_SIZE) + 1;
    int y0 = camy / TILE_SIZE;
    int y1 = y0 + (raster_height / TILE_SIZE) + 2;

    //klempujemo
    if(x0 < 0) x0 = 0;
    if(y0 < 0) y0 = 0;
    if(x1 < 0) x1 = 0;
    if(y1 < 0) y1 = 0;

    //klempujemo
    if(x0 >= WORLD_SIZE) x0 = WORLD_SIZE - 1;
    if(y0 >= WORLD_SIZE) y0 = WORLD_SIZE - 1;
    if(x1 >= WORLD_SIZE) x1 = WORLD_SIZE - 1;
    if(y1 >= WORLD_SIZE) y1 = WORLD_SIZE - 1;

    rafgl_raster_t *draw_tile;
    for(y = y0; y <= y1; y++)
    {
        for(x = x0; x <= x1; x++)
        {

            draw_tile = tiles + (tile_world[y][x] % NUMBER_OF_TILES);
            rafgl_raster_draw_raster(raster, draw_tile, x * TILE_SIZE - camx, y * TILE_SIZE - camy - draw_tile->height + TILE_SIZE);

        }
    }

}

int hit=0;
/**paintovanje pozadine i crnila*/
void paint(){
    int x,y;

    //koordinate centra kruga
    int cx = hero_pos_x+32, cy= hero_pos_y+32;
    //poluprecnik kruga
    int cr = 100;
    //pomocna promenljiva, za racunanje distance od centra do trenutnog piksela
    float dist;

    //boja kruga i boja pozadine
    rafgl_pixel_rgb_t  background;
    if(hit>0)
    background.rgba = rafgl_RGB(205, 0, 0);
    else
    background.rgba = rafgl_RGB(0, 0, 0);




    for(y=0; y<raster_height; y++){
        for(x=0;x<raster_width;x++){
            dist = rafgl_distance2D(cx,cy,x,y);
            if(dist < cr ){
                pixel_at_m(raster, x, y) = rafgl_lerppix(pixel_at_m(raster,x,y), background, dist/cr);
                 pixel_at_m(raster3, x, y) = rafgl_lerppix(pixel_at_m(raster,x,y), background, dist/cr);
            }else {

                pixel_at_m(raster, x, y) = background;
                 pixel_at_m(raster3, x, y) = rafgl_lerppix(pixel_at_m(raster,x,y), background, dist/cr);

            }


        }
    }

}

/**paintovanje torcheva sa interpolacijom*/
void paint2(){

    for(int i=0;i<NUMBER_OF_TORCHES;i++){

        if(abs(torch_pos_x[i]-camx)<raster_width&& abs(torch_pos_y[i]-camy)<raster_height){

            int  x0=abs(torch_pos_x[i]-camx);
            int y0=abs(torch_pos_y[i]-camy);
            if(x0<240)x0=240;
            if (y0<240)y0=240;

            for(int y=y0-160;y<y0+160;y++){
                if(y/raster_height>0)break;
                for(int x=x0-240;x<x0+240;x++){
                    if(x/raster_width>0)break;
                    float     dist=  rafgl_distance2D(torch_pos_x[i]-camx, torch_pos_y[i]-camy,x,y);

                    if(dist<100){
                        pixel_at_m(raster, x, y) = rafgl_lerppix(pixel_at_m(raster2,x,y), pixel_at_m(raster,x,y), dist/100);
                   ///otkomentarisi ovo ako endscreen vidi i torches
                   //      pixel_at_m(raster3, x, y) = rafgl_lerppix(pixel_at_m(raster2,x,y), pixel_at_m(raster,x,y), dist/100);

                    }
                }
            }
        }
    }

}


void set_ghost_spawn(struct ghost ghost1){
    ghost1.spawn_x[0]=0;
    ghost1.spawn_x[1]=0;
    ghost1.spawn_x[2]=TILE_SIZE*WORLD_SIZE;
    ghost1.spawn_x[3]=TILE_SIZE*WORLD_SIZE;

    ghost1.spawn_y[0]=0;
    ghost1.spawn_y[1]=TILE_SIZE*WORLD_SIZE;
    ghost1.spawn_y[2]=0;
    ghost1.spawn_y[3]=TILE_SIZE*WORLD_SIZE;
}


void paint_health(){
    int x,y;
    rafgl_pixel_rgb_t pink,test;
    pink.rgba=rafgl_RGB(255,0,254);
    for(y=0;y<32;y++){
        for(x=0;x<hero_health*32;x++){
            test =pixel_at_m(heart,x%32,y%32);
            if(test.rgba!=pink.rgba)
                pixel_at_m(raster, x, y) = pixel_at_m(heart,x%32,y%32);
        }
    }


}



void  update_alive(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args){

    if(game_data->is_lmb_down && game_data->is_rmb_down)
    {
        pressed = 1;
        location = rafgl_clampf(game_data->mouse_pos_y, 0, raster_height - 1);
        selector = 1.0f * location / raster_height;
    }
    else
    {
        pressed = 0;
    }
    animation_running = 1;
    int curx=camx+hero_pos_x;
    int cury=camy+hero_pos_y;
    //  printf("%d %d %d %d\n",tile_world[cury/64][curx/64],curx/64,cury/64, cury);
    if(game_data->keys_down[RAFGL_KEY_W]  )
    {
        direction = 2;
        int flag=1;
        ///seemingly random brojevi +15/+45, ali da ne bude previse strogo uzima centralnih 30p i njih gleda
        for(int x=curx+15;x<curx+45;x++){
            if(tile_world[cury/TILE_SIZE][x/TILE_SIZE]==1 || tile_world[cury/TILE_SIZE][x/TILE_SIZE]==4){
                flag=0;
                break;
            }
        }


        if(flag)
            hero_pos_y = hero_pos_y - hero_speed * delta_time+1;
        ///ZASTO +1? dobro pitanje , nemam ideju iskreno


    }

    else if(game_data->keys_down[RAFGL_KEY_S])
    {
        direction = 0;
        int flag=1;

        ///seemingly random brojevi +15/+45, ali da ne bude previse strogo uzima centralnih 30p i njih gleda
        for(int x=curx+15;x<curx+45;x++){
            if(tile_world[(cury+64)/TILE_SIZE][x/TILE_SIZE]==1 ||tile_world[(cury+64)/TILE_SIZE][x/TILE_SIZE]==4){
                flag=0;
                break;
            }
        }


        if(flag)
            hero_pos_y = hero_pos_y + hero_speed * delta_time;

    }

    else if(game_data->keys_down[RAFGL_KEY_A])
    {
        ///ZASTO +1? dobro pitanje , nemam ideju iskreno

        int flag=1;
        for(int y=cury+10;y<cury+50;y++){
            if(tile_world[y/TILE_SIZE][(curx)/TILE_SIZE]==1||tile_world[y/TILE_SIZE][(curx)/TILE_SIZE]==4){
                flag=0;
                break;
            }
        }
        if(flag)
            hero_pos_x = hero_pos_x - hero_speed * delta_time +1;
        direction = 1;
    }

    else if(game_data->keys_down[RAFGL_KEY_D])
    {
        direction = 3;
        int flag=1;
        for(int y=cury+10;y<cury+50;y++){
            if(tile_world[y/TILE_SIZE][(curx+64)/TILE_SIZE]==1 ||tile_world[y/TILE_SIZE][(curx+64)/TILE_SIZE]==4){
                flag=0;
                break;
            }
        }
        if(flag)
            hero_pos_x = hero_pos_x + hero_speed * delta_time;

    }
    else
    {
        animation_running = 0;
    }

    if(animation_running)
    {
        if(hover_frames == 0)
        {

            animation_frame = (animation_frame + 1) % 10;

            hover_frames = 3;
        }
        else
        {

            hover_frames--;
        }

    }
    ///SKUPLJANJE ITEMA

    if(game_data->keys_down[RAFGL_KEY_SPACE] && (tile_world[(camy+hero_pos_y+32)/TILE_SIZE][(camx+hero_pos_x+32)/TILE_SIZE]==3)){
        tile_world[(camy+hero_pos_y+32)/TILE_SIZE][(camx+hero_pos_x+32)/TILE_SIZE]=0;
        hero_speed=250;
        speed=120;
    }else if(game_data->keys_down[RAFGL_KEY_SPACE] &&  tile_world[(camy+hero_pos_y+32)/TILE_SIZE][(camx+hero_pos_x+32)/TILE_SIZE]==2){
        tile_world[(camy+hero_pos_y+32)/TILE_SIZE][(camx+hero_pos_x+32)/TILE_SIZE]=0;
        hero_health++;
    }
    if(speed==0){
        hero_speed=155;
    }else
        speed--;

    ///PORT
    if(tile_world[(camy+hero_pos_y+32)/TILE_SIZE][(camx+hero_pos_x+32)/TILE_SIZE]==5){
        tile_world[(camy+hero_pos_y+32)/TILE_SIZE][(camx+hero_pos_x+32)/TILE_SIZE]=0;
        printf("%d %d\n",port_pos_x,port_pos_y);
        camx=port_pos_x-raster_width/2;
        camy=port_pos_y-raster_height/2;
        hero_pos_y =raster_height/2;
        hero_pos_x =raster_width/2;
        hero_speed=300;
        speed=250;
    }

    ///POMERANJE KAMERE
    if(hero_pos_x <= raster_width / 3)
    {
        if(!(camx<2)){
            if(hero_speed>125)
                camx-=1;
            hero_pos_x=(raster_width/3)+1;
            camx -=2;
        }
    }

    if(hero_pos_x >= raster_width - raster_width / 3 )
    {
        if(!(camx+2>(MAZE_SIZE)*TILE_SIZE-raster_width)){
            hero_pos_x=(raster_width - raster_width / 3 )-1;
            if(hero_speed>125)
                camx+=1;
            camx +=2;
        }
    }


    if(hero_pos_y <= raster_height / 3)
    {
        if(!(camy<2)){
            hero_pos_y=(raster_height/3)+1;
            if(hero_speed>125)
                camy-=1;
            camy -=3;
        }
    }

    if(hero_pos_y >  raster_height - raster_height / 3)
    {
        if(!(camy+2>(WORLD_SIZE-2)*TILE_SIZE-raster_height)){
            camy +=3;
            hero_pos_y=(raster_height - raster_height / 3) -1;
            if(hero_speed>125)
                camy+=1;

        }
    }


     if (game_data->mouse_pos_x > raster_width -raster_width/10)
         camx+=10;
     if (game_data->mouse_pos_x < raster_width/10)
         camx-=10;
    if (game_data->mouse_pos_y > raster_height -raster_height/10)
         camy+=10;
     if (game_data->mouse_pos_y < raster_height/10)
         camy-=10;
     /*  */

    render_tilemap(&raster);
    render_tilemap(&raster2);


    if(game_data->keys_pressed[RAFGL_KEY_S] && game_data->keys_down[RAFGL_KEY_LEFT_SHIFT])
    {
        sprintf(save_file, "save%d.png", save_file_no++);
        rafgl_raster_save_to_png(&raster, save_file);
    }

    if(hit>0){
        hit--;
    }

    paint();
    paint2();

    rafgl_raster_draw_spritesheet(&raster, &hero, animation_frame, direction, hero_pos_x, hero_pos_y);

///GHOST MOVEMENT +GHOST RESET

    if(ghost.pos_x<(camx+hero_pos_x)){
        ghost.pos_x+=ghost.speed*delta_time;

    }
    if(ghost.pos_x>(camx+hero_pos_x)){
        ghost.pos_x-=ghost.speed*delta_time-1;
    }
    if(ghost.pos_y<(camy+hero_pos_y)){

        ghost.pos_y+=ghost.speed*delta_time;}

    if(ghost.pos_y>(camy+hero_pos_y)){

        ghost.pos_y-=ghost.speed*delta_time-1;}

    if((ghost.pos_x-camx)<raster_width && (ghost.pos_y-camy)<raster_height){
        rafgl_raster_draw_spritesheet(&raster, &ghost.sprite, 0, 0, ghost.pos_x-camx, ghost.pos_y-camy);
        ghost.speed=70;

        float dist=rafgl_distance2D(ghost.pos_x+40,ghost.pos_y+40,(hero_pos_x+camx)+32,(hero_pos_y+camy)+32);//centar sprite-ova
        if(dist<40){
            hit=5;
            hero_health--;
            float max=-1;
            int sp=-1;
            for(int i=0;i<3;i++){
                float dist1=rafgl_distance2D(hero_pos_x+camx,hero_pos_y+camy+32,ghost.spawn_x[i],ghost.spawn_y[i]);//centar sprite-ova
                if(abs(dist1)>max){
                    max=abs(dist1);
                    sp=i;
                }
            }
            if(sp>=0){
                ghost.pos_x=ghost.spawn_x[sp];
                ghost.pos_y=ghost.spawn_y[sp];
                ghost.speed=200;
                printf("%d %d\n",ghost.pos_x,ghost.pos_y);
            }
        }
    }
    paint_health();





}

int xftime=0;
void draw_endscreen(rafgl_game_data_t *game_data){
if(ftime==0){
    xftime=1;
    initRain(&raster);
}
 int x0,y0;
    x0=raster_width/4;
    y0=raster_height/4;

    rafgl_pixel_rgb_t background;
     rafgl_pixel_rgb_t sampled;
    rafgl_pixel_rgb_t result;
    background.rgba=rafgl_RGB(0,0,0);
   // rafgl_raster_draw_raster(&raster, rst, x0, y0);
    for(int y=0;y<raster_height;y++){
     for(int x=0;x<raster_width;x++){
      //  pixel_at_m(raster,x,y)=background;
        int brightness;
        sampled = pixel_at_m(raster3,x,y);
        brightness = (sampled.r + sampled.g + sampled.b)/3;

            result.r = brightness;
            result.g = brightness;
            result.b = brightness;

            pixel_at_m(raster,x,y) = result;
        }
    }



    drawRain(&raster);
    updateRain(&raster);

    rafgl_button_t button;
    rafgl_button_innit(&button,x0+195,y0+125,150,30,rafgl_RGB(80,80,80));
   // rafgl_button_show(&raster,&button);

    rafgl_raster_draw_string(&raster,"You failed",x0+115,y0+40,rafgl_RGB(255,0,0),640);
    rafgl_raster_draw_string(&raster,"Try Again",x0+125,y0+100,rafgl_RGB(255,0,0),640);
    int tryagain=rafgl_button_check(&button,game_data);
    if(tryagain!=0){
        camx=0, camy=WORLD_SIZE*(TILE_SIZE-3)-raster_height;
        ghost.pos_x=3600;
        ghost.pos_y=0;
        GenerateMaze(&mazeMatrix,MAZE_SIZE,MAZE_SIZE);
        ShowMaze(mazeMatrix,MAZE_SIZE,MAZE_SIZE);
        init_map(mazeMatrix,MAZE_SIZE,MAZE_SIZE);
        hero_health=3;
        hero_pos_x=0;
        hero_pos_y =raster_height-2*TILE_SIZE;
        xftime=0;
    }
    rafgl_button_t button1;
    rafgl_button_innit(&button1,x0+200,y0+185,80,30,rafgl_RGB(80,80,80));
  //  rafgl_button_show(&raster,&button1);
    rafgl_raster_draw_string(&raster,"Quit?",x0+155,y0+160,rafgl_RGB(255,0,0),640);
    int quit=rafgl_button_check(&button1,game_data);
    if(quit!=0){
        exit(0);
    }



}
int xtime=5; int passed_time=0; float xrand=0.8f; int firsttime=0;
void draw_winscreen(rafgl_game_data_t *game_data){
if(firsttime==0){
    firsttime=1;
    initFireworks(&raster);
}
 int x0,y0;
    x0=raster_width/4;
    y0=raster_height/4;
    if(passed_time==xtime){

         xrand-=0.01f;
        passed_time=0;
         }
         else
            passed_time++;

            if(xrand<0)
                xrand=0;
    rafgl_raster_draw_spritesheet(&raster2, &hero, animation_frame, direction, hero_pos_x, hero_pos_y);

     rafgl_pixel_rgb_t sampled;
    rafgl_pixel_rgb_t result;

    for(int y=0;y<raster_height;y++){
     for(int x=0;x<raster_width;x++){
        sampled = pixel_at_m(raster2,x,y);
            result.r = sampled.r*xrand;
            result.g = sampled.g*xrand;
            result.b = sampled.b*xrand;
            pixel_at_m(raster,x,y) = result;
        }
    }

    drawFireworks(&raster,xrand);
    updateFireworks(&raster);
    rafgl_button_t button;
    rafgl_raster_draw_string(&raster,"YOU WIN!!", x0+140,y0+40,rafgl_RGB(255,255,255),640);


    rafgl_button_innit(&button,x0+195,y0+125,210,30,rafgl_RGB(80,80,80));
  //  rafgl_button_show(&raster,&button);
    rafgl_raster_draw_string(&raster,"Play Again?",x0+125,y0+100,rafgl_RGB(255,255,255),640);
    int tryagain=rafgl_button_check(&button,game_data);
    if(tryagain!=0){
        camx=0, camy=WORLD_SIZE*(TILE_SIZE-3)-raster_height;
        ghost.pos_x=3600;
        ghost.pos_y=0;
        GenerateMaze(&mazeMatrix,MAZE_SIZE,MAZE_SIZE);
        ShowMaze(mazeMatrix,MAZE_SIZE,MAZE_SIZE);
        init_map(mazeMatrix,MAZE_SIZE,MAZE_SIZE);
        hero_health=3;
        hero_pos_x=0;
        hero_pos_y =raster_height-2*TILE_SIZE;
        firsttime=0;
    }
    rafgl_button_t button1;
    rafgl_button_innit(&button1,x0+200,y0+185,80,30,rafgl_RGB(80,80,80));
  //  rafgl_button_show(&raster,&button1);
    rafgl_raster_draw_string(&raster,"Exit?",x0+165,y0+160,rafgl_RGB(255,255,255),640);
    int quit=rafgl_button_check(&button1,game_data);
    if(quit!=0){
        exit(0);
    }



}
