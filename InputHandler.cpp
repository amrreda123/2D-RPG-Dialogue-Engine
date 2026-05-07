#include "InputHandler.h"
#include "Globals.h"
#include <GL/glut.h>
#include <windows.h>
#include <mmsystem.h>
#include <iostream>

// عشان يقدر يشوف دالة loadScript اللي في الـ main
extern void loadScript(const std::string &filename);

void InputHandler::keyboard(unsigned char key, int x, int y) {
    if (currentState == MENU) {
        if (key == 's' || key == 'S') {
            PlaySound(TEXT(".\\assets\\blip.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
            currentState = PLAYING;
            fadeAlpha = 1.0f; 
            std::cout << "Amro Engine: Adventure Started!" << std::endl;
        }
        else if (key == 'e' || key == 'E' || key == 27) {
            std::cout << "Amro Engine: Closing safely..." << std::endl;
            exit(0);
        }
    }
    else if (currentState == PLAYING) {
        if (currentLine >= script.size()) {
            if (key == 'r' || key == 'R') {
                PlaySound(TEXT(".\\assets\\blip.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
                fadeAlpha = 1.0f;
                currentLine = 0;                  
                loadScript("assets/script.json"); 
                std::cout << "Amro Engine: Restarting..." << std::endl;
            }
        }
        else {
            if (script[currentLine].isChoice) {
                if (key == '1') {
                    PlaySound(TEXT(".\\assets\\blip.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
                    displayedChars = 0;
                    lastTypeTime = glutGet(GLUT_ELAPSED_TIME);
                    loadScript(script[currentLine].file1);
                }
                else if (key == '2') {
                    PlaySound(TEXT(".\\assets\\blip.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
                    displayedChars = 0;
                    lastTypeTime = glutGet(GLUT_ELAPSED_TIME);
                    loadScript(script[currentLine].file2);
                }
            }
            else if (key == ' ') {
                PlaySound(TEXT(".\\assets\\blip.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
                currentLine++;     
                flashAlpha = 0.3f; 
                displayedChars = 0;
                lastTypeTime = glutGet(GLUT_ELAPSED_TIME);
                
                if (currentLine < script.size()) {
                    std::string text = script[currentLine].text;
                    if (text.find("!") != std::string::npos || text.find("?") != std::string::npos) {
                        shakeIntensity = 0.5f;
                    }
                }
            }
        }
        if (key == 27) {
            exit(0);
        }
    }
    glutPostRedisplay();
}