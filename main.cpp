#define STB_IMAGE_IMPLEMENTATION
#include "Character.h"

struct DialogueLine
{
    std::string speaker, text;
    float r, g, b;
    bool isChoice = false;
    std::string opt1, file1;
    std::string opt2, file2;
};

enum GameState
{
    MENU,
    PLAYING
};
GameState currentState = MENU; // اللعبة تبدأ بالقائمة الرئيسية

std::vector<Character *> cast;
std::vector<DialogueLine> script;
float fadeAlpha = 1.0f;
float flashAlpha = 0.0f;
float shakeIntensity = 0.0f;
GLuint bgTexture;
int currentLine = 0;
struct RainDrop
{
    float x, y;  // مكان النقطة
    float speed; // سرعتها وهي نازلة
};

std::vector<RainDrop> rainSystem;
const int RAIN_COUNT = 100; // عدد نقط المطرة

int displayedChars = 0;    // عدد الحروف المعروضة حالياً
int lastTypeTime = 0;      // توقيت آخر حرف اتكتب
const int TYPE_SPEED = 30; // السرعة (كل 30 مللي ثانية حرف)
void loadScript(const std::string &filename)
{
    script.clear();
    currentLine = 0;
    std::ifstream file(filename);
    if (file.is_open())
    {
        json j;
        file >> j;
        for (auto &item : j)
        {
            DialogueLine line;
            line.speaker = item.value("speaker", "");
            line.text = item.value("text", "");
            if (item.contains("color"))
            {
                line.r = item["color"][0];
                line.g = item["color"][1];
                line.b = item["color"][2];
            }
            else
            {
                line.r = 1.0f;
                line.g = 1.0f;
                line.b = 1.0f;
            }
            line.isChoice = item.value("is_choice", false);
            if (line.isChoice)
            {
                line.opt1 = item.value("opt1", "");
                line.file1 = item.value("file1", "");
                line.opt2 = item.value("opt2", "");
                line.file2 = item.value("file2", "");
            }
            script.push_back(line);
        }
    }
}

void init()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // ضبط الكاميرا على المقاس الواسع الجديد
    gluOrtho2D(-20.0, 20.0, -11.25, 11.25);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    bgTexture = loadTexture("assets/background.png");

    // التعديل: رفعنا الـ Y من -9.0 لـ -5.0 عشان يظهروا فوق الصندوق
    cast.push_back(new Character("Magdy Khaled", "assets/character.png", -18.5f, -5.0f, 7.0f, 10.0f));
    cast.push_back(new Character("Donia Ahamed", "assets/character2.png", -11.0f, -5.0f, 7.0f, 10.0f));
    cast.push_back(new Character("Amr Reda", "assets/character3.png", -3.5f, -5.0f, 7.0f, 10.0f));
    cast.push_back(new Character("Israa Ahmed", "assets/character4.png", 4.0f, -5.0f, 7.0f, 10.0f, true));
    cast.push_back(new Character("Manna", "assets/character5.png", 11.5f, -5.0f, 7.0f, 10.0f, true));
    loadScript("assets/script.json");
    // ضيف ده في آخر دالة init
    for (int i = 0; i < RAIN_COUNT; i++)
    {
        RainDrop d;
        d.x = (rand() % 40) - 20.0f;             // توزيع عشوائي على عرض الشاشة (-20 لـ 20)
        d.y = (rand() % 22) - 11.0f;             // توزيع عشوائي على الارتفاع
        d.speed = 0.1f + (rand() % 10) / 100.0f; // سرعات متفاوتة
        rainSystem.push_back(d);
    }
}

void drawMenu()
{
    // 1. رسم الخلفية (ممكن تستخدم نفس الـ bgTexture)
    glEnable(GL_TEXTURE_2D);
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

    // 2. طبقة تعتيم خفيفة عشان النص يظهر
    glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(-20.0f, -11.25f);
    glVertex2f(20.0f, -11.25f);
    glVertex2f(20.0f, 11.25f);
    glVertex2f(-20.0f, 11.25f);
    glEnd();

    // 3. كتابة نصوص القائمة
    glColor3f(1.0f, 0.84f, 0.0f); // لون ذهبي
    renderText(-6.5f, 2.0f, "AMRO ENGINE: ADVENTURE PRO");

    glColor3f(1.0f, 1.0f, 1.0f); // أبيض للخيارات
    renderText(-4.5f, -1.0f, "[ S ]  START ADVENTURE");
    renderText(-4.5f, -3.0f, "[ E ]  EXIT ENGINE");

    glColor3f(0.6f, 0.6f, 0.6f);
    renderText(-9.0f, -9.0f, "Developed by: Amr Reda Mohamed"); //
}

