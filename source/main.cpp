#include <switch.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <chrono>
#include <SDL.h>
#include <SDL_image.h>
#include "audio.h"

int currentProgramCell = 0;
int currentCell = 0;
int errorType = 0;
int bracketsPassed = 0;
int bufferCell = 0;
int currentProgramTreeDepth = 0;
int spriteCount = 0;
int backgroundCount = 0;
int audioCount = 0;
int objectCount = 0;
int digitCount = 0;
int currentBackground = 0;
int timeSet = 0;
int cells[1028];
int programTree[5];
int programTreeTransitionCells[4];
int objectIndices[64];
int powersOfTen[6] = {1, 10, 100, 1000, 10000, 100000};
int objectTintRComponents[7] = {0, 224, 246, 255, 147, 109, 142};
int objectTintGComponents[7] = {0, 102, 178, 217, 196, 158, 124};
int objectTintBComponents[7] = {0, 102, 107, 102, 125, 235, 195};
long long int timeStart;
char assetPath[18];
char program0[1028];
char program1[1028];
char program2[1028];
char program3[1028];
char program4[1028];
bool updateScreen = false;

PadState pad;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;
SDL_Texture* characters[60];
SDL_Texture* sprites[16];
SDL_Texture* backgrounds[16];
Audio* audio[16];
SDL_Rect renderArea;

char readCurrentProgramCell() {
    switch (programTree[currentProgramTreeDepth]) {
        default:
            return program0[currentProgramCell];
        case 1:
            return program1[currentProgramCell];
        case 2:
            return program2[currentProgramCell];
        case 3:
            return program3[currentProgramCell];
        case 4:
            return program4[currentProgramCell];
    }
}

int getInputValue(int mode) {
    padUpdate(&pad);
    
    switch (mode) {
        default:
            if (padGetStickPos(&pad, 0).x < -16384) {
                return 1;
            }
        
            if (padGetStickPos(&pad, 0).x > 16384) {
                return 2;
            }
        
            if (padGetStickPos(&pad, 0).y > 16384) {
                return 3;
            }
        
            if (padGetStickPos(&pad, 0).y < -16384) {
                return 4;
            }
        
            switch (padGetButtons(&pad)) {
                case HidNpadButton_A:
                    return 5;
                case HidNpadButton_B:
                    return 6;
                case HidNpadButton_X:
                    return 7;
                case HidNpadButton_Y:
                    return 8;
                case HidNpadButton_Plus:
                    return 9;
            }
        
            return 0;
        case 1:
            return round(padGetStickPos(&pad, 0).x / 3277);
        case 2:
            return round(padGetStickPos(&pad, 0).y / 3277);
    }
}

