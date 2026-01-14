#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

int main() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "IMG_Init Error: " << IMG_GetError() << std::endl;
        return -1;
    }
    SDL_Window* window = SDL_CreateWindow(
    "CURSOR PLATFORMER",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    1, 1,
    SDL_WINDOW_FULLSCREEN
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_Surface* surface = IMG_Load("cursor.png");
    if (!surface) {
        std::cout << "IMG_Load Error: " << IMG_GetError() << std::endl;
        return -1;
    }
    SDL_Texture* tCursor = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    int w, h;
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
    buttonDown.y = buttonUp.y  + buttonUp.h + 50;
    buttonDown.w = 200;
    buttonDown.h = 200;
    
    SDL_Rect buttonClick;
    buttonClick.x = buttonUp.x - 210;
    buttonClick.y = buttonDown.y;
    buttonClick.w = 200;
    buttonClick.h = 200;
    
    surface = IMG_Load("phldr.png");
    SDL_Texture* tP = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    int pX = 10;
    int pY = 10;
    SDL_Event event;
    bool running = true;
    int cX = 0;
    int cY = 0;
    bool down = false;
    bool down2 = false;
    SDL_FingerID fingerId1 = -1;
SDL_FingerID fingerId2 = -1;
    
        while (running) {
                int mX, mY, mmX, mmY;
                //SDL_FingerID fingerId1 = -1;
//SDL_FingerID fingerId2 = -1;
                //Uint32 mouseState = SDL_GetMouseState(&mX, &mY);
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
                        if(down || down2) {
                            // Button Left
if (((mX >= buttonLeft.x && mX <= buttonLeft.x + buttonLeft.w) &&
     (mY >= buttonLeft.y && mY <= buttonLeft.y + buttonLeft.h)) ||
    ((mmX >= buttonLeft.x && mmX <= buttonLeft.x + buttonLeft.w) &&
     (mmY >= buttonLeft.y && mmY <= buttonLeft.y + buttonLeft.h))) {
    cX -= 10;
}
// Button Right
if (((mX >= buttonRight.x && mX <= buttonRight.x + buttonRight.w) &&
          (mY >= buttonRight.y && mY <= buttonRight.y + buttonRight.h)) ||
         ((mmX >= buttonRight.x && mmX <= buttonRight.x + buttonRight.w) &&
          (mmY >= buttonRight.y && mmY <= buttonRight.y + buttonRight.h))) {
    cX += 10;
}
                        
// Button Up
if (((mX >= buttonUp.x && mX <= buttonUp.x + buttonUp.w) &&
     (mY >= buttonUp.y && mY <= buttonUp.y + buttonUp.h)) ||
    ((mmX >= buttonUp.x && mmX <= buttonUp.x + buttonUp.w) &&
     (mmY >= buttonUp.y && mmY <= buttonUp.y + buttonUp.h))) {
    cY -= 10;
}
// Button Down
if (((mX >= buttonDown.x && mX <= buttonDown.x + buttonDown.w) &&
          (mY >= buttonDown.y && mY <= buttonDown.y + buttonDown.h)) ||
         ((mmX >= buttonDown.x && mmX <= buttonDown.x + buttonDown.w) &&
          (mmY >= buttonDown.y && mmY <= buttonDown.y + buttonDown.h))) {
    cY += 10;
}
// Button Click
if (((mX >= buttonClick.x && mX <= buttonClick.x + buttonClick.w) &&
          (mY >= buttonClick.y && mY <= buttonClick.y + buttonClick.h)) ||
         ((mmX >= buttonClick.x && mmX <= buttonClick.x + buttonClick.w) &&
          (mmY >= buttonClick.y && mmY <= buttonClick.y + buttonClick.h))) {
    if (cX >= pX && cX <= pX + 128 && cY >= pY && cY <= pY + 128) {
        pX = rand() % (w - 128);
        pY = rand() % (h - 128);
    }
}
                        }
                            

        
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_Rect dest = {pX, pY, 128, 128};
        SDL_RenderCopy(renderer, tP, NULL, &dest);

        dest = {cX, cY, 28, 28};
        SDL_RenderCopy(renderer, tCursor, NULL, &dest);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);

        SDL_RenderFillRect(renderer, &buttonLeft);
        SDL_RenderFillRect(renderer, &buttonRight);
        SDL_RenderFillRect(renderer, &buttonUp);
        SDL_RenderFillRect(renderer, &buttonDown);
        SDL_RenderFillRect(renderer, &buttonClick);

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    SDL_DestroyTexture(tCursor);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}