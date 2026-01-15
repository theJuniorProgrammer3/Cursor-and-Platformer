#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>
#include <vector>

static std::random_device rd;
static std::mt19937 gen(rd());
int ROWS = 20;
int COLS = 100;
int w, h;
SDL_Rect camera = {0, 0, w, h};  // w,h = ukuran window
// std::vector<std::vector<int>> maze;
std::vector<std::vector<int>> maze(ROWS, std::vector<int>(COLS, 0));

// 0 = dinding, 1 = jalan
// int maze[ROWS][COLS];

struct Cell {
  int r, c;
};

int dr[4] = {-2, 2, 0, 0};  // langkah 2 sel
int dc[4] = {0, 0, -2, 2};

bool inBounds(int r, int c) {
  return r > 0 && r < ROWS - 1 && c > 0 && c < COLS - 1;
}

void dfs(int r, int c) {
  maze[r][c] = 1;  // tandai sebagai jalan

  // buat urutan arah acak
  std::vector<int> dirs = {0, 1, 2, 3};
  std::shuffle(dirs.begin(), dirs.end(), gen);

  for (int i : dirs) {
    int nr = r + dr[i];
    int nc = c + dc[i];
    if (inBounds(nr, nc) && maze[nr][nc] == 0) {
      // buka dinding di antara
      maze[r + dr[i] / 2][c + dc[i] / 2] = 1;
      dfs(nr, nc);
    }
  }
}

void genMaze() {
  // init semua jadi dinding
  /* for (int r=0; r<ROWS; r++)
       for (int c=0; c<COLS; c++)
           maze[r][c] = 0;*/

  // srand(time(0));
  dfs(1, 1);  // mulai dari (1,1)
}

int rw = 38;
int rh = 38;

void renderMaze(SDL_Renderer* renderer) {
  SDL_Rect r;
  r.w = rw;
  r.h = rh;
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      if (maze[i][j] == 0) {
        r.x = j * rw - camera.x;
        r.y = i * rh - camera.y;
        SDL_RenderFillRect(renderer, &r);
      }
    }
  }
}


