import urllib.request
import os

# التأكد من وجود فولدر assets
if not os.path.exists('assets'):
    os.makedirs('assets')

# روابط الصور الكبيرة (Official Artwork) لأول 3 بوكيمونات
big_pokemon = {
    "assets/character.png": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/25.png", # Pikachu
    "assets/character2.png": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/4.png",  # Charmander
    "assets/character3.png": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/7.png"   # Squirtle
}

print("🚀 جاري استبدال الشخصيات الصغيرة بنسخ كبيرة عالية الجودة...")

for path, url in big_pokemon.items():
    try:
        print(f"📥 جاري تحميل نسخة كبيرة لـ {path}...")
        req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
        with urllib.request.urlopen(req) as response, open(path, 'wb') as out_file:
            out_file.write(response.read())
        print(f"✅ تم التحديث بنجاح!")
    except Exception as e:
        print(f"❌ مشكلة في {path}: {e}")

print("\n🎉 دلوقتي كل الفريق بقى بجودة عالية وحجم كبير!")