void updateScreenIfNeeded() {
    for (int i = 0; i < objectCount; i++) {
        if (currentCell > objectIndices[i] - 1 && currentCell < objectIndices[i] + 5) {
            updateScreen = true;
        }
    }

    if (updateScreen) {
        SDL_RenderClear(renderer);
            
        renderArea.x = 0;
        renderArea.y = 0;
        renderArea.w = 1280;
        renderArea.h = 720;
            
        if (currentBackground > -1 && currentBackground < backgroundCount + 1) {
            if (currentBackground > 0) {
                SDL_RenderCopy(renderer, backgrounds[currentBackground - 1], NULL, &renderArea);
            }
        } else {
            errorType = 5;
        }

        for (int ii = 0; ii < objectCount; ii++) {
            switch (cells[objectIndices[ii] + 1]) {
                default:
                    if (cells[objectIndices[ii]] == 0) {
                        digitCount = 1;
                    } else {
                        for (int iii = 0; iii < 6; iii++) {
                            if ((double)abs(cells[objectIndices[ii]]) / powersOfTen[iii] >= 0.1) {
                                digitCount = iii;
                            }
                        }
                    }

                    if (cells[objectIndices[ii]] < 0) {
                        digitCount++;
                    }
                        
                    renderArea.y = cells[objectIndices[ii] + 4] * cells[objectIndices[ii] + 6] % 720;
                    renderArea.w = 128;
                    renderArea.h = 120;

                    for (int iii = 0; iii < digitCount; iii++) {
                        if (cells[objectIndices[ii]] < 0 && iii == digitCount - 1) {
                            texture = characters[37];
                        } else {
                            texture = characters[abs(cells[objectIndices[ii]]) / powersOfTen[iii] % 10];
                        }

                        if (cells[objectIndices[ii] + 2] > 0 && cells[objectIndices[ii] + 2] < 8) {
                            SDL_SetTextureColorMod(texture, objectTintRComponents[cells[objectIndices[ii] + 2] - 1], objectTintGComponents[cells[objectIndices[ii] + 2] - 1], objectTintBComponents[cells[objectIndices[ii] + 2] - 1]);
                        }

                        renderArea.x = (cells[objectIndices[ii] + 3] * cells[objectIndices[ii] + 5] + (digitCount - iii - 1) * 128) % 1280;
                            
                        SDL_RenderCopy(renderer, texture, NULL, &renderArea);
                    }

                    break;
                case 1:
                    if (cells[objectIndices[ii]] > -1 && cells[objectIndices[ii]] < 50) {
                        texture = characters[cells[objectIndices[ii]] + 10];

                        if (cells[objectIndices[ii] + 2] > 0) {
                            SDL_SetTextureColorMod(texture, objectTintRComponents[cells[objectIndices[ii] + 2] - 1], objectTintGComponents[cells[objectIndices[ii] + 2] - 1], objectTintBComponents[cells[objectIndices[ii] + 2] - 1]);
                        }
                            
                        renderArea.x = cells[objectIndices[ii] + 3] * cells[objectIndices[ii] + 5] % 1280;
                        renderArea.y = cells[objectIndices[ii] + 4] * cells[objectIndices[ii] + 6] % 720;
                            
                        SDL_QueryTexture(texture, NULL, NULL, &renderArea.w, &renderArea.h);
                        SDL_RenderCopy(renderer, texture, NULL, &renderArea);
                    } else {
                        errorType = 5;
                    }

                    break;
                case 2:
                    if (cells[objectIndices[ii]] > -1 && cells[objectIndices[ii]] < spriteCount) {
                        texture = sprites[cells[objectIndices[ii]]];

                        if (cells[objectIndices[ii] + 2] > 0) {
                            SDL_SetTextureColorMod(texture, objectTintRComponents[cells[objectIndices[ii] + 2] - 1], objectTintGComponents[cells[objectIndices[ii] + 2] - 1], objectTintBComponents[cells[objectIndices[ii] + 2] - 1]);
                        }

                        renderArea.x = cells[objectIndices[ii] + 3] * cells[objectIndices[ii] + 5] % 1280;
                        renderArea.y = cells[objectIndices[ii] + 4] * cells[objectIndices[ii] + 6] % 720;
                        
                        SDL_QueryTexture(texture, NULL, NULL, &renderArea.w, &renderArea.h);
                        SDL_RenderCopy(renderer, texture, NULL, &renderArea);
                    } else {
                        errorType = 5;
                    }

                    break;
            }
        }

        SDL_RenderPresent(renderer);
    }

    updateScreen = false;
}

