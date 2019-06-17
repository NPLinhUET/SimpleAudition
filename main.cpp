#include <iostream>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include <time.h>
#include <string>
#include <sstream>
#include "utils.h"
#include <fstream>
#include <cstring>

using namespace std;
//-----------------------------------------------------------------------//
class Arrow{
    int arr;
public:
    SDL_Rect shape;
    Arrow (): arr(NULL) {};
    int getArr() {return arr;}
    void creat(); // tao mui ten moi
    void drawArrow(SDL_Renderer *ren) const;//  ve mui ten ra man hinh
    void moving(int step);
};
void Arrow::creat()
{
    arr = rand()%NUMBER_OF_IMAGE; // chon mui ten bat ki
    shape.x = 100+ARROW_SIZE*(rand()%COLS);
    shape.y = 0;
    shape.h = shape.w = ARROW_SIZE;
}
void Arrow::drawArrow(SDL_Renderer *ren)
    const
    {
        SDL_Texture *tex = loadTexture(IMAGE[arr], ren);
        renderTexture(tex, ren, shape.x, shape.y, shape.w, shape.h);
        SDL_DestroyTexture(tex);
    }
void Arrow::moving(int step)
{
    shape.y += step;
}
//---------------Trang thai game---------------------------//
class Status{
    bool playing;
public:
    Status() : playing(true) {};// khoi tao mac dinh game dang chay
    bool checkPlaying() {return playing;}
    void pause() {playing = false;} // tam dung game
    void resume() {playing = true;}// choi tiep sau khi pause
};
//--------------------------------------------------------------------------------------//
void playGame(SDL_Renderer *renderer, vector <string> &scoreList);
void printMenu(SDL_Renderer *ren,const SDL_Rect &newGame, const SDL_Rect &highScore, const SDL_Rect &quitGame);
void print_high_score(SDL_Renderer *ren, const vector <string> &scoreList, const SDL_Rect &backButton);
bool pressBack(const SDL_Rect &backButton);
void countDown(int num, SDL_Renderer *ren, SDL_Texture *bgImage);
void renderScreen(SDL_Renderer *renderer, SDL_Texture *bgImage, SDL_Texture *dest,
                  SDL_Texture *life, int lives, const Arrow *arrow, bool playing, Text *score);
string enterPlayerName(SDL_Renderer *ren);
void displayEnterNameScreen(SDL_Renderer *ren, const string &playerName);
void updateHighScore(vector <string> &scoreList, const string &playerName, const string &score);
void renderFinish(SDL_Renderer *renderer, SDL_Texture *bgImage, SDL_Texture *dest, Text *score);
void arrowMoving(Arrow *arrow);
//--------------------------------------------------------------------------------------//

int main(int argc, char* argv[])
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    initSDL(window, renderer);
    initTTF();
    srand(time(NULL));
//------------------------------------------------------------------------------//
    SDL_Rect newGame, quitGame, highScore;
    highScore.x = newGame.x = quitGame.x = SCREEN_WIDTH/2-50;
    newGame.w = quitGame.w = highScore.w = 150;
    newGame.h = quitGame.h = highScore.h = 50;
    newGame.y = SCREEN_HEIGHT/2-50;
    highScore.y = newGame.y + newGame.h + 50;
    quitGame.y = highScore.y + highScore.h + 50;
    vector <string> scoreList;
    highScoreInp("high score.txt",scoreList);
    SDL_Rect backButton;
    backButton.x = 0;
    backButton.y = 525;
    backButton.h = backButton.w = 75;
    SDL_Event e;
    while (true)
    {
        printMenu(renderer,newGame,highScore,quitGame);
        if (SDL_WaitEvent(&e)==0) continue;
        if (e.type == SDL_QUIT) break;
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) break;
        if (e.type == SDL_MOUSEBUTTONDOWN)
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    if (newGame.x <= e.button.x && e.button.x <= newGame.x+newGame.w
                        && newGame.y <= e.button.y && e.button.y <= newGame.y+newGame.h)
                    {
                        playGame(renderer,scoreList);
                    }
                    if (quitGame.x <= e.button.x && e.button.x <= quitGame.x+quitGame.w
                        && quitGame.y <= e.button.y && e.button.y <= quitGame.y+quitGame.h)
                    {
                        break;
                    }
                    if (highScore.x <= e.button.x && e.button.x <= highScore.x+highScore.w
                        && highScore.y <= e.button.y && e.button.y <= highScore.y+highScore.h)
                    {
                        print_high_score(renderer,scoreList,backButton);
                        if (pressBack(backButton)) continue;
                    }
                }

    }
