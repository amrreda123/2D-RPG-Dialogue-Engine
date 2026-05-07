#pragma once
#include <string>
#include <vector>
#include <GL/glut.h>

// 1. التعريفات (Structs & Enums)
struct DialogueLine {
    std::string speaker, text;
    float r, g, b;
    bool isChoice = false;
    std::string opt1, file1;
    std::string opt2, file2;
};

struct RainDrop {
    float x, y;
    float speed;
};

enum GameState { MENU, PLAYING };

// 2. إعلان عن المتغيرات (Extern Variables)
// كلمة extern بتقول للبرنامج: "المتغير ده موجود، بس هتلاقي قيمته الحقيقية في ملف الـ main"
extern GameState currentState;
extern std::vector<DialogueLine> script;
extern int currentLine;
extern float fadeAlpha;
extern float flashAlpha;
extern float shakeIntensity;
extern std::vector<RainDrop> rainSystem;
extern int displayedChars;
extern int lastTypeTime;

// الثوابت (Constants) ممكن تفضل هنا عادي
const int RAIN_COUNT = 100;
const int TYPE_SPEED = 30;