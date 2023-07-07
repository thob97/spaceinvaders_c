#include <stdio.h>
#include <time.h>
#include "headers/SDL.h"
#include "headers/SDL_image.h"
#include "headers/SDL_mixer.h"
#include "headers/SDL_ttf.h"
//cc `sdl2-config --cflags --libs` -o name main.c -lSDL2
//gcc -I/Applications/_mbrew/Cellar/sdl2/2.28.1/include -L/Applications/_mbrew/Cellar/sdl2/2.28.1/lib  -o test main.c -lSDL2

struct character{
    SDL_Rect hitbox;
    SDL_Rect shot;
    int health;
    int isShotAvailable;    //0 = no, 1 = yes
    char name;              //p = player, e = enemy, w = wall, s = special
    char type;              //p = purple, y = yellow, g = green
    int points;
};

int main (int argc, char *argv[]){
    int shipCollums = 8;
    int shipRows = 5;
    int wall_rows = 5;
    int wall_collums = 3;
    int wall_anzahl = 3;
    int window_w = 680;
    int window_h = 480;
    int listcounter = 0;
    int listsize = (1+(shipCollums*shipRows)+(wall_anzahl*wall_collums*wall_rows));
    struct character list[listsize];
    
    int points = 0;
    
    //audio
    const int samplingFrequency = 44100;    // 44100 Hz Abtastfrequenz
    Uint16 audioFormat = AUDIO_S16SYS;      // 16 Bits pro Sample
    const int numChannels = 2;              // 2 Kanäle = Stereo
    const int chunkSize = 4096;             // ein guter Wert ...
    
    //random
    srandom((int)time(NULL));

    SDL_Window *window;
    SDL_Renderer *renderer;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1 )
    {
        printf("error Audio oder Video %s\n", SDL_GetError() );
    }

    if(Mix_OpenAudio(samplingFrequency, audioFormat, numChannels, chunkSize) == -1)
    {
        printf("error 1 %s\n", SDL_GetError() );
    }
    
    Mix_Chunk* sound = Mix_LoadWAV("music/boom.wav");
    Mix_Chunk* sound2 = Mix_LoadWAV("music/firestorm.wav");
    Mix_Music* music = Mix_LoadMUS("music/windman.mp3");
    Mix_Chunk* sound3 = Mix_LoadWAV("music/victory.wav");
    Mix_Chunk* sound4 = Mix_LoadWAV("music/gameover.wav");
    Mix_Chunk* sound5 = Mix_LoadWAV("music/loseexplosion.wav");
    if(!sound || !sound2 || !music || !sound3)
    {
        printf("error at sounds: %s\n", SDL_GetError() );
    }

    //Window Setting
    window=SDL_CreateWindow("Game Window",                          
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            window_w,
                            window_h,
                            0
                            );
    
    
    if(!window) {
    printf("Window could not be created SDL_Error: %s\n",SDL_GetError());
    } else {
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    }
    
    renderer = SDL_CreateRenderer(window, -1, (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    
    if (!renderer) {
        printf("error creating renderer: %s\n", SDL_GetError() );
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return(1);
    }
    //load images into ram
    SDL_Surface* sbg02 = IMG_Load("pictures/player01.png");
    SDL_Surface* sbg03 = IMG_Load("pictures/player02.png");
    SDL_Surface* sbg04 = IMG_Load("pictures/enemy101.png");
    SDL_Surface* sbg05 = IMG_Load("pictures/enemy102.png");
    SDL_Surface* sbg06 = IMG_Load("pictures/enemy201.png");
    SDL_Surface* sbg07 = IMG_Load("pictures/enemy202.png");
    SDL_Surface* sbg08 = IMG_Load("pictures/enemy301.png");
    SDL_Surface* sbg09 = IMG_Load("pictures/enemy302.png");
    //SDL_Surface* sbg10 = IMG_Load("pictures/bonus01.png");
    //SDL_Surface* sbg11 = IMG_Load("pictures/bonus02.png");
    SDL_Surface* sbg12 = IMG_Load("pictures/explosion01.png");
    SDL_Surface* sbg13 = IMG_Load("pictures/explosion02.png");
    if(!sbg02 || !sbg03 || !sbg04 || !sbg05 || !sbg06 || !sbg07 || !sbg08 || !sbg09){
        printf("error creating sbg file\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return(1);
    }
    //make texture from images
    SDL_Texture* tex02 = SDL_CreateTextureFromSurface(renderer, sbg02);
    SDL_FreeSurface(sbg02);
    SDL_Texture* tex03 = SDL_CreateTextureFromSurface(renderer, sbg03);
    SDL_FreeSurface(sbg03);
    SDL_Texture* tex04 = SDL_CreateTextureFromSurface(renderer, sbg04);
    SDL_FreeSurface(sbg04);
    SDL_Texture* tex05 = SDL_CreateTextureFromSurface(renderer, sbg05);
    SDL_FreeSurface(sbg05);
    SDL_Texture* tex06 = SDL_CreateTextureFromSurface(renderer, sbg06);
    SDL_FreeSurface(sbg06);
    SDL_Texture* tex07 = SDL_CreateTextureFromSurface(renderer, sbg07);
    SDL_FreeSurface(sbg07);
    SDL_Texture* tex08 = SDL_CreateTextureFromSurface(renderer, sbg08);
    SDL_FreeSurface(sbg08);
    SDL_Texture* tex09 = SDL_CreateTextureFromSurface(renderer, sbg09);
    SDL_FreeSurface(sbg09);
    if (!tex02 || !tex03 || !tex04 || !tex05 || !tex06 || !tex07 || !tex08 || !tex09) {
        printf("error creating texture: %s\n", SDL_GetError() );
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return(1);
    }
    
    //start music playback
    Mix_PlayMusic(music, -1); //audio
    
    //player settings
    list[listcounter].hitbox.w = 18;
    list[listcounter].hitbox.h = 36;
    list[listcounter].hitbox.x = (window_w/2)-(list[listcounter].hitbox.w/2);
    list[listcounter].hitbox.y = 440;
    list[listcounter].shot.x = -100;
    list[listcounter].shot.y = 0;
    list[listcounter].shot.w = 3;
    list[listcounter].shot.h = 10;
    list[listcounter].health = 3;
    list[listcounter].isShotAvailable = 1;
    list[listcounter].name = 'p';
    list[listcounter].points = 0;
    listcounter++;
    
    //Ship Settings
    for (int i=0; i<shipCollums; i++){
        for (int j=0; j<shipRows; j++){
            if (j % 3){
                list[listcounter].hitbox.w = 39;
                list[listcounter].hitbox.h = 27;
                list[listcounter].hitbox.x = i * (window_w/shipCollums-shipCollums*3) + 20;
                list[listcounter].hitbox.y = (40 * j) + 20;
                list[listcounter].shot.x = -100;
                list[listcounter].shot.y = 0;
                list[listcounter].shot.w = 3;
                list[listcounter].shot.h = 10;
                list[listcounter].health = 1;
                list[listcounter].isShotAvailable = 1;
                list[listcounter].name = 'e';
                list[listcounter].type = 'g';
                list[listcounter].points = 10;
            }
            else if (j % 2){
                list[listcounter].hitbox.w = 18;
                list[listcounter].hitbox.h = 24;
                list[listcounter].hitbox.x = i * (window_w/shipCollums-shipCollums*3) + 20 + (list[listcounter].hitbox.w/2);
                list[listcounter].hitbox.y = (40 * j) + 20;
                list[listcounter].shot.x = -100;
                list[listcounter].shot.y = 0;
                list[listcounter].shot.w = 3;
                list[listcounter].shot.h = 10;
                list[listcounter].health = 1;
                list[listcounter].isShotAvailable = 1;
                list[listcounter].name = 'e';
                list[listcounter].type = 'y';
                list[listcounter].points = 30;
            }
            else{
                list[listcounter].hitbox.w = 26;
                list[listcounter].hitbox.h = 22;
                list[listcounter].hitbox.x = i * (window_w/shipCollums-shipCollums*3) + 20 + (list[listcounter].hitbox.w/4);
                list[listcounter].hitbox.y = (40 * j) + 20;
                list[listcounter].shot.x = -100;
                list[listcounter].shot.y = 0;
                list[listcounter].shot.w = 3;
                list[listcounter].shot.h = 10;
                list[listcounter].health = 1;
                list[listcounter].isShotAvailable = 1;
                list[listcounter].name = 'e';
                list[listcounter].type = 'p';
                list[listcounter].points = 50;
            }
            listcounter++;
        }
    }
    
    
    //Wall Settings
    for(int q=0; q<wall_anzahl; q++) {
        for (int i=0; i<wall_rows; i++){
            for (int j=0; j<wall_collums; j++){
                list[listcounter].hitbox.x = i*11+110+(605/wall_anzahl)*q; //q*x = Abstant nach jeder Wall
                list[listcounter].hitbox.y = j*11+390;
                list[listcounter].hitbox.w = 11;
                list[listcounter].hitbox.h = 11;
                list[listcounter].shot.x = -100;
                list[listcounter].shot.y = 0;
                list[listcounter].shot.w = 0;
                list[listcounter].shot.h = 0;
                list[listcounter].health = 1;
                list[listcounter].name = 'w';
                list[listcounter].points = 0;
                listcounter++;
                
            }
        }
    }
    
    
    //While loop with parameters
    int done = 0;
    int frame = 0;
    int move = 20;
    int movedown = 0;
    int shotspeed = 8;
    SDL_Rect boom = {0,0,0,0};  //Useless Rect
    int count = 8;
    int count2 = 0;
    int gametime = 0;
    int musicStatus = 1;
    int enemysLeft = shipCollums*shipRows;
    int enemySpeed = 60;        //smaller = faster
    while (!done) {
        SDL_Delay(1000/60);          //60 Fps
        list[0].shot.y -= shotspeed;
        
        
        
        //Events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type==SDL_QUIT) {
                done=1;
            } 
            else if (event.type==SDL_KEYDOWN) {
                //Esc Key => close
                if (event.key.keysym.sym==SDLK_ESCAPE) {
                    done=1;
                } 
                //Right => move Player right + 10
                else if (event.key.keysym.sym==SDLK_RIGHT && list[0].hitbox.x < (680 - list[0].hitbox.w)) {
                    list[0].hitbox.x+=11;
                } 
                //Left => move Player left + 10
                else if (event.key.keysym.sym==SDLK_LEFT && list[0].hitbox.x > 1) {
                    list[0].hitbox.x-=11;
                }
                //Space => shot
                else if (event.key.keysym.sym==SDLK_SPACE && (list[0].shot.y < 0 || list[0].isShotAvailable == 1)) {
                    list[0].isShotAvailable = 0;
                    list[0].shot.y = list[0].hitbox.y- (list[0].hitbox.h/2)+1; 
                    list[0].shot.x = list[0].hitbox.x + (list[0].hitbox.w/2);
                    Mix_PlayChannel(-1, sound, 0);
                }
                //s key => music on/off
                else if (event.key.keysym.sym == SDLK_s){
                    if (musicStatus == 1){
                        Mix_PauseMusic();
                        musicStatus = 0;
                    }
                    else{
                        Mix_ResumeMusic();
                        musicStatus = 1;
                    }
                }
            }
        }
        
        //background render
        SDL_SetRenderDrawColor(renderer,13,13,38,0);
        SDL_RenderClear(renderer);
        
        if (frame == 60 || (frame == 30 && enemysLeft <= (shipRows*shipCollums/2) || ((frame == 15 || frame == 45) && enemysLeft <= (shipRows*shipCollums/4)))){
            if (count2 == count){
                move = move * (-1);
                movedown = 20; 
                count2 = 0;
            }
                count2++;
        }
        for (int i = 0; i < listsize; i++){
            //Shot color
            SDL_SetRenderDrawColor(renderer,0,204,255,0);
            SDL_RenderFillRect(renderer,&list[i].shot);
            
            //Rendering Ships
            if(list[i].health != 0){
                switch(list[i].name){
                    case 'p':
                        //player ship render
                        if (list[i].health > 0){
                            if(frame > 30){
                                SDL_RenderCopy(renderer, tex02, NULL, &list[i].hitbox);
                            }
                            else{
                                SDL_RenderCopy(renderer, tex03, NULL, &list[i].hitbox);
                            }
                        } 
                    break;
                    case 'e':
                        //enemy render
                        switch(list[i].type){
                            case 'g':
                                if(frame > 30){
                                SDL_RenderCopy(renderer, tex04, NULL, &list[i].hitbox);
                                }
                                else{
                                    SDL_RenderCopy(renderer, tex05, NULL, &list[i].hitbox);
                                }
                            break;
                            case 'y':
                                if(frame > 30){
                                SDL_RenderCopy(renderer, tex08, NULL, &list[i].hitbox);
                                }
                                else{
                                    SDL_RenderCopy(renderer, tex09, NULL, &list[i].hitbox);
                                }
                            break;
                            case 'p':
                                if(frame > 30){
                                SDL_RenderCopy(renderer, tex06, NULL, &list[i].hitbox);
                                }
                                else{
                                    SDL_RenderCopy(renderer, tex07, NULL, &list[i].hitbox);
                                }
                            break;
                        }
                        //enemy ship shot movement
                        list[i].shot.y += shotspeed;
                    break;
                    case 'w':
                        //walls render
                        SDL_SetRenderDrawColor(renderer,255,255,255,0);
                        if (list[i].health > 0){
                            SDL_RenderFillRect(renderer,&list[i].hitbox);
                        } 
                    break;
                }
            }
            //collision
            for(int j = 0; j < listsize; j++){
                if (!(list[i].name == 'e' && list[j].name == 'e')){
                    if( (SDL_IntersectRect(&list[i].shot, &list[j].hitbox, &boom) == SDL_TRUE && list[j].health > 0 && i != j)){
                        list[j].health -= 1;
                        list[i].shot.x = -100;
                        list[i].isShotAvailable = 1;
                        Mix_PlayChannel(-1, sound2, 0);
                        if (list[j].name == 'e'){
                            points += list[j].points;
                            enemysLeft--;
                            if(enemysLeft == 0){
                                printf("YOU WIN\nSCORE: %d\n", points);
                                Mix_PlayChannel(-1, sound3, 0);
                                SDL_Delay(7000);
                                done = 1;
                            }
                            else{
                                printf("%d enemys left!\n", enemysLeft);
                            }
                        }
                    }
                    if(SDL_IntersectRect(&list[i].hitbox, &list[j].hitbox, &boom) && i != j && list[i].name != 'w' && list[j].name != 'w'){
                        printf("GAME OVER\n");
                        Mix_PlayChannel(-1, sound4, 0);
                        SDL_Delay(2000);
                        Mix_PlayChannel(-1, sound5, 0);
                        SDL_Delay(5000);
                        done = 1;
                    }
                }
                
            }
            
            //Ships Movement
            
            
            if((frame == 60 || (frame == 30 && enemysLeft <= (shipRows*shipCollums/2) || ((frame == 15 || frame == 45) && enemysLeft <= (shipRows*shipCollums/4)))) && list[i].name == 'e'){
                
                list[i].hitbox.x += move;
                list[i].hitbox.y += movedown;
                
                //Ships Shooting chance
                int chance = random() % 100;
                if (chance<=5 && list[i].isShotAvailable == 1 && list[i].health !=0){
                    list[i].isShotAvailable = 0;
                    list[i].shot.x = list[i].hitbox.x + (list[i].hitbox.w/2) + 1;
                    list[i].shot.y = list[i].hitbox.y + (list[i].hitbox.h/2) + 1;
                    Mix_PlayChannel(-1, sound, 0);
                }
            }
        }
        
        movedown = 0;
        
        //render onto foregroundwindow
        SDL_RenderPresent(renderer);
        
        //framecounter
        if(frame == 60) {frame = 0; gametime++;}
        frame++;
        
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QUIT;
    return(1);
}