//----------------------------------------------------------------------------------------------//
    TTF_Quit();
    Mix_Quit();
    quitSDL(window, renderer);
    return 0;
}
//-----------------------------------------------------------------------------//
void playGame(SDL_Renderer *renderer, vector <string> &scoreList)
{
    string playerName;
    int step = 0, score = 0, cnt = 0;
    float time = 40;
    SDL_Texture *bgImage = loadTexture("image/background.bmp",renderer);
    SDL_Texture *dest = loadTexture("image/dest.bmp",renderer);
    SDL_Texture *life = loadTexture("image/life.bmp",renderer);
    int lives = MAX_MISS;
    SDL_Event e;
    Arrow *arrow = new Arrow[ARROWS_ON_SCREEN];
    Status stt;
    SDL_Color fg = {238, 18, 137};
    Text *printedScore = new Text ("000000","font/BAUHS93.TTF",30,fg);
    SDL_Rect dst;
    dst.x = 0;
    dst.y = 500;
    dst.w = 10;
    dst.h = SCREEN_WIDTH;
    countDown(CDTIME,renderer,bgImage);
    Mix_Music *music;
    setSoundtrack(&music);
    Mix_PlayMusic(music,-1);
    arrow[cnt].creat();
    do
    {// neu game dang chay
        if (stt.checkPlaying())
        {
            if (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT) break; // thoat game khi dong cua so
                if (e.type == SDL_KEYDOWN)
                {
                    if (e.key.keysym.sym == SDLK_ESCAPE) break; // thoat game khi an 'Esc'
                    // tam dung khi an 'Space'
                    if (e.key.keysym.sym == SDLK_SPACE)
                        {
                            Mix_PauseMusic();
                            stt.pause();
                            renderScreen(renderer,bgImage,dest,life,lives,arrow,stt.checkPlaying(),printedScore);
                            continue;
                        }
                    if (arrow[step].shape.y+arrow[step].shape.h > dst.y &&               //
                                arrow[step].shape.y < dst.y)                            // Neu mui ten den vach
                        {                                                              // va nguoi choi nhan dung mui ten
                                if (e.key.keysym.sym == ARROW[arrow[step].getArr()])  //
                                {// cong diem
                                    score += SCORE;
                                    stringstream s_score;
                                    s_score << score;
                                    string zero(6-s_score.str().size(),'0');
                                    printedScore->setText(zero + s_score.str());
                                    step = (step+1)%ARROWS_ON_SCREEN;// chuyen sang xet mui ten tiep theo
                                }
                                else // neu nguoi choi nhan khong dung mui ten
                                {// mat 1 mang
                                    lives--;
                                    step = (step+1)%ARROWS_ON_SCREEN; // chuyen sang xet mui ten tiep theo
                                }
                        }
                }
            }
            // neu mui ten da qua vach thi mat 1 mang
            if (arrow[step].shape.y >= dst.y)
                {
                    lives--;
                    step = (step+1)%ARROWS_ON_SCREEN;
                }
            // tao them mui ten tren man hinh
            if (arrow[cnt].shape.y == arrow[cnt].shape.h)
            {
                cnt = (cnt + 1)%ARROWS_ON_SCREEN;
                arrow[cnt].creat();
            }
            arrowMoving(arrow); // tao chuyen dong cho cac mui ten
            // hien thi len man hinh
            renderScreen(renderer,bgImage,dest,life,lives,arrow,stt.checkPlaying(),printedScore);
            SDL_Delay(time);
            if (time>1) time -= 0.025; //giam thoi gian cho => tang toc do chuyen dong cua mui ten
        }
        else // khi game dang tam dung
        {
            if (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT) break;
                if (e.type == SDL_KEYDOWN)
                {
                    if (e.key.keysym.sym == SDLK_ESCAPE) break;
                    // tiep tuc game khi an 'Space'
                    if (e.key.keysym.sym == SDLK_SPACE)
                    {
                        Mix_ResumeMusic();
                        stt.resume();
                    }
                }
            }
        }
    } while (lives>0); // chay game khi con mang
    SDL_Delay(2000);
    playerName = enterPlayerName(renderer); // nhap ten nguoi choi
    updateHighScore(scoreList,playerName,printedScore->getText()); // update danh sach diem cao
    // hien thi ket thuc game
    renderFinish(renderer,bgImage,dest,printedScore);
    waitUntilKeyPressed();
    delete [] arrow;
    delete printedScore;
    freeMemory(renderer,music,bgImage,dest,life);
}
void printMenu(SDL_Renderer *ren,const SDL_Rect &newGame, const SDL_Rect &highScore, const SDL_Rect &quitGame)
{
    SDL_Texture *bg = loadTexture("image/background.bmp",ren);
    renderTexture(bg,ren,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    SDL_Color fg = {255, 0, 0};
    Text *name = new Text ("Simple Audition", "font/PARCHM.TTF", 100, fg);
    render_Text(ren,name,100,50,SCREEN_WIDTH-200,200);
    fg = {139, 0, 139};
    Text *new_game = new Text ("New Game", "font/RAVIE.TTF", 50, fg);
    render_Text(ren,new_game,newGame.x,newGame.y,newGame.w,newGame.h);
    fg = {0, 255, 0};
    Text *high_score = new Text ("High Score", "font/RAVIE.TTF",50,fg);
    render_Text(ren,high_score,highScore.x,highScore.y,highScore.w,highScore.h);
    fg = {65, 105, 225};
    Text *quit_game = new Text ("Quit Game", "font/RAVIE.TTF",50,fg);
    render_Text(ren,quit_game,quitGame.x,quitGame.y,quitGame.w,quitGame.h);
    SDL_RenderPresent(ren);
    delete name;
    delete new_game;
    delete high_score;
    delete quit_game;
    SDL_DestroyTexture(bg);
}

void print_high_score(SDL_Renderer *ren, const vector <string> &scoreList, const SDL_Rect &backButton)
{
    SDL_Texture *bg = loadTexture("image/background.bmp",ren);
    renderTexture(bg,ren,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    SDL_Color fg = {255, 20, 147};
    Text scr("font/BAUHS93.TTF",40,fg);
    for (unsigned int i=0; i<scoreList.size(); i++)
    {
        scr.setText(scoreList[i]);
        render_Text(ren,&scr,100,SCREEN_HEIGHT/2+50*(i-scoreList.size()/2),600,50);
    }
    SDL_Texture *back_button = loadTexture("image/back.BMP",ren);
    renderTexture(back_button,ren,backButton.x,backButton.y,backButton.w,backButton.h);
    SDL_RenderPresent(ren);
    SDL_DestroyTexture(back_button);
    SDL_DestroyTexture(bg);
}

void updateHighScore(vector <string> &scoreList, const string &playerName, const string &score)
{
    string saved = playerName + "..................." + score;
    scoreList.push_back(saved);
    sortScore(scoreList);
    scoreList.erase(scoreList.end());
    for (unsigned i=0; i<scoreList.size(); i++)
        {
            stringstream num;
            num << i+1 << '.';
            if (scoreList[i] == saved)
                scoreList[i].insert(0,num.str());
            else scoreList[i].replace(0,i<9? 2:3,num.str());
        }
    putNewHighScoreToFile(scoreList);
}

bool pressBack(const SDL_Rect &backButton)
{
    SDL_Event event;
    while (true)
    {
        if (SDL_WaitEvent(&event)!=0 && event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
        {
            if (backButton.x <= event.button.x && event.button.x <= backButton.x+backButton.w
                        && backButton.y <= event.button.y && event.button.y <= backButton.y+backButton.h)
                    return true;
        }
        else SDL_Delay(100);
    }
}

string enterPlayerName(SDL_Renderer *ren)
{
    string playerName(LEN_OF_NAME,'?');
    SDL_Event e;
    displayEnterNameScreen(ren,playerName);
    SDL_StartTextInput();
    for (int i=0; i<LEN_OF_NAME; i++)
    {
        while (true)
        {
            if (SDL_WaitEvent(&e)!=0 && e.type == SDL_TEXTINPUT)
            {
                char c = *(e.text.text);
                c = toupper(c);
                playerName[i] = c;
                break;
            }
            SDL_Delay(100);
        }
        displayEnterNameScreen(ren,playerName);
    }
    waitUntilKeyPressed();
    return playerName;
}
void displayEnterNameScreen(SDL_Renderer *ren, const string &playerName)
{
    SDL_Texture *bg = loadTexture("image/background.bmp",ren);
    renderTexture(bg,ren,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    SDL_Color fg = {65, 105, 225};
    Text *tex = new Text("Please enter your name:", "font/BAUHS93.TTF",50,fg);
    render_Text(ren,tex,0,0);
    Text *name = new Text(playerName,"font/BAUHS93.TTF",50,fg);
    render_Text(ren,name,0,100);
    SDL_RenderPresent(ren);
    SDL_DestroyTexture(bg);
    delete tex;
    delete name;
}

void arrowMoving(Arrow *arrow)
{
    const int STEP = 4;
    for (int i=0; i<ARROWS_ON_SCREEN; i++)
        arrow[i].moving(STEP);
}

void countDown(int num, SDL_Renderer *ren, SDL_Texture *bgImage)
{
    SDL_Color fg = {255,0,0};
    Text cd("font/BAUHS93.TTF",150,fg);
    do
    {
        SDL_Delay(1000);
        stringstream n;
        n << num;
        cd.setText(n.str().c_str());
        renderTexture(bgImage, ren, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        render_Text(ren,&cd,0,0);
        SDL_RenderPresent(ren);
        num--;
    } while (num>0);
}

void renderFinish(SDL_Renderer *renderer, SDL_Texture *bgImage, SDL_Texture *dest, Text *score)
{
    renderTexture(bgImage, renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    renderTexture(dest, renderer, 0, 500, SCREEN_WIDTH, 10);
    SDL_Color fg = {238, 0, 0};
    score->setColor(fg);
    score->setFont("font/BAUHS93.TTF",70);
    score->setText("Your score: "+score->getText());
    render_Text(renderer,score,0,0);
    Text noti ("Press any key to back to menu!","font/FREESCPT.TTF",50,fg);
    render_Text(renderer,&noti,0,100);
    SDL_RenderPresent(renderer);
}

void renderScreen(SDL_Renderer *renderer, SDL_Texture *bgImage, SDL_Texture *dest,
                  SDL_Texture *life, int lives, const Arrow *arrow, bool playing, Text *score)
{
    renderTexture(bgImage, renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    renderTexture(dest, renderer, 0, 500, SCREEN_WIDTH, 10);
    for (int i=0; i<ARROWS_ON_SCREEN; i++)
        arrow[i].drawArrow(renderer);
    if (!playing)
    {
        SDL_Texture *pause = loadTexture("image/pause.bmp",renderer);
        renderTexture(pause,renderer);
        SDL_DestroyTexture(pause);
    }
    render_Text(renderer,score,SCREEN_WIDTH-200,SCREEN_HEIGHT-50,200,50);
    if (lives > 0)
    {
        for (int i=0; i<lives; i++)
            renderTexture(life,renderer,50*i,550,50,50);
    }
    else
    {
        SDL_Texture *gameOver = loadTexture("image/gameover.bmp",renderer);
        renderTexture(gameOver,renderer);
        SDL_DestroyTexture(gameOver);
    }
    SDL_RenderPresent(renderer);
}