void updateRain()
{
    for (int i = 0; i < rainSystem.size(); i++)
    {
        rainSystem[i].y -= rainSystem[i].speed; // تنزل لتحت

        // لو النقطة خرجت من الشاشة تحت، ترجع تظهر فوق تاني
        if (rainSystem[i].y < -11.25f)
        {
            rainSystem[i].y = 11.25f;
            rainSystem[i].x = (rand() % 40) - 20.0f;
        }
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    if (currentState == MENU)
    {
        drawMenu();
    }
    else if (currentState == PLAYING)
    {
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
void keyboard(unsigned char key, int x, int y)
{
    // --- 1. حالة القائمة الرئيسية (Main Menu) ---
    if (currentState == MENU)
    {
        if (key == 's' || key == 'S')
        {
            // تشغيل صوت وبدء المغامرة
            PlaySound(TEXT(".\\assets\\blip.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
            currentState = PLAYING;
            fadeAlpha = 1.0f; // تأثير دخول ناعم
            std::cout << "Amro Engine: Adventure Started!" << std::endl;
        }
        else if (key == 'e' || key == 'E' || key == 27)
        { // الخروج بـ E أو ESC
            std::cout << "Amro Engine: Closing safely..." << std::endl;
            exit(0);
        }
    }

    // --- 2. حالة اللعب (Playing State) ---
    else if (currentState == PLAYING)
    {

        // أ- لو الحوار خلص وعاوزين نعيد (Restart)
        if (currentLine >= script.size())
        {
            if (key == 'r' || key == 'R')
            {
                PlaySound(TEXT(".\\assets\\blip.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
                fadeAlpha = 1.0f;
                currentLine = 0;                  // تصفير العداد
                loadScript("assets/script.json"); // العودة للبداية
                std::cout << "Amro Engine: Restarting..." << std::endl;
            }
        }

        // ب- لو إحنا لسه جوه الحوارات
        else
        {
            // نظام الاختيارات (Branching)
            if (script[currentLine].isChoice)
            {
                if (key == '1')
                {
                    PlaySound(TEXT(".\\assets\\blip.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
                    loadScript(script[currentLine].file1);
                    displayedChars = 0;
lastTypeTime = glutGet(GLUT_ELAPSED_TIME);
                }
                else if (key == '2')
                {
                    PlaySound(TEXT(".\\assets\\blip.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
                    loadScript(script[currentLine].file2);
                    displayedChars = 0;
lastTypeTime = glutGet(GLUT_ELAPSED_TIME);
                }
            }
            // نظام المسطرة للحوار العادي
            else if (key == ' ')
            {
                PlaySound(TEXT(".\\assets\\blip.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

                currentLine++;     // السطر التالي
                flashAlpha = 0.3f; // وميض أبيض خفيف
                                   // ضيف السطر ده جوه اف المسطرة واف الاختيارات 1 و 2
                displayedChars = 0;
                lastTypeTime = glutGet(GLUT_ELAPSED_TIME);
                // لوجيك الاهتزاز (Shake) لو فيه انفعال في النص
                if (currentLine < script.size())
                {
                    std::string text = script[currentLine].text;
                    if (text.find("!") != std::string::npos || text.find("?") != std::string::npos)
                    {
                        shakeIntensity = 0.5f;
                    }
                }
            }
        }

        // ج- زر ESC للخروج أثناء اللعب
        if (key == 27)
        {
            exit(0);
        }
    }

    // تحديث الشاشة بعد أي ضغطة زرار
    glutPostRedisplay();
}
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1280, 720);
    // مقاس HD مضبوط
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Amro Engine - Adventure Pro Edition");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(display); // بيخلي الشاشة تعمل ريفريش طول ما الجهاز مش مشغول
    glutMainLoop();
    return 0;
}