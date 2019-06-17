#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <sstream>
#include <fstream>
#include <vector>
#include "utils.h"

using namespace std;

void logSDLError(std::ostream& os, const std::string &msg, bool fatal)
{
    os << msg << " Error: " << SDL_GetError() << std::endl;
    if (fatal)
    {
        SDL_Quit();
        exit(1);

    }
}

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logSDLError(std::cout, "SDL_Init", true);
    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

//window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED,

//SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (window == nullptr)
        logSDLError(std::cout, "CreateWindow", true);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
                                                SDL_RENDERER_PRESENTVSYNC);
//SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(SDL_GetWindowSurface(window));
    if (renderer == nullptr)
        logSDLError(std::cout, "CreateRenderer", true);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void waitUntilKeyPressed()
{
    SDL_Event e;
    while (true) {
            if ( SDL_WaitEvent(&e) != 0 && (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
                return;
    SDL_Delay(100);
    }
}

void initTTF()
{
    if (TTF_Init() < 0)
    {
        cout << TTF_GetError() << endl;
    }
}

void setSoundtrack(Mix_Music **music)
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        cout << Mix_GetError() << endl;
    }
    *music = NULL;
    stringstream num;
    num << rand()%NUM_OF_TRACKS + 1;
    string file = "E:/Cpp/SimpleAudition/music/soundtrack" + num.str() + ".mp3";
 //   string file = "music/soundtrack5.mp3";
    *music = Mix_LoadMUS(file.c_str());
    if (music == NULL)
    {
        cout << Mix_GetError() << endl;
    }
}

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren)
{
    bool fatal;
	//Khởi tạo là nullptr để tránh lỗi 'dangling pointer'
	SDL_Texture *texture = nullptr;
	//Nạp ảnh từ tên file (với đường dẫn)
	SDL_Surface *loadedImage = SDL_LoadBMP(file.c_str());
	//Nếu không có lỗi, chuyển đổi về dạng texture and và trả về
	if (loadedImage != nullptr){
		texture = SDL_CreateTextureFromSurface(ren, loadedImage);
		SDL_FreeSurface(loadedImage);
		//Đảm bảo việc chuyển đổi không có lỗi
		if (texture == nullptr){
			logSDLError(std::cout, "CreateTextureFromSurface",fatal);
		}
	}
	else {
		logSDLError(std::cout, "LoadBMP",fatal);
	}
	return texture;
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y)
{
	//Thiết lập hình chữ nhật đích mà chúng ta muốn vẽ ảnh vào trong
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	//Truy vẫn texture để lấy chiều rộng và cao (vào chiều rộng và cao tương ứng của hình chữ nhật đích)
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    //Đưa toàn bộ ảnh trong texture vào hình chữ nhật đích
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h)
{
	//Thiết lập hình chữ nhật đích mà chúng ta muốn vẽ ảnh vào trong
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
    dst.w = w;
    dst.h = h;
    //Đưa toàn bộ ảnh trong texture vào hình chữ nhật đích
    //(ảnh sẽ co dãn cho khớp với kích cỡ mới)
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

void freeMemory(SDL_Renderer *renderer, Mix_Music *music, SDL_Texture *bgImage, SDL_Texture *dest,
                  SDL_Texture *life)
{
    Mix_FreeMusic(music);
    SDL_DestroyTexture(bgImage);
    SDL_DestroyTexture(dest);
    SDL_DestroyTexture(life);

}
void render_Text(SDL_Renderer *renderer, Text *text,int x, int y, int w, int h)
{
    SDL_Surface *surface = TTF_RenderText_Blended(text->getFont(), text->getText().c_str(), text->getColor());
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    renderTexture(tex,renderer,x,y,w,h);
    SDL_DestroyTexture(tex);
}

void render_Text(SDL_Renderer *renderer, Text *text, int deltaX, int deltaY)
{
    SDL_Surface *surface = TTF_RenderText_Blended(text->getFont(), text->getText().c_str(), text->getColor());
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    int w, h;
    SDL_QueryTexture(tex,NULL,NULL,&w,&h);
    renderTexture(tex,renderer,SCREEN_WIDTH/2-w/2+deltaX,SCREEN_HEIGHT/2-h/2+deltaY,w,h);
    SDL_DestroyTexture(tex);
}
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren)
{
    int w, h;
    SDL_QueryTexture(tex,NULL,NULL,&w,&h);
    renderTexture(tex,ren,SCREEN_WIDTH/2-w/2,SCREEN_HEIGHT/2-h/2);
}

void highScoreInp(string fileName, vector <string> &scoreList)
{
    ifstream file(fileName.c_str());
    string s;
    if (file.is_open())
    {
        while (file >> s)
        {
            scoreList.push_back(s);
        }
        file.close();
    }
    else cout << "can't open file";
}

void sortScore(vector <string> &scoreList)
{
    for (unsigned i=0; i<scoreList.size()-1; i++)
        for (unsigned j=i+1; j<scoreList.size(); j++)
            {
                string s1 = scoreList[i].substr(scoreList[i].length()-6);
                string s2 = scoreList[j].substr(scoreList[j].length()-6);
                if (s1<s2)
                swap(scoreList[i],scoreList[j]);
            }
}

void putNewHighScoreToFile(vector <string> &scoreList)
{
    ofstream file("high score.txt");
    for (unsigned i=0; i<scoreList.size(); i++)
        file << scoreList[i] << endl;
    file.close();
}

Text::Text(string _str, string file, int _size, const SDL_Color &_fg):
        str(_str)
    {
        fg = _fg;
        font = NULL;
        font = TTF_OpenFont(file.c_str(),_size);
        if (font == NULL)
        {
            cout << TTF_GetError() << endl;
        }
    }
Text::Text(string file, int _size, const SDL_Color &_fg)
    {
        fg = _fg;
        font = NULL;
        font = TTF_OpenFont(file.c_str(),_size);
        if (font == NULL)
        {
            cout << TTF_GetError() << endl;
        }
    }
Text::~Text()
{
    TTF_CloseFont(font);
}
TTF_Font* Text::getFont()
{
    return font;
}
SDL_Color Text::getColor() {return fg;}
string Text::getText() {return str;}
void Text::setText(const string &s) {str = s;}
void Text::setColor(const SDL_Color &_fg) {fg = _fg;}
void Text::setFont(string file, int _size)
    {
        TTF_Font *curFont = font;
        font = TTF_OpenFont(file.c_str(),_size);
        if (font == curFont)
        {
            cout << TTF_GetError() << endl;
        }
    }
