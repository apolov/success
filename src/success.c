#include <pebble.h>

#define KEY_QUESTION  0
#define KEY_OP1 1
#define KEY_OP2 2
#define KEY_OP3 3
#define OUTPUT 5
#define APP_TITLE_HEIGHT 6
//Keys for all the lines
#define TOTAL_LINES 4

//Global structures
static Window *window;
static TextLayer *outputLayer;
static MenuLayer *menuLayer;
//static PropertyAnimation *trainAnimation;
//static int current_state;
static char line_status[TOTAL_LINES][32];
char question_buffer[64], op1_buffer[32], op2_buffer[32], op3_buffer[32];

//Prototypes
void applog(char* message);
void breakpoint();
void send_int(int key, int msg);
void setup_app_message();
void remove_all_layers();
//void set_fsm_state(int new_state);
void set_image(BitmapLayer *layer, GBitmap *bitmap, GRect frame);
void destroy_property_animation(PropertyAnimation *prop_animation);
void animateLayer(PropertyAnimation *animation, Layer *input, GRect startLocation, GRect finishLocation, int duration, int delay);
void interpret_message_result(AppMessageResult app_message_error);

/****************************** App Message callbacks ***************************************/

void out_sent_handler(DictionaryIterator *sent, void *context) 
{

}

/*
 * Out failed handler
 */
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) 
{
  applog("PEBBLE: out_failed_handler");

  //Get reason
  interpret_message_result(reason);
}

/*
 * In received handler
 */
void in_received_handler(DictionaryIterator *iter, void *context) 
{
  Tuple *tuple = dict_find(iter, OUTPUT);
  if(tuple) 
  {
    text_layer_set_text(outputLayer, tuple->value->cstring);
  }

  //New FSM state?
  /*tuple = dict_find(iter, STATE);
  if(tuple)
  {
    set_fsm_state(tuple->value->int32);
  }*/

  //Line info?
  /*for(int i = 0; i < TOTAL_LINES; i++)
  {
    tuple = dict_find(iter, i);
    if(tuple)
    {
      strcpy(line_status[i], tuple->value->cstring);
    }
  }*/
}
/*
 * In dropped handler
 */
void in_dropped_handler(AppMessageResult reason, void *context) 
{
  applog("PEBBLE: in_dropped_handler");

  //Get reason
  interpret_message_result(reason);
}
/************************************ Menu Layer callbacks *******************************************/

/*
 * Draw a MenuLayer row
 */

void draw_row_handler(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
  {
  switch (cell_index->row) 
    {
  case KEY_OP1:
    menu_cell_basic_draw(ctx, cell_layer, "Option 1", op1_buffer, NULL);
    break;
  case KEY_OP2:
    menu_cell_basic_draw(ctx, cell_layer, "Option 2", op2_buffer, NULL);
    break;
  case KEY_OP3:
    menu_cell_basic_draw(ctx, cell_layer, "Option 3", op3_buffer, NULL);
    break;
  default:
      menu_cell_basic_draw(ctx, cell_layer, "Unknown", "This is a bug!", NULL);
    break;
    } 
  }


/*static void in_received_handler(DictionaryIterator *iter, void *context) 
{
  (void) context;
  
  //Get data
  Tuple *t = dict_read_first(iter);
  if(t)
  {
    process_tuple(t);
  }
  
  //Get next
  while(t != NULL)
  {
    t = dict_read_next(iter);
    if(t)
    {
      process_tuple(t);
    }
  }
}*/

/*void send_int(uint8_t key, uint8_t msg)
{
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  
  Tuplet value = TupletInteger(key, msg);
  dict_write_tuplet(iter, &value);
  
  app_message_outbox_send();
}*/
/*
 * Get the number of rows in the MenuLayer
 */
uint16_t get_num_rows_handler(MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
  return TOTAL_LINES;
}

/*
 * Click handler - unused
void select_click_handler(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
  if(current_state == STATE_SHOWING)
  {
    set_fsm_state(STATE_WAITING);
  }
}
*/

/************************************** Click events *************************************************/

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) 
{}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) 
{}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) 
{}

void config_provider(Window *window)
{
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_single_click_handler);
}
 
/************************************** Window lifecycle callbacks ***********************************/

