// Nintendo Switch 2 support code for LOVE: start-up, the 120 Hz request, the Joy-Con mouse and the touch screen.
// The Joy-Con mouse and the touch screen are read here and handed to SDL as events, because SDL's Switch backend does
// not deliver them on the Switch 2.
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#ifdef __SWITCH__
#include <SDL.h>
#include <switch.h>

// Calls LOVE makes that newlib does not provide. love.system.openURL cannot work on the console anyway.
int posix_spawnp(pid_t *pid, const char *file, const void *fa, const void *attr, char *const argv[], char *const envp[])
{
	(void)pid; (void)file; (void)fa; (void)attr; (void)argv; (void)envp;
	return ENOSYS;
}

pid_t waitpid(pid_t pid, int *status, int options)
{
	(void)pid; (void)status; (void)options;
	errno = ECHILD;
	return -1;
}

// Runs first in main(): the surface size, a log file, and the directories LOVE derives its save folder from.
void switch_s2_early(void)
{
	// 1920x1080 in handheld and docked mode, the system scales it on a TV
	nwindowSetDimensions(nwindowGetDefault(), 1920, 1080);
	SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
	freopen("sdmc:/switch/balatro/log.txt", "w", stdout);
	freopen("sdmc:/switch/balatro/log.txt", "a", stderr);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setenv("HOME", "/switch/balatro/save", 1);
	setenv("XDG_DATA_HOME", "/switch/balatro/save", 1);
	setenv("XDG_CONFIG_HOME", "/switch/balatro/save", 1);
}

// Asks the display for its highest refresh rate (120 Hz in handheld mode).
void switch_s2_set_frame_rates(void)
{
	nwindowSetFrameRates(nwindowGetDefault(), 0, 0, true);
}

// The last position of the pointer or the touch, which love.mouse.getPosition returns.
static int last_x, last_y, position_known;

int switch_s2_touch_pos(int *x, int *y)
{
	if (!position_known) {
		return 0;
	}
	*x = last_x;
	*y = last_y;
	return 1;
}

// Joy-Con mouse: libounce's PadPointer follows the Joy-Con that slides on a surface.
static PadState ptr_pad;
static PadPointer ptr_state;
static int ptr_ready;   // 0 not tried, 1 ready, -1 not available
static int ptr_active;
static unsigned ptr_old_buttons;
static int ptr_old_x = -1, ptr_old_y = -1;

// Non-zero while a Joy-Con is in use as a mouse.
int switch_s2_pointer_active(void)
{
	return ptr_active;
}

static void pointer_poll(void)
{
	if (ptr_ready == 0) {
		padInitializeAny(&ptr_pad);
		ptr_ready = R_SUCCEEDED(padPointerInitialize(&ptr_state, 1920, 1080)) ? 1 : -1;
	}
	if (ptr_ready < 0) {
		return;
	}

	padUpdate(&ptr_pad);
	padPointerUpdate(&ptr_state, &ptr_pad);
	ptr_active = ptr_state.active ? 1 : 0;
	if (!ptr_state.active) {
		ptr_old_buttons = 0;
		ptr_old_x = ptr_old_y = -1;
		return;
	}

	int x = ptr_state.x, y = ptr_state.y;
	unsigned b = padPointerGetButtons(&ptr_state);
	SDL_Event m;

	if (x != ptr_old_x || y != ptr_old_y) {
		SDL_zero(m);
		m.type = SDL_MOUSEMOTION;
		m.motion.state = ((b & 1) ? SDL_BUTTON_LMASK : 0) | ((b & 2) ? SDL_BUTTON_RMASK : 0);
		m.motion.x = x;
		m.motion.y = y;
		m.motion.xrel = ptr_old_x < 0 ? 0 : x - ptr_old_x;
		m.motion.yrel = ptr_old_y < 0 ? 0 : y - ptr_old_y;
		SDL_PushEvent(&m);
		ptr_old_x = x;
		ptr_old_y = y;
	}

	unsigned changed = b ^ ptr_old_buttons;
	for (int bit = 0; bit < 2; bit++) {
		if (changed & (1u << bit)) {
			SDL_zero(m);
			m.type = (b & (1u << bit)) ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
			m.button.button = bit == 0 ? SDL_BUTTON_LEFT : SDL_BUTTON_RIGHT;
			m.button.state = (b & (1u << bit)) ? SDL_PRESSED : SDL_RELEASED;
			m.button.clicks = 1;
			m.button.x = x;
			m.button.y = y;
			SDL_PushEvent(&m);
		}
	}
	ptr_old_buttons = b;

	last_x = x;
	last_y = y;
	position_known = 1;
}

