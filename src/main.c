#include <pebble.h>
#include "main.h"

static Window *titleWindow;
static TextLayer *titleText;
static TextLayer *startText;

static void title_window_load(Window *window){
  //Display title text
  titleText = text_layer_create(GRect(0, 50, 144, 40));
  text_layer_set_background_color(titleText, GColorBlack);
  text_layer_set_text_color(titleText, GColorWhite);
  text_layer_set_text(titleText, "PuNcH!");
  text_layer_set_font(titleText, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(titleText, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(titleText));
  
  //Display Select to Play text
  startTest = text_layer_create(GRect(0, 100, 144, 35));
  text_layer_set_background_color(startText, GColorClear);
  text_layer_set_text_color(startText, GColorBlack);
  text_layer_set_text(startText, "Press Select to Play.");
  text_layer_set_font(startText, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(startText, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(titleText));
}

static void title_window_unload(Window *window){
  text_layer_destroy(titleText);
  text_layer_destroy(startText);
}

static void init(){
  titleWindow = window_create();
  window_set_window_handlers(titleWindow, (WindowHandlers){
    .load = title_window_load,
    .unload = title_window_unload
  });
  window_stack_push(titleWindow, true);
}

static void deinit(){
  window_destroy(titleWindow);
}


int main(void){
  init();
  app_event_loop();
  deinit();
}