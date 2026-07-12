#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "micro-ui/microui.h"

TTF_Font *global_font = NULL;

int text_width_callback(mu_Font font, const char *str, int len) {
    if (len == -1) { len = strlen(str); }
    int w = 0, h = 0;
    TTF_SizeText(global_font, str, &w, &h);
    return w;
}

int text_height_callback(mu_Font font) {
    return TTF_FontHeight(global_font);
}


int main(int argc, char *argv[]) {
    


    if (SDL_Init(SDL_INIT_VIDEO) < 0) { return 1; }
    if (TTF_Init() < 0) { SDL_Quit(); return 1; }

    SDL_Window *window = SDL_CreateWindow("MicroUI Pure Loop Example", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    global_font = TTF_OpenFont("/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf", 14);
    if (!global_font) {
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    mu_Context *ctx = malloc(sizeof(mu_Context));
    mu_init(ctx);
    ctx->text_width = text_width_callback;
    ctx->text_height = text_height_callback;

    int running = 1;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running = 0; }
            else if (e.type == SDL_MOUSEMOTION) { 
                mu_input_mousemove(ctx, e.motion.x, e.motion.y); 
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
                int btn = (e.button.button == SDL_BUTTON_LEFT) ? MU_MOUSE_LEFT : MU_MOUSE_RIGHT;
                if (e.type == SDL_MOUSEBUTTONDOWN) mu_input_mousedown(ctx, e.button.x, e.button.y, btn);
                else mu_input_mouseup(ctx, e.button.x, e.button.y, btn);
            }
        }

        mu_begin(ctx);
        if (mu_begin_window(ctx, "My First Window", mu_rect(50, 50, 300, 200))) {
            mu_layout_row(ctx, 1, (int[]) { -1 }, 0);
            mu_label(ctx, "Hello World from MicroUI!");
            if (mu_button(ctx, "Quit Application")) { running = 0; }
            mu_end_window(ctx);
        }
        mu_end(ctx);

        SDL_SetRenderDrawColor(renderer, 30, 30, 35, 255);
        SDL_RenderClear(renderer);

        mu_Command *cmd = NULL;
        while (mu_next_command(ctx, &cmd)) {
            switch (cmd->type) {
                case MU_COMMAND_RECT:
                    SDL_SetRenderDrawColor(renderer, cmd->rect.color.r, cmd->rect.color.g, cmd->rect.color.b, cmd->rect.color.a);
                    SDL_Rect r = { cmd->rect.rect.x, cmd->rect.rect.y, cmd->rect.rect.w, cmd->rect.rect.h };
                    SDL_RenderFillRect(renderer, &r);
                    break;
                case MU_COMMAND_TEXT: {
                    if (!cmd->text.str || cmd->text.str[0] == '\0') break;
                    SDL_Color text_color = { cmd->text.color.r, cmd->text.color.g, cmd->text.color.b, cmd->text.color.a };
                    SDL_Surface *surface = TTF_RenderText_Blended(global_font, cmd->text.str, text_color);
                    if (surface) {
                        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
                        SDL_Rect text_target = { cmd->text.pos.x, cmd->text.pos.y, surface->w, surface->h };
                        SDL_RenderCopy(renderer, texture, NULL, &text_target);
                        SDL_FreeSurface(surface);
                        SDL_DestroyTexture(texture);
                    }
                    break;
                }
                case MU_COMMAND_CLIP: {
                    SDL_Rect clip = { cmd->clip.rect.x, cmd->clip.rect.y, cmd->clip.rect.w, cmd->clip.rect.h };
                    SDL_RenderSetClipRect(renderer, &clip);
                    break;
                }
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16); 
    }

    TTF_CloseFont(global_font);
    TTF_Quit();
    free(ctx);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();





    return 0;
}
