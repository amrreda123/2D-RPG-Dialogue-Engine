#define STB_IMAGE_IMPLEMENTATION
#include "Globals.h"
#include "InputHandler.h"
#include "Character.h"
#include "DisplayHandler.h"
// هنا بنعرف المتغيرات "فعلياً" وبنديها قيمتها الابتدائية
GameState currentState = MENU;
std::vector<DialogueLine> script;
std::vector<Character *> cast; // دي تفضل هنا عشان الـ Character موجود في هيدر لوحده
std::vector<RainDrop> rainSystem;
GLuint bgTexture;

int currentLine = 0;
float fadeAlpha = 1.0f;
float flashAlpha = 0.0f;
float shakeIntensity = 0.0f;
int displayedChars = 0;
int lastTypeTime = 0;

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


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1280, 720);
    // مقاس HD مضبوط
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Amro Engine - Adventure Pro Edition");
    init();
    glutDisplayFunc(DisplayHandler::display);
    glutKeyboardFunc(InputHandler::keyboard);
    glutIdleFunc(DisplayHandler::display);
    glutMainLoop();
    return 0;
}