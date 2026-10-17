// mcmenu.c - mod menu do Minecraft PS3 (szkielet, tylko tryb offline)
// Wersja pod otwarty PSL1GHT. NIE przetestowane na konsoli.
// Sterowanie: L1 + SELECT = menu, góra/dół = wybór, X = zmień, O = zamknij.

#include <ppu-types.h>
#include <sys/thread.h>
#include <io/pad.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "offsets.h"

typedef enum { OPT_TOGGLE, OPT_FLOAT_CYCLE } OptType;

typedef struct {
    const char *name;
    OptType     type;
    uint32_t    addr;
    int         state;              // toggle: 0/1, float: indeks w levels[]
} Option;

static const float speed_levels[] = { 1.0f, 1.5f, 2.0f, 3.0f };
#define SPEED_COUNT (sizeof(speed_levels) / sizeof(speed_levels[0]))

static Option options[] = {
    { "Fly",      OPT_TOGGLE,      ADDR_FLY,     0 },
    { "God mode", OPT_TOGGLE,      ADDR_GODMODE, 0 },
    { "No-clip",  OPT_TOGGLE,      ADDR_NOCLIP,  0 },
    { "Speed",    OPT_FLOAT_CYCLE, ADDR_SPEED,   0 },
};
#define OPT_COUNT ((int)(sizeof(options) / sizeof(options[0])))

static volatile int running   = 1;
static int          menu_open = 0;
static int          cursor    = 0;
static sys_ppu_thread_t tid;

static int addr_ok(uint32_t a) { return a >= MEM_MIN && a < MEM_MAX; }

static void write_u8(uint32_t a, uint8_t v)  { if (addr_ok(a)) *(volatile uint8_t *)a = v; }
static void write_f32(uint32_t a, float v)   { if (addr_ok(a)) *(volatile float *)a = v; }

static void apply(const Option *o) {
    if (o->type == OPT_TOGGLE) write_u8(o->addr, (uint8_t)o->state);
    else                       write_f32(o->addr, speed_levels[o->state]);
}

// Komunikat dla gracza. TODO: podmień na powiadomienie XMB lub rysowanie
// w HUD, gdy będziesz miał działający hook. Na razie tylko log.
static void say(const char *msg) { printf("[mcmenu] %s\n", msg); }

static void show_menu(void) {
    char line[64];
    for (int i = 0; i < OPT_COUNT; i++) {
        const Option *o = &options[i];
        if (o->type == OPT_TOGGLE)
            snprintf(line, sizeof line, "%c %s: %s", i == cursor ? '>' : ' ',
                     o->name, o->state ? "ON" : "OFF");
        else
            snprintf(line, sizeof line, "%c %s: x%.1f", i == cursor ? '>' : ' ',
                     o->name, speed_levels[o->state]);
        say(line);
    }
}

// Zwraca 1 tylko w chwili wciśnięcia (nie, gdy przycisk jest przytrzymany).
#define PRESSED(now, prev) ((now) && !(prev))

static void menu_thread(void *arg) {
    (void)arg;
    padData d;
    int p_up = 0, p_down = 0, p_x = 0, p_o = 0, p_combo = 0;

    while (running) {
        // Pad jest już zainicjowany przez grę, więc tylko odczytujemy.
        if (ioPadGetData(0, &d) == 0 && d.len > 0) {
            int combo = d.BTN_L1 && d.BTN_SELECT;
            if (PRESSED(combo, p_combo)) { menu_open = !menu_open; if (menu_open) show_menu(); }

            if (menu_open) {
                int moved = 0;
                if (PRESSED(d.BTN_DOWN, p_down)) { cursor = (cursor + 1) % OPT_COUNT; moved = 1; }
                if (PRESSED(d.BTN_UP,   p_up))   { cursor = (cursor + OPT_COUNT - 1) % OPT_COUNT; moved = 1; }
                if (PRESSED(d.BTN_CROSS, p_x)) {
                    Option *o = &options[cursor];
                    if (o->type == OPT_TOGGLE) o->state = !o->state;
                    else o->state = (o->state + 1) % SPEED_COUNT;
                    apply(o);
                    moved = 1;
                }
                if (PRESSED(d.BTN_CIRCLE, p_o)) menu_open = 0;
                if (moved && menu_open) show_menu();
            }
            p_up = d.BTN_UP; p_down = d.BTN_DOWN; p_x = d.BTN_CROSS;
            p_o = d.BTN_CIRCLE; p_combo = combo;
        }

        // Gra potrafi nadpisać wartości, więc włączone opcje odświeżamy.
        for (int i = 0; i < OPT_COUNT; i++)
            if (options[i].state || options[i].type == OPT_FLOAT_CYCLE) apply(&options[i]);

        usleep(50 * 1000);
    }
    sysThreadExit(0);
}

int main(void) {
    sysThreadCreate(&tid, menu_thread, NULL, 1000, 0x2000, THREAD_JOINABLE, "mcmenu");
    u64 ret;
    sysThreadJoin(tid, &ret);
    return 0;
}
