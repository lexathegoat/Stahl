#include "headers.h"

// Kernel global değişkenleri
static char cmd_buffer[256];
static int cmd_index = 0;

// Built-in komutlar
void cmd_help() {
    kprint("DivineOS Komutlar:\n");
    kprint("  help     - Bu yardim mesajini goster\n");
    kprint("  clear    - Ekrani temizle\n");
    kprint("  mem      - Bellek durumunu goster\n");
    kprint("  tasks    - Caliskan task'lari listele\n");
    kprint("  chaos    - Kaos modu (rastgele grafikler)\n");
    kprint("  plasma   - Plasma efekti\n");
    kprint("  mandel   - Mandelbrot fractal\n");
    kprint("  spiral   - Spiral animasyon\n");
    kprint("  reboot   - Sistemi yeniden baslat\n");
}

void cmd_clear() {
    clear_screen();
}

void cmd_mem() {
    uint32_t total = get_total_memory();
    uint32_t used = get_used_memory();
    uint32_t free = total - used;
    
    kprint("=== Bellek Durumu ===\n");
    kprint("Toplam: ");
    kprint_dec(total / 1024);
    kprint(" KB\n");
    kprint("Kullanilan: ");
    kprint_dec(used / 1024);
    kprint(" KB\n");
    kprint("Bos: ");
    kprint_dec(free / 1024);
    kprint(" KB\n");
}

void cmd_tasks() {
    kprint("=== Caliskan Task'lar ===\n");
    list_tasks();
}

void cmd_chaos() {
    kprint("Kaos modu baslatiliyor...\n");
    // Rastgele piksel efekti
    for(int i = 0; i < 10000; i++) {
        int x = rand() % 320;
        int y = rand() % 200;
        int color = rand() % 256;
        plot_pixel(x, y, color);
    }
}

void cmd_plasma() {
    kprint("Plasma efekti baslatiliyor...\n");
    for(int y = 0; y < 200; y++) {
        for(int x = 0; x < 320; x++) {
            int color = (int)(128 + 127 * sin(x / 16.0)) % 256;
            plot_pixel(x, y, color);
        }
    }
}

void cmd_mandelbrot() {
    kprint("Mandelbrot fractal hesaplaniyor...\n");
    for(int py = 0; py < 200; py++) {
        for(int px = 0; px < 320; px++) {
            float x0 = (px - 160.0f) / 80.0f;
            float y0 = (py - 100.0f) / 80.0f;
            float x = 0, y = 0;
            int iteration = 0;
            int max_iteration = 100;
            
            while(x*x + y*y <= 4 && iteration < max_iteration) {
                float xtemp = x*x - y*y + x0;
                y = 2*x*y + y0;
                x = xtemp;
                iteration++;
            }
            
            int color = iteration * 255 / max_iteration;
            plot_pixel(px, py, color);
        }
    }
}

void cmd_spiral() {
    kprint("Spiral ciziliyor...\n");
    float angle = 0;
    float radius = 1;
    
    for(int i = 0; i < 1000; i++) {
        int x = 160 + (int)(radius * cos(angle));
        int y = 100 + (int)(radius * sin(angle));
        
        if(x >= 0 && x < 320 && y >= 0 && y < 200) {
            plot_pixel(x, y, (i % 256));
        }
        
        angle += 0.1f;
        radius += 0.1f;
    }
}

void cmd_reboot() {
    kprint("Sistem yeniden baslatiliyor...\n");
    // Keyboard controller üzerinden reboot
    uint8_t temp;
    asm volatile("cli");
    do {
        temp = inb(0x64);
        if(temp & 0x01)
            inb(0x60);
    } while(temp & 0x02);
    outb(0x64, 0xFE);
    asm volatile("hlt");
}

// Komut işleyici
void process_command(char* cmd) {
    if(strcmp(cmd, "help") == 0) {
        cmd_help();
    } else if(strcmp(cmd, "clear") == 0) {
        cmd_clear();
    } else if(strcmp(cmd, "mem") == 0) {
        cmd_mem();
    } else if(strcmp(cmd, "tasks") == 0) {
        cmd_tasks();
    } else if(strcmp(cmd, "chaos") == 0) {
        cmd_chaos();
    } else if(strcmp(cmd, "plasma") == 0) {
        cmd_plasma();
    } else if(strcmp(cmd, "mandel") == 0) {
        cmd_mandelbrot();
    } else if(strcmp(cmd, "spiral") == 0) {
        cmd_spiral();
    } else if(strcmp(cmd, "reboot") == 0) {
        cmd_reboot();
    } else if(strlen(cmd) > 0) {
        kprint("Bilinmeyen komut: ");
        kprint(cmd);
        kprint("\n'help' yazarak komutlari gorebilirsiniz.\n");
    }
}

// Kernel ana fonksiyonu
void kernel_main() {
    // Ekranı başlat
    init_screen();
    
    // Banner göster
    set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    kprint("========================================\n");
    kprint("       DivineOS v1.0 - Sacred Code      \n");
    kprint("========================================\n");
    set_color(COLOR_WHITE, COLOR_BLACK);
    kprint("\n");
    
    // Bellek yöneticisini başlat
    init_memory();
    kprint("[OK] Bellek yoneticisi baslatildi\n");
    
    // Klavye sürücüsünü başlat
    init_keyboard();
    kprint("[OK] Klavye surucu baslatildi\n");
    
    // Task scheduler'ı başlat
    init_scheduler();
    kprint("[OK] Task scheduler baslatildi\n");
    
    kprint("\n");
    set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
    kprint("Hosgeldiniz! 'help' yazarak baslayabilirsiniz.\n");
    set_color(COLOR_WHITE, COLOR_BLACK);
    kprint("\n");
    
    // Ana komut döngüsü
    while(1) {
        kprint("divine> ");
        cmd_index = 0;
        
        // Kullanıcıdan komut al
        while(1) {
            char c = get_key();
            
            if(c == '\n') {
                kprint("\n");
                cmd_buffer[cmd_index] = '\0';
                break;
            } else if(c == '\b') {
                if(cmd_index > 0) {
                    cmd_index--;
                    kprint_backspace();
                }
            } else if(c >= 32 && c <= 126) {
                if(cmd_index < 255) {
                    cmd_buffer[cmd_index++] = c;
                    kprint_char(c);
                }
            }
        }
        
        // Komutu işle
        process_command(cmd_buffer);
    }
}

// Port I/O fonksiyonları
uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Basit string fonksiyonları
int strlen(const char* str) {
    int len = 0;
    while(str[len]) len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// Basit matematik fonksiyonları
float sin(float x) {
    float result = x;
    float term = x;
    for(int i = 1; i < 10; i++) {
        term *= -x * x / ((2 * i) * (2 * i + 1));
        result += term;
    }
    return result;
}

float cos(float x) {
    return sin(x + 1.5708f);
}

// Rastgele sayı üreteci (LCG)
static uint32_t rand_seed = 12345;

int rand() {
    rand_seed = rand_seed * 1103515245 + 12345;
    return (rand_seed / 65536) % 32768;
}