int main(int argc, char* argv[]) {
    srand(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    chdir("sdmc:/switch/Cognito");
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
    
    FILE* file = fopen("program.txt", "r");

    fgets(program0, sizeof(program0), file);
    fgets(program1, sizeof(program1), file);
    fgets(program2, sizeof(program2), file);
    fgets(program3, sizeof(program3), file);
    fgets(program4, sizeof(program4), file);
    fclose(file);
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    initAudio();

    window = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    for (int i = 0; i < 60; i++) {
        strcpy(assetPath, ("Characters/" + std::__cxx11::to_string(i) + ".png").c_str());

        characters[i] = IMG_LoadTexture(renderer, assetPath);
    }

    for (int i = 0; i < 16; i++) {
        strcpy(assetPath, ("Sprites/" + std::__cxx11::to_string(i) + ".png").c_str());

        sprites[i] = IMG_LoadTexture(renderer, assetPath);

        if (sprites[i]) {
            spriteCount = i + 1;
        }

        strcpy(assetPath, ("Backgrounds/" + std::__cxx11::to_string(i + 1) + ".png").c_str());

        backgrounds[i] = IMG_LoadTexture(renderer, assetPath);

        if (backgrounds[i]) {
            backgroundCount = i + 1;
        }

        strcpy(assetPath, ("Audio/" + std::__cxx11::to_string(i) + ".wav").c_str());

        audio[i] = createAudio(assetPath);

        if (audio[i]) {
            audioCount = i + 1;
        }
    }
    
    while ((readCurrentProgramCell() != ';' || currentProgramTreeDepth != 0) && getInputValue(0) != 9) {
        switch(readCurrentProgramCell()) {
            case '+':
                cells[currentCell]++;
                
                updateScreenIfNeeded();
                
                break;
            case '-':
                cells[currentCell]--;
                
                updateScreenIfNeeded();
                
                break;
            case '<':
                if (currentCell > 0) {
                    currentCell--;
                } else {
                    errorType = 1;
                }
                
                break;
            case '>':
                if (currentCell < 255) {
                    currentCell++;
                } else {
                    errorType = 1;
                }

                break;
            case '[':
                if (cells[currentCell] == 0) {
                    bracketsPassed = 1;

                    while (bracketsPassed != 0 && errorType != 3) {
                        while (readCurrentProgramCell() != ']') {
                            if (readCurrentProgramCell() == ';') {
                                errorType = 3;

                                break;
                            } else {
                                currentProgramCell++;
                            }
                            
                            if (readCurrentProgramCell() == '[') {
                                bracketsPassed++;
                            }
                        }
                        
                        bracketsPassed--;
                        currentProgramCell++;
                    }

                    currentProgramCell--;
                }

                break;
            case ']':
                if (cells[currentCell] != 0) {
                    bracketsPassed = 1;

                    while (bracketsPassed != 0 && errorType != 4) {
                        while (readCurrentProgramCell() != '[') {
                            if (currentProgramCell == 0) {
                                errorType = 4;

                                break;
                            } else {
                                currentProgramCell--;
                            }
                            
                            if (readCurrentProgramCell() == ']') {
                                bracketsPassed++;
                            }
                        }
                        
                        bracketsPassed--;
                        currentProgramCell--;
                    }

                    currentProgramCell++;
                }

                break;
            case ',':
                if (cells[currentCell] == 0) {
                    cells[currentCell] = getInputValue(0);
                } else {
                    cells[currentCell] = getInputValue(1);
                    cells[currentCell + 1] = getInputValue(2);
                }

                updateScreenIfNeeded();

                break;
            case '.':
                if (cells[currentCell + 1] == 3) {
                    currentBackground = cells[currentCell];
                    updateScreen = true;
                } else {
                    objectIndices[objectCount] = currentCell;
                    objectCount++;
                }

                updateScreenIfNeeded();

                break;
            case ';':
                if (currentProgramTreeDepth > 0) {
                    currentProgramTreeDepth--;
                    currentProgramCell = programTreeTransitionCells[currentProgramTreeDepth];
                }

                break;
            case ':':
                objectCount = 0;

                break;
            case '\"':
                silenceMusic();
                
                break;
            case '?':
                cells[currentCell] = rand() % (cells[currentCell] + 1);

                updateScreenIfNeeded();

                break;
            case '!':
                if (cells[currentCell] > -1 && cells[currentCell] < audioCount) {
                    if (audio[cells[currentCell]]->length < 1920000) {
                        playSoundFromMemory(audio[cells[currentCell]]);
                    } else {
                        playMusicFromMemory(audio[cells[currentCell]], cells[currentCell + 1]);
                    }
                } else {
                    errorType = 5;
                }

                break;
            case '`':
                timeSet = cells[currentCell];
                timeStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

                break;
            case '~':
                cells[currentCell] = abs(timeSet - (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - timeStart) / 100);

                updateScreenIfNeeded();

                break;
            case '^':
                if (cells[currentCell] > -1 && cells[currentCell] < 256) {
                    currentCell = cells[currentCell];
                } else {
                    errorType = 6;
                }

                break;
            case '=':
                if (cells[currentCell] > -1 && cells[currentCell] < 256) {
                    cells[currentCell] = cells[cells[currentCell]];
                } else {
                    errorType = 6;
                }


                updateScreenIfNeeded();

                break;
            case '#':
                if (cells[currentCell] == 0) {
                    cells[currentCell] = 1;
                } else {
                    cells[currentCell] = 0;
                }

                updateScreenIfNeeded();

                break;
            case '|':
                if (cells[currentCell] < 0) {
                    cells[currentCell] = 0;
                } else {
                    cells[currentCell] = 1;
                }

                updateScreenIfNeeded();

                break;
            case '@':
                if (cells[currentCell] > 0 && cells[currentCell] < 5) {
                    programTreeTransitionCells[currentProgramTreeDepth] = currentProgramCell;
                    currentProgramTreeDepth++;
                    programTree[currentProgramTreeDepth] = cells[currentCell];
                    currentProgramCell = -1;
                } else {
                    errorType = 6;
                }

                break;
            case '/':
                bufferCell = cells[currentCell];

                break;
            case '\\':
                cells[currentCell] = bufferCell;

                updateScreenIfNeeded();

                break;
            case '&':
                cells[currentCell] *= 2;

                updateScreenIfNeeded();

                break;
            case '_':
                currentProgramCell++;
        }

        if (errorType > 0) {
            SDL_RenderClear(renderer);

            renderArea.x = 0;
            renderArea.y = 0;
            texture = characters[14];

            SDL_SetTextureColorMod(texture, 224, 102, 102);
            SDL_RenderCopy(renderer, texture, NULL, &renderArea);

            renderArea.x += 128;
            texture = characters[27];

            SDL_SetTextureColorMod(texture, 224, 102, 102);
            SDL_RenderCopy(renderer, texture, NULL, &renderArea);

            renderArea.x += 128;

            SDL_RenderCopy(renderer, texture, NULL, &renderArea);

            renderArea.x += 128;
            texture = characters[45];

            SDL_SetTextureColorMod(texture, 224, 102, 102);
            SDL_RenderCopy(renderer, texture, NULL, &renderArea);

            renderArea.x += 128;
            texture = characters[errorType];

            SDL_SetTextureColorMod(texture, 224, 102, 102);
            SDL_RenderCopy(renderer, texture, NULL, &renderArea);

            renderArea.x = 0;
            renderArea.y = 120;
            texture = characters[18];

            SDL_SetTextureColorMod(texture, 224, 102, 102);
            SDL_RenderCopy(renderer, texture, NULL, &renderArea);

            renderArea.x += 128;
            texture = characters[23];

            SDL_SetTextureColorMod(texture, 224, 102, 102);
            SDL_RenderCopy(renderer, texture, NULL, &renderArea);

            renderArea.x += 128;
            texture = characters[28];

            SDL_SetTextureColorMod(texture, 224, 102, 102);
            SDL_RenderCopy(renderer, texture, NULL, &renderArea);

            renderArea.x += 128;
            texture = characters[45];

            SDL_SetTextureColorMod(texture, 224, 102, 102);
            SDL_RenderCopy(renderer, texture, NULL, &renderArea);

            renderArea.x += 128;
            texture = characters[programTree[currentProgramTreeDepth]];

            SDL_SetTextureColorMod(texture, 224, 102, 102);
            SDL_RenderCopy(renderer, texture, NULL, &renderArea);

            renderArea.x += 128;
            texture = characters[42];

            SDL_SetTextureColorMod(texture, 224, 102, 102);
            SDL_RenderCopy(renderer, texture, NULL, &renderArea);
            
            if (currentProgramCell == 0) {
                digitCount = 1;
            } else {
                for (int i = 0; i < 4; i++) {
                    if ((double)currentProgramCell / powersOfTen[i] >= 0.1) {
                        digitCount = i;
                    }
                }
            }
            
            for (int i = 0; i < digitCount; i++) {
                renderArea.x = (digitCount - i + 5) * 128;
                texture = characters[currentProgramCell / powersOfTen[i] % 10];

                SDL_SetTextureColorMod(texture, 224, 102, 102);
                SDL_RenderCopy(renderer, texture, NULL, &renderArea);
            }

            SDL_RenderPresent(renderer);
            SDL_Delay(10000);

            break;
        }

        currentProgramCell++;
    }

    endAudio();

    for (int i = 0; i < 60; i++) {
        SDL_DestroyTexture(characters[i]);
    }

    for (int i = 0; i < spriteCount; i++) {
        SDL_DestroyTexture(sprites[i]);
    }

    for (int i = 0; i < backgroundCount; i++) {
        SDL_DestroyTexture(backgrounds[i]);
    }

    for (int i = 0; i < audioCount; i++) {
        freeAudio(audio[i]);
    }
    
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
