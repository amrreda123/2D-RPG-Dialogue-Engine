#pragma once
#include "includes.h"

// دالة تحميل الصور وتحويلها لـ Textures في OpenGL
inline GLuint loadTexture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // السطر ده مهم جداً عشان يمنع تشوه الصور أو إنها تطلع بيضاء (بيظبط محاذاة البكسلات)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // إعدادات تنعيم الصور (عشان نقضي على البكسلة وتطلع بجودة عالية)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // إعدادات التكرار
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // OpenGL بيقرأ الصور من تحت لفوق، فلازم نقلبها عشان تظهر معدولة
    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    // تحميل الصورة (4 قنوات يعني RGBA عشان الشفافية)
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 4);

    if (data) {
        // بناء الـ Texture والـ Mipmaps (لتحسين الأداء والجودة)
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        std::cout << "SUCCESS: Loaded " << filename << std::endl;
    } else {
        std::cout << "ERROR: Failed to load " << filename << std::endl;
    }
    
    // مسح بيانات الصورة من الرام بعد ما رفعناها لكارت الشاشة
    stbi_image_free(data);
    
    return textureID;
}
// 1. النسخة الأولى: بتتعامل مع الـ String (عشان الحوارات وتأثير الآلة الكاتبة)
inline void renderText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

// 2. النسخة التانية: بتتعامل مع النصوص المباشرة const char* (عشان القائمة الرئيسية تحل الإيرور)
inline void renderText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        text++;
    }
}