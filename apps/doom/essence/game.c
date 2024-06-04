#include "include.h"

#include "../config.h"

#include "../doomkeys.h"
#include "../d_main.h"
#include "../m_argv.h"

EsInstance *instance = 0;
pixel_t *ES_ScreenBuffer = 0;

typedef struct {
    uint8_t value;
    bool pressed;
} KeyEntry;
#define KEY_QUEUE_SIZE 64

static KeyEntry KeyQueue[KEY_QUEUE_SIZE] = { 0 };
static size_t KeyQueueHead = 0;
static size_t KeyQueueTail = 0;

uint8_t ES_TranslateKey(EsScancode scancode) {
    switch (scancode) {
    case ES_SCANCODE_ENTER: return KEY_ENTER;
    case ES_SCANCODE_ESCAPE: return KEY_ESCAPE;
    case ES_SCANCODE_LEFT_ARROW: return KEY_LEFTARROW;
    case ES_SCANCODE_RIGHT_ARROW: return KEY_RIGHTARROW;
    case ES_SCANCODE_UP_ARROW: return KEY_UPARROW;
    case ES_SCANCODE_DOWN_ARROW: return KEY_DOWNARROW;
    case ES_SCANCODE_LEFT_CTRL:
    case ES_SCANCODE_RIGHT_CTRL:
        return KEY_FIRE;
    case ES_SCANCODE_SPACE: return KEY_USE;
    case ES_SCANCODE_LEFT_SHIFT:
    case ES_SCANCODE_RIGHT_SHIFT:
        return KEY_RSHIFT;
    case ES_SCANCODE_LEFT_ALT:
    case ES_SCANCODE_RIGHT_ALT:
        return KEY_LALT;
    case ES_SCANCODE_TAB:
        return KEY_TAB;
    case ES_SCANCODE_F2: return KEY_F2;
    case ES_SCANCODE_F3: return KEY_F3;
    case ES_SCANCODE_F4: return KEY_F4;
    case ES_SCANCODE_F5: return KEY_F5;
    case ES_SCANCODE_F6: return KEY_F6;
    case ES_SCANCODE_F7: return KEY_F7;
    case ES_SCANCODE_F8: return KEY_F8;
    case ES_SCANCODE_F9: return KEY_F9;
    case ES_SCANCODE_F10: return KEY_F10;
    case ES_SCANCODE_F11: return KEY_F11;
    case ES_SCANCODE_EQUALS: return KEY_EQUALS;
    case ES_SCANCODE_LEFT_BRACE:
    case ES_SCANCODE_RIGHT_BRACE:
        return KEY_MINUS;
    case ES_SCANCODE_0: return '0';
    case ES_SCANCODE_1: return '1';
    case ES_SCANCODE_2: return '2';
    case ES_SCANCODE_3: return '3';
    case ES_SCANCODE_4: return '4';
    case ES_SCANCODE_5: return '5';
    case ES_SCANCODE_6: return '6';
    case ES_SCANCODE_7: return '7';
    case ES_SCANCODE_8: return '8';
    case ES_SCANCODE_9: return '9';
    default:
    }

    if (ES_SCANCODE_A <= scancode && scancode <= ES_SCANCODE_Z) {
        return scancode + ('a' - ES_SCANCODE_A);
    }

    return 0;
}

uint8_t ES_GetKey(int *pressed, unsigned char *key) {
    if (KeyQueueHead == KeyQueueTail) return 0;

    KeyEntry entry = KeyQueue[KeyQueueHead];
    KeyQueueHead = (KeyQueueHead + 1) % KEY_QUEUE_SIZE;

    *key = entry.value;
    *pressed = entry.pressed;

    return 1;
}

uint32_t ES_GetTicksMs(void) {
    return EsTimeStampMs();
}

void ES_SleepMs(uint32_t ms) {
    EsSleep(ms);
}

int GameMessage(EsElement *element, EsMessage *message) {
    switch (message->type) {
    case ES_MSG_ANIMATE: {
        D_DoomTick();

        EsElementRepaint(element, ES_NULL);

        message->animate.complete = false;

        return ES_HANDLED;
    }

    case ES_MSG_PAINT: {
        EsPainter *painter = message->painter;

        EsDrawRectangle(painter, ES_RECT_4(0, RES_X, 0, RES_Y), 0x000000FF, 0, ES_RECT_1(0));
        EsDrawBitmap(painter, ES_RECT_4(0, RES_X, 0, RES_Y), ES_ScreenBuffer, RES_X * 4, ES_DRAW_BITMAP_OPAQUE);

        return ES_HANDLED;
    }

    case ES_MSG_GET_WIDTH: {
        message->measure.width = RES_X;
        
        return ES_HANDLED;
    }

    case ES_MSG_GET_HEIGHT: {
        message->measure.height = RES_Y;

        return ES_HANDLED;
    }

    case ES_MSG_KEY_UP:
    case ES_MSG_KEY_DOWN: {
        uint8_t key = ES_TranslateKey(message->keyboard.scancode);
        if (key == 0) return ES_HANDLED;

        KeyQueue[KeyQueueTail] = (KeyEntry) {
            .value = key,
            .pressed = message->type == ES_MSG_KEY_DOWN
        };

        KeyQueueTail = (KeyQueueTail + 1) % KEY_QUEUE_SIZE;

        return ES_HANDLED;
    }

    default:
        return 0;
    }
}

int ApplicationMessage(EsMessage *message) {
    switch (message->type) {
    case ES_MSG_INSTANCE_CREATE: {
        instance = EsInstanceCreate(message, "DOOM", -1);
        EsWindowSetIcon(instance->window, ES_ICON_APPLICATIONS_GAMES);

        EsPanel *panel = EsPanelCreate(instance->window, ES_CELL_FILL, 0);
        EsSpacerCreate(panel, ES_CELL_CENTER, 0, 0, 0);

        EsElement *game = EsCustomElementCreate(panel, ES_ELEMENT_FOCUSABLE, 0);
        game->messageUser = GameMessage;

        ES_Init();

        EsElementFocus(game, ES_FLAGS_DEFAULT);
        EsElementStartAnimating(game);

        return ES_HANDLED;
    }

    default:
        return ES_HANDLED;
    }
}

static char *ES_ARGV[] = { "doom" };
static int ES_ARGC = sizeof(ES_ARGV) / sizeof(char *);

void ES_Init(void) {
    myargc = ES_ARGC;
    myargv = ES_ARGV;

	ES_ScreenBuffer = ES_malloc(RES_X * RES_Y * 4);

	D_DoomMain();
}

void ES_Loop(void) {
	while (true) ApplicationMessage(EsMessageReceive());
}

void ES_Crash(const char *message) {
    ES_crashf("%s\n", message);

    EsPanic("");
}

void ES_Exit() {
    if (instance) EsInstanceClose(instance);

    EsProcessTerminate(ES_CURRENT_PROCESS, 0);
}
