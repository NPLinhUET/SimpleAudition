#ifndef AUDITION_INIT
#define AUDITION_INIT

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <fstream>
#include <vector>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const string WINDOW_TITLE = "Simple Audition";
const int NUMBER_OF_IMAGE = 4, MAX_MISS = 3, ARROW_SIZE = 100, NUM_OF_TRACKS = 4, CDTIME = 3,
            COLS = (SCREEN_WIDTH-200)/ARROW_SIZE, ARROWS_ON_SCREEN = (SCREEN_HEIGHT-100)/ARROW_SIZE,
            SCORE = 100, LEN_OF_NAME = 3, TOP = 10;
const SDL_Keycode ARROW []= {SDLK_LEFT,SDLK_RIGHT,SDLK_UP,SDLK_DOWN};
const string IMAGE[NUMBER_OF_IMAGE] = {"image/left.bmp","image/right.bmp","image/up.bmp","image/down.bmp"};

class Text{
    string str;
    TTF_Font *font;
    SDL_Color fg;
public:
    Text(string _str, string file, int _size, const SDL_Color &_fg);
    Text(string file, int _size, const SDL_Color &_fg);
    ~Text();
    TTF_Font *getFont();
    SDL_Color getColor();
    string getText();
    void setText(const string &s);
    void setColor(const SDL_Color &_fg);
    void setFont(string file, int _size);
};

void logSDLError(std::ostream& os, const std::string &msg, bool fatal);
void initSDL(SDL_Window* &window, SDL_Renderer* &renderer);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);
void waitUntilKeyPressed();
void initTTF();
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y);
void setSoundtrack(Mix_Music **music);
void freeMemory(SDL_Renderer *renderer, Mix_Music *music, SDL_Texture *bgImage, SDL_Texture *dest,
                  SDL_Texture *life);
void render_Text(SDL_Renderer *renderer, Text *text,int x, int y, int w, int h);
void render_Text(SDL_Renderer *renderer, Text *text, int deltaX, int deltaY);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren);
void highScoreInp(string file, vector <string> &scoreList);
void putNewHighScoreToFile(vector <string> &scoreList);
void sortScore(vector <string> &scoreList);
#endif // AUDITION_INIT
