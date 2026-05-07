#define STB_IMAGE_IMPLEMENTATION
#include "include/Globals.h"
#include "include/InputHandler.h"
#include "include/Character.h"
#include "include/DisplayHandler.h"
#include "include/ScriptLoader.h"
#include "include/EngineInitializer.h"
GameState currentState = MENU;
std::vector<DialogueLine> script;
std::vector<Character *> cast;
std::vector<RainDrop> rainSystem;
GLuint bgTexture;

int currentLine = 0;
float fadeAlpha = 1.0f;
float flashAlpha = 0.0f;
float shakeIntensity = 0.0f;
int displayedChars = 0;
int lastTypeTime = 0;

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1280, 720);
    // مقاس HD مضبوط
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Amro Engine - Adventure Pro Edition");
    EngineInitializer::init();
    glutDisplayFunc(DisplayHandler::display);
    glutKeyboardFunc(InputHandler::keyboard);
    glutIdleFunc(DisplayHandler::display);
    glutMainLoop();
    return 0;
}