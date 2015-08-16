#include <pebble.h>

#define WAKEUP_REASON 0
#define PERSIST_KEY_WAKEUP_ID 42
#define PERSIST_START_TIME 80

static Window *s_main_window;
static Layer *s_canvas_layer;

static TextLayer *s_output_layer;
static TextLayer *status_output_layer;
static TextLayer *dev_output_layer;
static TextLayer *title_output_layer;

int barP = 0;
static int bW = 120;
static int bH = 16;	
unsigned int started_at;
unsigned int pause_time;

static bool running;
static bool paused;
static int seconds_in_hour = 3600;
static WakeupId s_wakeup_id;

static void canvas_update_proc(Layer *this_layer, GContext *ctx) {
		
	int x = (144/2) - bW/2;	
	
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, GRect(x-1, 145, bW+2, bH+2), 0, GCornerNone);
	
	graphics_context_set_fill_color(ctx, GColorDarkGray);
	graphics_fill_rect(ctx, GRect(x, 146, bW, bH), 0, GCornerNone);
	
	graphics_context_set_fill_color(ctx, GColorBlue);
	int w = (barP / 100.0F) * bW;	
	graphics_fill_rect(ctx, GRect(x, 146, w, bH), 0, GCornerNone);
	
}

static void set_status_text(char *str){
	snprintf(str, 32, "%s", str);
	text_layer_set_text(status_output_layer, str);
}

static void set_perc_text(int i){
	static char str[32];
	snprintf(str, 32, "%d%s", i, "%");
	text_layer_set_text(s_output_layer, str);
}

static void timer_finished(){
	
	window_stack_push(s_main_window, true);
	
	set_status_text("Done!");
	set_perc_text(100);
	persist_delete(PERSIST_KEY_WAKEUP_ID);
	
	barP = 100;
	running = false;
	started_at = 0;
	pause_time = 0;
	
	layer_mark_dirty(s_canvas_layer);	
	
	static const uint32_t const segments[] = {1000, 200, 1000, 200, 1000, 200};
	VibePattern pat = {
		.durations = segments,
		.num_segments = ARRAY_LENGTH(segments),
	};
	vibes_enqueue_custom_pattern(pat);
	
}

static void wakeup_handler(WakeupId id, int32_t reason) {	
	timer_finished();
}

static void tick_handler(struct tm *tick_time, TimeUnits changed){
	
	if(paused){
		pause_time++;
	}
	
	if(!running){ //dont update screen Kappa
		return;
	}
	
	float f = ((time(NULL) - pause_time) - started_at);
	barP = (int) (f / (seconds_in_hour/2) * 100);	
	
	set_perc_text(barP);
		
/*	int tim = ((time(NULL) - pause_time) - started_at);
	static char str[32];
	snprintf(str, 32, "%s%d%s%d", "S: ", tim, "\nP: ", pause_time);
	text_layer_set_text(dev_output_layer, str);*/
	
	layer_mark_dirty(s_canvas_layer);
	
	if(barP >= 100){		
		timer_finished();
	}

}

static void main_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect window_bounds = layer_get_bounds(window_layer);

	s_canvas_layer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
	layer_add_child(window_layer, s_canvas_layer);

	layer_set_update_proc(s_canvas_layer, canvas_update_proc);
	
	s_output_layer = text_layer_create(GRect(0, 90, window_bounds.size.w, 50));
	text_layer_set_text_alignment(s_output_layer, GTextAlignmentCenter);
	text_layer_set_font(s_output_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_color(s_output_layer, GColorDarkGreen);
	text_layer_set_text(s_output_layer, "");
	layer_add_child(window_layer, text_layer_get_layer(s_output_layer));
	
	status_output_layer = text_layer_create(GRect(0, 60, window_bounds.size.w, 32));
	text_layer_set_text_alignment(status_output_layer, GTextAlignmentCenter);
	text_layer_set_font(status_output_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text(status_output_layer, "");
	layer_add_child(window_layer, text_layer_get_layer(status_output_layer));	
	
	dev_output_layer = text_layer_create(GRect(0, 0, window_bounds.size.w, 64));
	text_layer_set_text_alignment(dev_output_layer, GTextAlignmentCenter);
	text_layer_set_font(dev_output_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text(dev_output_layer, "");
	layer_add_child(window_layer, text_layer_get_layer(dev_output_layer));

	title_output_layer = text_layer_create(GRect(0, 0, window_bounds.size.w, 36));
	text_layer_set_text_alignment(title_output_layer, GTextAlignmentCenter);
	text_layer_set_background_color(title_output_layer, GColorGreen);
	text_layer_set_text_color(title_output_layer, GColorOxfordBlue);	
	text_layer_set_font(title_output_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text(title_output_layer, "WorkoutC");
	layer_add_child(window_layer, text_layer_get_layer(title_output_layer));
	
	started_at = persist_read_int(PERSIST_START_TIME);	
	if(started_at > 0){		
		running = true;
		set_status_text("In Progress");
	}	
	
}

static void main_window_unload(Window *window) {
	persist_write_int(PERSIST_START_TIME, started_at);
	layer_destroy(s_canvas_layer);
}

static void toggle_pause(){
	
	if(!running && !paused){
		return;
	}
	
	running = !running;
	paused = !paused;
	
	paused ? vibes_double_pulse() : vibes_short_pulse();
	set_status_text(paused ? "Paused" : "In Progress");
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	
	if(running && !paused){
		return;
	}
	
	if (!wakeup_query(s_wakeup_id, NULL)) {
		started_at = time(NULL);
		running = true;
		time_t future_time = started_at + (seconds_in_hour/2);

		s_wakeup_id = wakeup_schedule(future_time, WAKEUP_REASON, true);
		persist_write_int(PERSIST_KEY_WAKEUP_ID, s_wakeup_id);
		
		set_status_text("In Progress");
		layer_mark_dirty(s_canvas_layer);
	}
	
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	toggle_pause();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {

	running = false;
	paused = false;
	started_at = 0;
	pause_time = 0;
	barP = 0;
	s_wakeup_id = 0;
	persist_delete(PERSIST_KEY_WAKEUP_ID);
		
	set_status_text("Stopped");
	set_perc_text(0);
	
	layer_mark_dirty(s_canvas_layer);
}


static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);	
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
}

static void init(void){
	s_main_window = window_create();
	
	window_set_background_color(s_main_window, GColorWhite);
	window_set_click_config_provider(s_main_window, click_config_provider);
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload,
	});
	
	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
	
	if (launch_reason() == APP_LAUNCH_WAKEUP) {
		WakeupId id = 0;
		int32_t reason = 0;
		if (wakeup_get_launch_event(&id, &reason)) {
			wakeup_handler(id, reason);
		}
	}	

	wakeup_service_subscribe(wakeup_handler);
	
	window_stack_push(s_main_window, true);
}

static void deinit(void) {
	// Destroy main Window
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}