static void window_load(Window *window) 
{
  window_set_background_color(window, GColorBlack);

  /*//Images
  back = gbitmap_create_with_resource(RESOURCE_ID_BACK);
  backLayer = bitmap_layer_create(GRect(0, 0, 144, 144));
  train = gbitmap_create_with_resource(RESOURCE_ID_TRAIN);
  trainLayer = bitmap_layer_create(GRect(0, 0, 154, 34));*/

  //Output text
  outputLayer = text_layer_create(GRect(0, 90, 144, 30));
  text_layer_set_background_color(outputLayer, GColorClear);
  text_layer_set_text_color(outputLayer, GColorWhite);

  //Menu Layer
  menuLayer = menu_layer_create(GRect(0, 0, 144, 168 - APP_TITLE_HEIGHT));

  //Adding to window is done in set_fsm_state()

  window_set_background_color(window, GColorWhite);

  menu_layer_set_callbacks(menuLayer, NULL, (MenuLayerCallbacks) {
       .draw_row = (MenuLayerDrawRowCallback) draw_row_handler,
       .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) get_num_rows_handler,
       .select_click = (MenuLayerSelectCallback) select_single_click_handler //Unused for now
     });

  layer_add_child(window_get_root_layer(window), (Layer*) menuLayer);
    //vibes_short_pulse();

  menu_layer_set_click_config_onto_window(menuLayer, window);  
}

static void window_unload(Window *window) 
{
  /*//Free bitmaps
  gbitmap_destroy(back);
  gbitmap_destroy(train);

  //Free their layers
  bitmap_layer_destroy(backLayer);
  bitmap_layer_destroy(trainLayer);*/

  //Free tex layer
  text_layer_destroy(outputLayer);

  //Free menu layer
  menu_layer_destroy(menuLayer);

  //Free window
  window_destroy(window);
}

/************************************* App lifecycle callbacks *********************************/

static void init(void) 
{
  //Create window
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  //AppMessage
  setup_app_message();

  //Responiveness+
  app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);

  //Clicks
  window_set_click_config_provider(window, (ClickConfigProvider)config_provider);

  //Show
  window_stack_push(window, true);
}

static void deinit(void) 
{
  //Save power
  app_comm_set_sniff_interval(SNIFF_INTERVAL_NORMAL); 
}

int main(void) 
{
  init();
  app_event_loop();
  deinit();
}

/************************* Function Definitions **********************************/

/*
 * Convenience to log a message without respect to the level or line number
 */
 void applog(char* message)
{
  app_log(APP_LOG_LEVEL_INFO, "success.c", 0, message);
}


//  * Convenience marker for debugging function progress
 
// void breakpoint()
// {
//   app_log(APP_LOG_LEVEL_DEBUG, "main.c", 0, "BREAKPOINT");
// }

/*
 * Send an integer
 */
void send_int(int key, int msg) 
{
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  Tuplet value = TupletInteger(key, msg);
  dict_write_tuplet(iter, &value);

  app_message_outbox_send();
}

/*
 * Ronseal Wood Varnish
 */
void setup_app_message()
{
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  
  const uint32_t inbound_size = 256;
  const uint32_t outbound_size = 16;
  app_message_open(inbound_size, outbound_size);
}

/*
 * Remove all layers prior to setting layout by FSM state
 */
void remove_all_layers()
{
  //Remove text layers
  layer_remove_from_parent((Layer*) outputLayer);

  //Remove image
  //layer_remove_from_parent((Layer*) backLayer);

  //Remove menu layer
  layer_remove_from_parent((Layer*) menuLayer);
}

/*
 * Set the FSM state for the layout
 */
// void set_fsm_state(int new_state)
// {
//   //Report state in console
//   char *temp = malloc(sizeof("PEBBLE: state: 00"));
//   snprintf(temp, sizeof("PEBBLE: state: 00"), "PEBBLE: state: %d", new_state);
//   applog(temp);

  //Remember state
//   current_state = new_state;

//   //Apply the new state
//   switch(new_state)
//   {
//   //Initial state - waiting for JS
//   case STATE_INIT:
//     set_image(backLayer, back, GRect(0, 0, 144, 144));
//     layer_add_child(window_get_root_layer(window), (Layer*) outputLayer);
//     text_layer_set_text(outputLayer, "Waiting for PebbleJS...");

//     //Animate train
//     set_image(trainLayer, train, GRect(144, 49, 154, 34));
//     animateLayer(trainAnimation, (Layer*) bitmap_layer_get_layer(trainLayer), GRect(144, 49, 154, 34), GRect(-154, 49, 154, 34), 5000, 0);
//     break;
//   //Waiting for JS confirm
//   case STATE_WAITING:
//     text_layer_set_text(outputLayer, "Waiting for PebbleJS...");
//     send_int(REQUEST_DATA, 0);
//     break;
//   //Waiting for fresh data
//   case STATE_UPDATING:
//     text_layer_set_text(outputLayer, "Downloading data...");
//     break;
//   //Data obtained, show menu of new data
//   case STATE_SHOWING:
//     //Remove text layers
//     layer_remove_from_parent((Layer*) outputLayer);

