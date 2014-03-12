#include <pebble.h>

#define KEY_QUESTION  0
#define KEY_OP1 1
#define KEY_OP2 2
#define KEY_OP3 3
#define OUTPUT 5
#define APP_TITLE_HEIGHT 12

///Keys for all the lines
#define TOTAL_LINES 4

//Global structures
static Window *window;
static TextLayer *outputLayer;
static MenuLayer *menuLayer;
//static char line_status[TOTAL_LINES][32];
char question_buffer[64], op1_buffer[32], op2_buffer[32], op3_buffer[32];

//Prototypes
void applog(char* message);
void breakpoint();
void send_int(int key, int msg);
void setup_app_message();
void remove_all_layers();
//void set_fsm_state(int new_state);
void interpret_message_result(AppMessageResult app_message_error);

/****************************** App Message callbacks ***************************************/

void out_sent_handler(DictionaryIterator *sent, void *context) 
{}
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
  //(void) context;

  //Get data
  Tuple *t = dict_read_first(iter);
  if(t) 
  {
    int key = t->key;

  //Get integer value, if present
    int value = t->value->int32;

  //Get string value, if present
    char string_value[32];
    strcpy(string_value, t->value->cstring);
    //text_layer_set_text(outputLayer, t->value->cstring);

    switch(key) 
    {
    case KEY_QUESTION:
      //Location received
      snprintf(question_buffer, sizeof("couldbereallylongname"), "%s", string_value);
      break;
    case KEY_OP1:
      //op1 received
      snprintf(op1_buffer, sizeof("couldbelongname"), "%s", string_value);
      break;
    case KEY_OP2:
      //op2 received
      snprintf(op2_buffer, sizeof("couldbelongname"), "%s", string_value);
      break;
    case KEY_OP3:
      //op3 received received
      snprintf(op3_buffer, sizeof("couldbelongname"), "%s", string_value);
      break;
    }
  }    
    while (t !=NULL)
    {    
          t=dict_read_next(iter);
  
      if(t)
      {
    int key = t->key;

  //Get integer value, if present
    int value = t->value->int32;

  //Get string value, if present
    char string_value[32];
    strcpy(string_value, t->value->cstring); 

        switch(key) 
        {
    case KEY_QUESTION:
      //Location received
      snprintf(question_buffer, sizeof("couldbereallylongname"), "%s", string_value);
      break;
    case KEY_OP1:
      //op1 received
     snprintf(op1_buffer, sizeof("couldbelongname"), "%s", string_value);
      break;
    case KEY_OP2:
      //op2 received
      snprintf(op2_buffer, sizeof("couldbelongname"), "%s", string_value);
      break;
    case KEY_OP3:
      //op3 received received
      snprintf(op3_buffer, sizeof("couldbelongname"), "%s", string_value);
      break;
        }
      }
    }
    menu_layer_reload_data (menuLayer); 
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

void draw_row_handler(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
{
  switch (cell_index->row) 
    {
   case KEY_QUESTION:
    menu_cell_basic_draw(ctx, cell_layer, "Question", question_buffer, NULL);    
    break;
  case KEY_OP1:
    menu_cell_basic_draw(ctx, cell_layer, "Option 1 ",op1_buffer, NULL);
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

void up_click_handler(ClickRecognizerRef recognizer, void *context) 
{}

void select_click_handler(ClickRecognizerRef recognizer, void *context) 
{
  //Create an array of ON-OFF-ON etc durations in milliseconds
  uint32_t segments[] = {100, 200, 500};

  //Create a VibePattern structure with the segments and length of the pattern as fields
  VibePattern pattern = {
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
  };

  //Trigger the custom pattern to be executed
  vibes_enqueue_custom_pattern(pattern);
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) 
{}

void click_config_provider(void *context) 
{
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}
 
/************************************** Window lifecycle callbacks ***********************************/

static void window_load(Window *window) 
{
  window_set_background_color(window, GColorBlack);

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
       .select_click = (MenuLayerSelectCallback) select_click_handler //Unused for now
     });

  layer_add_child(window_get_root_layer(window), (Layer*) menuLayer);
    //vibes_short_pulse();

  //menu_layer_reload_data (menuLayer); 
 

  menu_layer_set_click_config_onto_window(menuLayer, window);  
}

static void window_unload(Window *window) 
{
 
  //Free text layer
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
  window_set_click_config_provider(window, click_config_provider);
  //window_set_click_config_provider(window, (ClickConfigProvider)config_provider);

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


