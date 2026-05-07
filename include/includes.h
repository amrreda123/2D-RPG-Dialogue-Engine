#pragma once // السطر ده بيمنع C++ من استدعاء المكتبات مرتين بالغلط

// 1. مكتبات C++ الأساسية
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

// 2. مكتبة OpenGL للرسوميات
#include <GL/glut.h>

// 3. مكتبة JSON
#include "json.hpp"
using json = nlohmann::json;

// 4. مكتبة الصور
#include "stb_image.h"

// 5. مكتبات الصوت للويندوز
#include <windows.h>
#include <mmsystem.h>