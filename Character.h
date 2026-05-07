#pragma once
#include "GraphicsUtils.h"

class Character
{
public:
    std::string name;
    GLuint textureID;

    // دول المتغيرات اللي الكومبايلر كان بيعيط عشان مش لاقيهم!
    float targetX, currentX, y, w, h;
    bool flip;

    Character(std::string n, const char *path, float _x, float _y, float _w, float _h, bool f = false)
        : name(n), targetX(_x), y(_y), w(_w), h(_h), flip(f)
    {

        currentX = (_x > 0) ? 25.0f : -25.0f;
        textureID = loadTexture(path);
    }

    void update()
    {
        currentX += (targetX - currentX) * 0.05f;
    }

    void draw(const std::string& currentSpeaker) {
        update(); 
        
        glPushMatrix(); 
        
        float centerX = currentX + w / 2.0f;
        float centerY = y + h / 2.0f;
        
        glTranslatef(centerX, centerY, 0.0f); 
        
        if (currentSpeaker == name) {
            glColor3f(1.0f, 1.0f, 1.0f); 
            glScalef(1.08f, 1.08f, 1.0f); 
        } else {
            glColor3f(0.4f, 0.4f, 0.4f); 
            glScalef(0.95f, 0.95f, 1.0f); 
        }
        
        glTranslatef(-centerX, -centerY, 0.0f); 

        // === السطرين المنقذين هنا ===
        glEnable(GL_TEXTURE_2D); // بنقوله صحصح إحنا هنرسم صورة دلوقتي
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        glBegin(GL_QUADS);
            float tx1 = flip ? 1.0f : 0.0f;
            float tx2 = flip ? 0.0f : 1.0f;
            glTexCoord2f(tx1, 0.0f); glVertex2f(currentX, y);
            glTexCoord2f(tx2, 0.0f); glVertex2f(currentX + w, y);
            glTexCoord2f(tx2, 1.0f); glVertex2f(currentX + w, y + h);
            glTexCoord2f(tx1, 1.0f); glVertex2f(currentX, y + h);
        glEnd();
        
        glDisable(GL_TEXTURE_2D); // نقفلها تاني عشان ما نبوظش باقي المشهد
        // ============================
        
        glPopMatrix(); 
    }
};