// Touch screen: the raw touches (a 1280x720 area) become finger events with the device id 7, which LOVE reads,
// and the first finger also acts as the left mouse button, the way SDL does it for touch screens.
static void touch_poll(void)
{
	static int started;
	static struct { unsigned id; float x, y; } prev[16];
	static int nprev;

	if (!started) {
		hidInitializeTouchScreen();
		started = 1;
	}

	HidTouchScreenState st;
	if (hidGetTouchScreenStates(&st, 1) < 1) {
		return;
	}

	int count = st.count > 16 ? 16 : st.count;
	struct { unsigned id; float x, y; } cur[16];
	SDL_Event e;
	for (int i = 0; i < count; i++) {
		cur[i].id = st.touches[i].finger_id;
		cur[i].x = st.touches[i].x / 1280.0f;
		cur[i].y = st.touches[i].y / 720.0f;
		int was = -1;
		for (int j = 0; j < nprev; j++) {
			if (prev[j].id == cur[i].id) {
				was = j;
				break;
			}
		}
		SDL_zero(e);
		e.tfinger.touchId = 7;
		e.tfinger.fingerId = cur[i].id;
		e.tfinger.x = cur[i].x;
		e.tfinger.y = cur[i].y;
		e.tfinger.pressure = 1.0f;
		if (was < 0) {
			e.type = SDL_FINGERDOWN;
		} else {
			e.type = SDL_FINGERMOTION;
			e.tfinger.dx = cur[i].x - prev[was].x;
			e.tfinger.dy = cur[i].y - prev[was].y;
		}
		SDL_PushEvent(&e);

		if (i == 0) {
			int px = (int)(cur[i].x * 1920.0f), py = (int)(cur[i].y * 1080.0f);
			SDL_Event m;
			SDL_zero(m);
			m.type = SDL_MOUSEMOTION;
			m.motion.which = SDL_TOUCH_MOUSEID;
			m.motion.state = was < 0 ? 0 : SDL_BUTTON_LMASK;
			m.motion.x = px;
			m.motion.y = py;
			m.motion.xrel = was < 0 ? 0 : (int)((cur[i].x - prev[was].x) * 1920.0f);
			m.motion.yrel = was < 0 ? 0 : (int)((cur[i].y - prev[was].y) * 1080.0f);
			SDL_PushEvent(&m);
			if (was < 0) {
				SDL_zero(m);
				m.type = SDL_MOUSEBUTTONDOWN;
				m.button.which = SDL_TOUCH_MOUSEID;
				m.button.button = SDL_BUTTON_LEFT;
				m.button.state = SDL_PRESSED;
				m.button.clicks = 1;
				m.button.x = px;
				m.button.y = py;
				SDL_PushEvent(&m);
			}
			last_x = px;
			last_y = py;
			position_known = 1;
		}
	}

	for (int j = 0; j < nprev; j++) {
		int still = 0;
		for (int i = 0; i < count; i++) {
			if (cur[i].id == prev[j].id) {
				still = 1;
				break;
			}
		}
		if (still) {
			continue;
		}
		SDL_zero(e);
		e.type = SDL_FINGERUP;
		e.tfinger.touchId = 7;
		e.tfinger.fingerId = prev[j].id;
		e.tfinger.x = prev[j].x;
		e.tfinger.y = prev[j].y;
		SDL_PushEvent(&e);
		if (j == 0) {
			SDL_Event m;
			SDL_zero(m);
			m.type = SDL_MOUSEBUTTONUP;
			m.button.which = SDL_TOUCH_MOUSEID;
			m.button.button = SDL_BUTTON_LEFT;
			m.button.state = SDL_RELEASED;
			m.button.clicks = 1;
			m.button.x = (int)(prev[j].x * 1920.0f);
			m.button.y = (int)(prev[j].y * 1080.0f);
			SDL_PushEvent(&m);
		}
	}

	nprev = count;
	for (int i = 0; i < count; i++) {
		prev[i].id = cur[i].id;
		prev[i].x = cur[i].x;
		prev[i].y = cur[i].y;
	}
}

// Called once per event pump.
void switch_s2_touch_poll(void)
{
	pointer_poll();
	touch_poll();
}
#endif