int main() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "SDL Error: " << SDL_GetError() << std::endl;
    return -1;
  }

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    std::cout << "IMG_Init Error: " << IMG_GetError() << std::endl;
    return -1;
  }
  SDL_Window* window =
      SDL_CreateWindow("CURSOR PLATFORMER", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 1, 1, SDL_WINDOW_FULLSCREEN);
  SDL_Renderer* renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_Surface* surface = IMG_Load("cursor.png");
  if (!surface) {
    std::cout << "IMG_Load Error: " << IMG_GetError() << std::endl;
    return -1;
  }
  SDL_Texture* tCursor = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  SDL_GetWindowSize(window, &w, &h);
  SDL_Rect buttonLeft;
  buttonLeft.x = 10;
  buttonLeft.y = h - 250;
  buttonLeft.w = 200;
  buttonLeft.h = 200;

  SDL_Rect buttonRight;
  buttonRight.x = buttonLeft.x + buttonLeft.w + 50;
  buttonRight.y = h - 250;
  buttonRight.w = 200;
  buttonRight.h = 200;

  SDL_Rect buttonUp;
  buttonUp.x = w - 210;
  buttonUp.y = h - 500;
  buttonUp.w = 200;
  buttonUp.h = 200;

  SDL_Rect buttonDown;
  buttonDown.x = w - 210;
  buttonDown.y = buttonUp.y + buttonUp.h + 50;
  buttonDown.w = 200;
  buttonDown.h = 200;

  SDL_Rect buttonClick;
  buttonClick.x = buttonUp.x - 210;
  buttonClick.y = buttonDown.y;
  buttonClick.w = 200;
  buttonClick.h = 200;

  SDL_Rect buttonSwap;
  buttonSwap.x = 0;
  buttonSwap.y = rh;
  buttonSwap.w = 150;
  buttonSwap.h = 100;

  surface = IMG_Load("phldr.png");
  SDL_Texture* tP = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  surface = IMG_Load("stickman.png");
  SDL_Texture* tStickman = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  int pX = 10;
  int pY = 10;
  SDL_Event event;
  bool running = true;
  int cX = 38;
  int cY = 38;
  int sX = 38;
  int sY = 38;
  float vsY = 0;
  float gravity = 0.5;
  bool down = false;
  bool down2 = false;
  bool isCursor = false;
  bool isJump = false;
  SDL_FingerID fingerId1 = -1;
  SDL_FingerID fingerId2 = -1;

  genMaze();

  while (running) {
    int mX, mY, mmX, mmY;
    // SDL_FingerID fingerId1 = -1;
    // SDL_FingerID fingerId2 = -1;
    // Uint32 mouseState = SDL_GetMouseState(&mX, &mY);
    camera.x = cX - w / 2;
    camera.y = cY - h / 2;

    // Pastikan kamera tidak keluar dari maze
    if (camera.x < 0) camera.x = 0;
    if (camera.y < 0) camera.y = 0;
    if (camera.x > COLS * rw - camera.w) camera.x = COLS * rw - camera.w;
    if (camera.y > ROWS * rh - camera.h) camera.y = ROWS * rh - camera.h;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) running = false;

      // Finger down
      if (event.type == SDL_FINGERDOWN) {
        if (!down) {
          mX = event.tfinger.x * w;
          mY = event.tfinger.y * h;
          fingerId1 = event.tfinger.fingerId;
          down = true;
        } else {
          mmX = event.tfinger.x * w;
          mmY = event.tfinger.y * h;
          fingerId2 = event.tfinger.fingerId;
          down2 = true;
        }
        // Button Swap
        if (((mX >= buttonSwap.x && mX <= buttonSwap.x + buttonSwap.w) &&
             (mY >= buttonSwap.y && mY <= buttonSwap.y + buttonSwap.h)) ||
            ((mmX >= buttonSwap.x && mmX <= buttonSwap.x + buttonSwap.w) &&
             (mmY >= buttonSwap.y && mmY <= buttonSwap.y + buttonSwap.h))) {
          isCursor = !isCursor;
        }
        // Button Up (Stickman)
        if (((mX >= buttonUp.x && mX <= buttonUp.x + buttonUp.w) &&
             (mY >= buttonUp.y && mY <= buttonUp.y + buttonUp.h)) ||
            ((mmX >= buttonUp.x && mmX <= buttonUp.x + buttonUp.w) &&
             (mmY >= buttonUp.y && mmY <= buttonUp.y + buttonUp.h))) {
          if (!isCursor) {
            if (maze[(sY / rh) - 1][sX / rw] == 1) {
              // need update for the condition: currently have bug
              isJump = true;
              vsY = -5;
            } else {
              sY -= (sY % rh);
            }
          }
        }
      }

      // Finger up
      if (event.type == SDL_FINGERUP) {
        if (event.tfinger.fingerId == fingerId1) {
          mX = -5;
          mY = -5;
          down = false;
        } else if (event.tfinger.fingerId == fingerId2) {
          mmX = -5;
          mmY = -5;
          down2 = false;
        }
      }
    }
    if (down || down2) {
      // Button Left
      if (((mX >= buttonLeft.x && mX <= buttonLeft.x + buttonLeft.w) &&
           (mY >= buttonLeft.y && mY <= buttonLeft.y + buttonLeft.h)) ||
          ((mmX >= buttonLeft.x && mmX <= buttonLeft.x + buttonLeft.w) &&
           (mmY >= buttonLeft.y && mmY <= buttonLeft.y + buttonLeft.h))) {
        if (isCursor) {
          if (maze[cY / rh][(cX - 10) / rw] == 1)
            cX -= 10;
          else
            cX -= (cX % rw);
        } else {
          if (maze[sY / rh][(sX - 10) / rw] == 1)
            sX -= 10;
          else
            sX -= (sX % rw);
        }
      }
      // Button Right
      if (((mX >= buttonRight.x && mX <= buttonRight.x + buttonRight.w) &&
           (mY >= buttonRight.y && mY <= buttonRight.y + buttonRight.h)) ||
          ((mmX >= buttonRight.x && mmX <= buttonRight.x + buttonRight.w) &&
           (mmY >= buttonRight.y && mmY <= buttonRight.y + buttonRight.h))) {
        if (isCursor) {
          if (maze[cY / rh][(cX / rw) + 1] == 1)
            cX += 10;
          else
            cX += 10 - (cX % rw);
        } else {
          if (maze[sY / rh][(sX / rw) + 1] == 1)
            sX += 10;
          else
            sX += 10 - (sX % rw);
        }
      }

      // Button Up
      if (((mX >= buttonUp.x && mX <= buttonUp.x + buttonUp.w) &&
           (mY >= buttonUp.y && mY <= buttonUp.y + buttonUp.h)) ||
          ((mmX >= buttonUp.x && mmX <= buttonUp.x + buttonUp.w) &&
           (mmY >= buttonUp.y && mmY <= buttonUp.y + buttonUp.h))) {
        if (isCursor) {
          if (maze[(cY - 10) / rh][cX / rw] == 1)
            cY -= 10;
          else
            cY -= (cY % rh);
        }
      }
      // Button Down
      if (((mX >= buttonDown.x && mX <= buttonDown.x + buttonDown.w) &&
           (mY >= buttonDown.y && mY <= buttonDown.y + buttonDown.h)) ||
          ((mmX >= buttonDown.x && mmX <= buttonDown.x + buttonDown.w) &&
           (mmY >= buttonDown.y && mmY <= buttonDown.y + buttonDown.h))) {
        if (maze[(cY / rh) + 1][cX / rw])
          cY += 10;
        else
          cY += 10 - (cY % rh);
      }
      // Button Click
      if (((mX >= buttonClick.x && mX <= buttonClick.x + buttonClick.w) &&
           (mY >= buttonClick.y && mY <= buttonClick.y + buttonClick.h)) ||
          ((mmX >= buttonClick.x && mmX <= buttonClick.x + buttonClick.w) &&
           (mmY >= buttonClick.y && mmY <= buttonClick.y + buttonClick.h))) {
        if (cX >= pX && cX <= pX + 128 && cY >= pY && cY <= pY + 128) {
          pX = rand() % ((COLS * rw) - 128) + 1;
          pY = rand() % ((ROWS * rh) - 128) + 1;
        }
      }
    }
    // Stickman gravity
    bool lL = maze[(sY + 28) / rh][sX / rw] == 1;
    bool lR = maze[(sY + 28) / rh][(sX + 19 - 1) / rw] == 1;
    // check if stickman's leg on air
    if (lL && lR) {
      vsY += gravity;
      sY += vsY;
      isJump = false;
    } else {
      if (!isJump) {
        vsY = 0;
        sY += rh - (sY % rh) - 28;
        // empty space beetween ground and stickman's  foot
      } else {
        vsY += gravity;
        if (maze[((sY + 28 + vsY) / rh)][sX / rw] == 1 &&
            maze[((sY + 28 + vsY) / rh)][(sX + 19 - 1) / rw] == 1)
          sY += vsY;
        else {
          vsY += gravity;
          sY -= sY % rh;
          isJump = false;
        }
      }
    }
    // Init
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    // Maze
    renderMaze(renderer);
    // Placeholder :)
    SDL_Rect dest = {pX - camera.x, pY - camera.y, 128, 128};
    SDL_RenderCopy(renderer, tP, NULL, &dest);
    // Stickman
    dest = {sX - camera.x, sY - camera.y, 19, 28};
    SDL_RenderCopy(renderer, tStickman, NULL, &dest);
    // Cursor
    dest = {cX - camera.x, cY - camera.y, 28, 28};
    SDL_RenderCopy(renderer, tCursor, NULL, &dest);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    // Buttons
    SDL_RenderFillRect(renderer, &buttonLeft);
    SDL_RenderFillRect(renderer, &buttonRight);
    SDL_RenderFillRect(renderer, &buttonUp);
    SDL_RenderFillRect(renderer, &buttonDown);
    SDL_RenderFillRect(renderer, &buttonClick);
    SDL_RenderFillRect(renderer, &buttonSwap);

    SDL_RenderPresent(renderer);
    SDL_Delay(10);
  }

  SDL_DestroyTexture(tCursor);
  SDL_DestroyTexture(tStickman);
  SDL_DestroyTexture(tP);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();
}