//     //Remove images
//     layer_remove_from_parent((Layer*) backLayer);
//     layer_remove_from_parent((Layer*) trainLayer);

//     //Remove any animation
//     destroy_property_animation(trainAnimation);

//     //Prepare window
//     window_set_background_color(window, GColorWhite);

      //Setup menu layer
     //menu_layer_set_click_config_onto_window(menuLayer, window);
     // menu_layer_set_callbacks(menuLayer, NULL, (MenuLayerCallbacks) {
     //        .draw_row = (MenuLayerDrawRowCallback) draw_row_handler,
     //   .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) get_num_rows_handler,
     //   .select_click = (MenuLayerSelectCallback) select_single_click_handler //Unused for now
     // });

     //layer_add_child(window_get_root_layer(window), (Layer*) menuLayer);
     //vibes_short_pulse();
     //break;
//   //Some error state - unused
//   case STATE_ERROR:
//     set_image(backLayer, back, GRect(0, 0, 144, 144));
//     layer_add_child(window_get_root_layer(window), (Layer*) outputLayer);
//     text_layer_set_text(outputLayer, "Error updating!");
//     break;
//   }
// }

/*
 * Set an image in a bitmap layer
 */
// void set_image(BitmapLayer *layer, GBitmap *bitmap, GRect frame) {
//   //Deinit old
//   layer_remove_from_parent((Layer*) layer);
//   bitmap_layer_destroy(layer);
 
//   //Init new
//   layer = bitmap_layer_create(frame);
 
//   //Set bitmap
//   bitmap_layer_set_bitmap(layer, bitmap);
 
  //Add to window
  //layer_add_child(window_get_root_layer(window), (Layer*) layer);
//}

/**
  * Destory an animation to conserve memory
  * Source: https://github.com/fuzzie360/pebble-noms/blob/master/src/noms.c
  */
// void destroy_property_animation(PropertyAnimation *prop_animation) {
//   if (prop_animation == NULL) {
//     return;
//   }

//   if (animation_is_scheduled((Animation*) prop_animation)) {
//     animation_unschedule((Animation*) prop_animation);
//   }

//   property_animation_destroy(prop_animation);
//   prop_animation = NULL;
// }

/**
  * Animation handlers
  */
// void animation_started(Animation *animation, void *data) {
//   (void)animation;
//   (void)data;
// }

// void animation_stopped(Animation *animation, void *data) {
//   (void)data;

//   //Teardown
//   destroy_property_animation((PropertyAnimation*) animation);

//   //SPECIAL CASE DO NOT COPY!
//   animateLayer(trainAnimation, (Layer*) bitmap_layer_get_layer(trainLayer), GRect(144, 49, 154, 34), GRect(-154, 49, 154, 34), 5000, 0);
// }
void interpret_message_result(AppMessageResult app_message_error)
{
  if(app_message_error == APP_MSG_OK)
  {
    applog("APP_MSG_OK");
  } 

  else if(app_message_error == APP_MSG_SEND_TIMEOUT)
  {
    applog("APP_MSG_SEND_TIMEOUT");
  } 

  else if(app_message_error == APP_MSG_SEND_REJECTED)
  {
    applog("APP_MSG_SEND_REJECTED");
  }

  else if(app_message_error == APP_MSG_NOT_CONNECTED)
  {
    applog("APP_MSG_NOT_CONNECTED");
  }

  else if(app_message_error == APP_MSG_APP_NOT_RUNNING)
  {
    applog("APP_MSG_APP_NOT_RUNNING");
  }

  else if(app_message_error == APP_MSG_INVALID_ARGS)
  {
    applog("APP_MSG_INVALID_ARGS");
  }

  else if(app_message_error == APP_MSG_BUSY)
  {
    applog("APP_MSG_BUSY");
  }

  else if(app_message_error == APP_MSG_BUFFER_OVERFLOW)
  {
    applog("APP_MSG_BUFFER_OVERFLOW");
  }

  else if(app_message_error == APP_MSG_ALREADY_RELEASED)
  {
    applog("APP_MSG_ALREADY_RELEASED");
  }

  else if(app_message_error == APP_MSG_CALLBACK_ALREADY_REGISTERED)
  {
    applog("APP_MSG_CALLBACK_ALREADY_REGISTERED");
  }

  else if(app_message_error == APP_MSG_CALLBACK_NOT_REGISTERED)
  {
    applog("APP_MSG_CALLBACK_NOT_REGISTERED");
  }

  else if(app_message_error == APP_MSG_OUT_OF_MEMORY)
  {
    applog("APP_MSG_OUT_OF_MEMORY");
  }
}


