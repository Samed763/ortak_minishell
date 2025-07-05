#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main() {
    char filepath[256];
    int result;

    // Kullanıcıdan dosya yolu al
    printf("Dosya yolunu girin (örn: /bin/ls): ");
    scanf("%255s", filepath);  // 255 karakterle sınırla (güvenlik için)

    // 1. Dosya var mı? (F_OK)
    result = access(filepath, F_OK);
    if (result == 0) {
        printf("✅ Dosya bulundu: %s\n", filepath);
    } else {
        perror("❌ Dosya yok veya erişilemiyor");
        return 1;  // Hata durumunda çık
    }

    // 2. Okuma izni var mı? (R_OK)
    result = access(filepath, R_OK);
    printf(result == 0 ? "📖 Okunabilir\n" : "🔒 Okuma izni yok\n");

    // 3. Yazma izni var mı? (W_OK)
    result = access(filepath, W_OK);
    printf(result == 0 ? "✏️ Yazılabilir\n" : "🔒 Yazma izni yok\n");

    // 4. Çalıştırma izni var mı? (X_OK)
    result = access(filepath, X_OK);
    printf(result == 0 ? "🚀 Çalıştırılabilir\n" : "🔒 Çalıştırma izni yok\n");

    return 0;
}