#include "include/DisplayHandler.h"
#include "include/Globals.h"
#include "include/Character.h"
#include <cmath>

// إعلام الملف بوجود دالة الرندر اللي في المين أو في ملف مساعد
extern void renderText(float x, float y, const char* text);
extern std::vector<Character*> cast;
extern GLuint bgTexture;

void DisplayHandler::updateRain() {
    for (int i = 0; i < rainSystem.size(); i++) {
        rainSystem[i].y -= rainSystem[i].speed;
        if (rainSystem[i].y < -11.25f) {
            rainSystem[i].y = 11.25f;
            rainSystem[i].x = (rand() % 40) - 20.0f;
        }
    }
}

void DisplayHandler::drawMenu() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bgTexture);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(-20.0f, -11.25f);
        glTexCoord2f(1, 0); glVertex2f(20.0f, -11.25f);
        glTexCoord2f(1, 1); glVertex2f(20.0f, 11.25f);
        glTexCoord2f(0, 1); glVertex2f(-20.0f, 11.25f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
    glBegin(GL_QUADS);
        glVertex2f(-20.0f, -11.25f); glVertex2f(20.0f, -11.25f);
        glVertex2f(20.0f, 11.25f); glVertex2f(-20.0f, 11.25f);
    glEnd();

    glColor3f(1.0f, 0.84f, 0.0f);
    renderText(-6.5f, 2.0f, "AMRO ENGINE: ADVENTURE PRO");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(-4.5f, -1.0f, "[ S ]  START ADVENTURE");
    renderText(-4.5f, -3.0f, "[ E ]  EXIT ENGINE");
    glColor3f(0.6f, 0.6f, 0.6f);
    renderText(-9.0f, -9.0f, "Developed by: Amr Reda Mohamed");
}

