#include "EngineInitializer.h"
#include "Globals.h"
#include "Character.h"
#include "ScriptLoader.h"
#include "GraphicsUtils.h"
#include <GL/glut.h>
#include <GL/glu.h>

// ربط المتغيرات والدوال اللي في الملفات التانية
extern std::vector<Character*> cast;
extern GLuint bgTexture;

void EngineInitializer::init() {
    // 1. إعدادات OpenGL الأساسية
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // ضبط الكاميرا على مقاس HD (Ortho)
    gluOrtho2D(-20.0, 20.0, -11.25, 11.25);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 2. تحميل الخلفية
    bgTexture = loadTexture("assets/background.png");

    // 3. تجهيز كاست الشخصيات
    cast.push_back(new Character("Magdy Khaled", "assets/character.png", -18.5f, -5.0f, 7.0f, 10.0f));
    cast.push_back(new Character("Donia Ahamed", "assets/character2.png", -11.0f, -5.0f, 7.0f, 10.0f));
    cast.push_back(new Character("Amr Reda", "assets/character3.png", -3.5f, -5.0f, 7.0f, 10.0f));
    cast.push_back(new Character("Israa Ahmed", "assets/character4.png", 4.0f, -5.0f, 7.0f, 10.0f, true));
    cast.push_back(new Character("Manna", "assets/character5.png", 11.5f, -5.0f, 7.0f, 10.0f, true));

    // 4. تحميل أول ملف سيناريو
    ScriptLoader::loadScript("assets/script.json");

    // 5. تهيئة نظام المطرة (Rain System)
    for (int i = 0; i < RAIN_COUNT; i++) {
        RainDrop d;
        d.x = (rand() % 40) - 20.0f;
        d.y = (rand() % 22) - 11.0f;
        d.speed = 0.1f + (rand() % 10) / 100.0f;
        rainSystem.push_back(d);
    }
}