void DisplayHandler::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (currentState == MENU) {
        drawMenu();
    } 
    else if (currentState == PLAYING) {
        glPushMatrix();
        
        
        if (shakeIntensity > 0.0f)
        {
            float shakeX = ((rand() % 100) / 100.0f - 0.5f) * shakeIntensity;
            float shakeY = ((rand() % 100) / 100.0f - 0.5f) * shakeIntensity;
            glTranslatef(shakeX, shakeY, 0.0f);
            shakeIntensity -= 0.015f;
            if (shakeIntensity < 0.0f)
                shakeIntensity = 0.0f;
        }

        static float timer = 0.0f;
        timer += 0.015f;
        float breathing = sin(timer) * 0.2f;

        // 1. رسم الخلفية (تم تعديلها لتملأ الـ 20 بالكامل)
        if (bgTexture != 0)
        {
            glEnable(GL_TEXTURE_2D);
            glColor3f(1.0f, 1.0f, 1.0f);
            glBindTexture(GL_TEXTURE_2D, bgTexture);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex2f(-20.0f, -11.25f);
            glTexCoord2f(1, 0);
            glVertex2f(20.0f, -11.25f);
            glTexCoord2f(1, 1);
            glVertex2f(20.0f, 11.25f);
            glTexCoord2f(0, 1);
            glVertex2f(-20.0f, 11.25f);
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }

        // 2. رسم الشخصيات
        std::string speaker = (currentLine < script.size()) ? script[currentLine].speaker : "";
        for (auto c : cast)
        {
            float originalY = c->y;
            if (c->name == speaker)
            {
                c->y += breathing;
            }
            c->draw(speaker);
            c->y = originalY;
        }

        // رسم المطرة (يفضل قبل الـ UI)
        glLineWidth(1.0f);
        glColor4f(0.8f, 0.8f, 1.0f, 0.5f); // لون أزرق فاتح وشفاف شوية
        glBegin(GL_LINES);
        for (int i = 0; i < rainSystem.size(); i++)
        {
            glVertex2f(rainSystem[i].x, rainSystem[i].y);
            glVertex2f(rainSystem[i].x, rainSystem[i].y - 0.5f); // طول نقطة المطر
        }
        glEnd();

        updateRain(); // تحديث الأماكن للفريم الجاي
                      // 3. رسم صندوق الحوار (تم توسيعه ليصل لـ 19)
        glEnable(GL_BLEND);
        glColor4f(0.0f, 0.0f, 0.0f, 0.85f);
        glBegin(GL_QUADS);
        glVertex2f(-19.0f, -10.5f);
        glVertex2f(19.0f, -10.5f);
        glVertex2f(19.0f, -4.5f);
        glVertex2f(-19.0f, -4.5f);
        glEnd();

        glColor3f(0.7f, 0.7f, 0.7f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-19.0f, -10.5f);
        glVertex2f(19.0f, -10.5f);
        glVertex2f(19.0f, -4.5f);
        glVertex2f(-19.0f, -4.5f);
        glEnd();

        // --- 4. الحماية الهندسية ورسم النصوص ---
        if (currentLine < script.size())
        {
            // أ. لوجيك الآلة الكاتبة (بقى محمي جوه الـ if)
            int currentTime = glutGet(GLUT_ELAPSED_TIME);
            std::string fullText = script[currentLine].text;

            if (currentTime - lastTypeTime > TYPE_SPEED && displayedChars < fullText.length())
            {
                displayedChars++;
                lastTypeTime = currentTime;
            }
            std::string textToDisplay = fullText.substr(0, displayedChars);

            // ب. رسم النصوص حسب نوعها (حوار أو اختيار)
            if (script[currentLine].isChoice)
            {
                glColor3f(1.0f, 1.0f, 0.0f); // أصفر
                renderText(-18.0f, -5.5f, "SYSTEM (DECISION):");

                glColor3f(1.0f, 1.0f, 1.0f);                      // أبيض للنص نفسه
                renderText(-17.5f, -7.0f, textToDisplay.c_str()); // تأثير الآلة الكاتبة

                // إظهار الاختيارات فقط بعد ما النص يخلص كتابة
                if (displayedChars >= fullText.length())
                {
                    glColor3f(0.5f, 1.0f, 0.5f); // أخضر للاختيارات
                    renderText(-12.0f, -8.5f, script[currentLine].opt1);
                    renderText(-12.0f, -9.5f, script[currentLine].opt2); // إنت كنت ناسي دي
                }
            }
            else
            {
                glColor3f(script[currentLine].r, script[currentLine].g, script[currentLine].b);
                renderText(-18.0f, -5.5f, script[currentLine].speaker + ":");

                glColor3f(1.0f, 1.0f, 1.0f);
                renderText(-17.5f, -7.5f, textToDisplay.c_str()); // تأثير الآلة الكاتبة

                if (displayedChars >= fullText.length())
                {
                    glColor3f(0.4f, 0.4f, 0.4f);
                    renderText(15.0f, -9.5f, "[ SPACE ]");
                }
            }
        }
        else
        {
            // --- 5. شاشة النهاية (هنا تاخد سكرين شوت النهاية) ---
            glColor3f(1.0f, 0.84f, 0.0f); // لون ذهبي للنهاية
            renderText(-6.0f, -6.0f, "--- MISSION ACCOMPLISHED ---");

            glColor3f(1.0f, 1.0f, 1.0f);
            renderText(-10.0f, -8.0f, "Press [ R ] to Play Again  |  Press [ ESC ] to Exit");
        }

        // 6. المؤثرات البصرية (Fade & Flash)
        if (fadeAlpha > 0.0f)
        {
            glColor4f(0.0f, 0.0f, 0.0f, fadeAlpha);
            glBegin(GL_QUADS);
            glVertex2f(-20.0f, -11.25f);
            glVertex2f(20.0f, -11.25f);
            glVertex2f(20.0f, 11.25f);
            glVertex2f(-20.0f, 11.25f);
            glEnd();
            fadeAlpha -= 0.008f;
        }

        if (flashAlpha > 0.0f)
        {
            glColor4f(1.0f, 1.0f, 1.0f, flashAlpha);
            glBegin(GL_QUADS);
            glVertex2f(-20.0f, -11.25f);
            glVertex2f(20.0f, -11.25f);
            glVertex2f(20.0f, 11.25f);
            glVertex2f(-20.0f, 11.25f);
            glEnd();
            flashAlpha -= 0.02f;
        }


        glPopMatrix();
        glutPostRedisplay();
    }
    glutSwapBuffers();
}