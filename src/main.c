#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "http.h"
#include "util.h"
//#include "weather_layer.h"
//#include "time_layer.h"
#include "link_monitor.h"
#include "config.h"
	
//#define MY_UUID { 0x01, 0x6E, 0x11, 0x1F, 0xB2, 0x60, 0x4E, 0x80, 0xA7, 0xCD, 0x78, 0x35, 0xA0, 0x31, 0xE3, 0xF3 } //OG PEBSONA 4
#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x04, 0x9F, 0x49, 0xC0, 0x99, 0xAD }
//find a way to have this work without using this id!
PBL_APP_INFO(MY_UUID,
             "Pebsona4", "Masamune_Shadow",
             1, 4, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

// POST variables
#define WEATHER_KEY_LATITUDE 1
#define WEATHER_KEY_LONGITUDE 2
#define WEATHER_KEY_UNIT_SYSTEM 3
//#define WEATHER_KEY_TIME 3

// Received variables
#define WEATHER_KEY_ICON 1
#define WEATHER_KEY_TEMPERATURE 2
	
#define WEATHER_HTTP_COOKIE 1949327671
#define TIME_HTTP_COOKIE 1131038282
	
#define DATE "Date"
#define TIME "Time"
#define WORD "Word"
#define WEATHER "Weather"
#define ALL "All"

#define timeWidth 19
#define timeHeight 19
#define timePosY 55
	
#define	dateFrame  (GRect(0,2,100,26))
#define	wordDateFrame  (GRect(100,10,50,26))
#define	wordFrame  (GRect(0,32,144,21))
#define	weatherFrame  (GRect(53,87,75,75))

#define	timeFrame  (GRect(0,timePosY,144,25))
#define	timeHourTensFrame  (GRect(0,timePosY,timeWidth,timeHeight))
#define	timeHourOnesFrame  (GRect(20,timePosY,timeWidth,timeHeight))
#define	timeColonFrame  (GRect(40,timePosY,8,timeHeight))
#define	timeMinuteTensFrame  (GRect(48,timePosY,timeWidth,timeHeight))
#define	timeMinuteOnesFrame  (GRect(68,timePosY,timeWidth, timeHeight))

	//Animation Defines
#define openAnimationDuration 500
#define closingAnimationDuration 500
#define slideAnimationDuration 500
#define openAnimationDelay 250
#define closingAnimationDelay 3000
#define slideAnimationDelay 250
#define LAYER_TEXT_MONTH_FRAME (GRect(45,0,50,45))
#define LAYER_TEXT_YEAR_FRAME (GRect(45,50,50,28))
#define START_RECT (GRect(67,0,0,168))
#define END_RECT (GRect(46,0,52,168))
//all of the images will be of the same size as the square, but thanks to the inverts, it'll work out w/ trans.
#define DAY_0_IMAGE_FRAME (GRect(0,119,0,38))
#define DAY_1_IMAGE_FRAME (GRect(1,119,40,38))
#define DAY_2_IMAGE_FRAME (GRect(53,119,40,38))//highlighed frame
#define	DAY_3_IMAGE_FRAME (GRect(105,119,40,38))
#define DAY_4_IMAGE_FRAME (GRect(170,119,40,38))
#define DAY_0_WORD_FRAME (GRect(0,119,0,38))
#define DAY_1_WORD_FRAME (GRect(5,85,35,10))
#define DAY_2_WORD_FRAME (GRect(57,85,35,10))
#define DAY_3_WORD_FRAME (GRect(110,85,35,10))
#define DAY_4_WORD_FRAME (GRect(170,85,35,10))	
#define DAY_0_DAY_FRAME   (GRect(0,95,0,15))
#define DAY_1_DAY_FRAME   (GRect(5,95,35,15))
#define DAY_2_DAY_FRAME   (GRect(57,95,35,15))
#define DAY_3_DAY_FRAME   (GRect(110,95,35,15))
#define DAY_4_DAY_FRAME   (GRect(170,95,35,15))
	
//Once weather support gets introduced, the "Sunny" icon will change to reflect said weather.
//ingame there are:
/*
Sunny		=	1
Umbrella	=	2
Raindrops	
Cloud		=	3 
Snowman		=	4
Snow
Thunder
Fog
(All are a combo of these)
*/

/* Word Time Enums
Midnight	=	0
EarlyMorning=	1
Morning		=	2
Lunchtime	=	3 
Afternoon	=	4
Evening		=	5
Night		=	6
*/


Window window;
/*=========Layers============*/
TextLayer 	layerDate; //white
TextLayer 	layerDateWord; //white
BitmapLayer layerTimeH1_WHITE;
BitmapLayer layerTimeH2_WHITE;
BitmapLayer layerTimeM1_WHITE;
BitmapLayer layerTimeM2_WHITE;
BitmapLayer layerTimeH1_BLACK;
BitmapLayer layerTimeH2_BLACK;
BitmapLayer layerTimeM1_BLACK;
BitmapLayer layerTimeM2_BLACK;

BitmapLayer layerColon_WHITE;
BitmapLayer layerColon_BLACK;

BitmapLayer layerWord_WHITE;
BitmapLayer layerWord_BLACK;
BitmapLayer layerWeather;

//structs added later, don't want to fix what isn't broken, so I won't be going back and updating the previous code.
typedef struct dayTransDay{
	TextLayer 		layerWord;
	TextLayer 		layerDay;
	BitmapLayer 	layerWeather;
	BitmapLayer 	layerWeatherSplitTop_WHITE;
	BitmapLayer 	layerWeatherSplitTop_BLACK;
	BitmapLayer 	layerWeatherSplitBottom_WHITE;
	BitmapLayer 	layerWeatherSplitBottom_BLACK;
	BmpContainer 	imgWeather;
	BmpContainer 	imgWeatherSplitTop_WHITE;
	BmpContainer 	imgWeatherSplitTop_BLACK;
	BmpContainer 	imgWeatherSplitBottom_WHITE;
	BmpContainer 	imgWeatherSplitBottom_BLACK;
	PropertyAnimation slideLeftAnimation;
	GRect wordRect;
	GRect dayRect;
	GRect imgRect;
	bool isSplit;
	int previousWeather;
	int previousWeatherSplitTop;
	int previousWeatherSplitBottom;
	int currentWeather;
	int currentWeatherSplitTop;
	int currentWeatherSplitBottom;
}dayTransDay;
#define NUMBER_OF_DAYS 4
typedef struct dayTrans{
	BitmapLayer layerDayTrans;
	InverterLayer invertLayer;
	TextLayer layerTextMonth;
	TextLayer layerTextYear;
	GRect layerTextMonthFrame;
	GRect layerTextYearFrame;
	GRect startRect;
	GRect endRect;
	//dayTransDay day[NUMBER_OF_DAYS]; //4
	dayTransDay day;
	PblTm tm;
	PebbleTickEvent tick;
	PropertyAnimation openingAnimation;
	PropertyAnimation closingAnimation;	
}dayTrans;
dayTrans dayTransition;
//Word Transitions
/*Layer transition_word_layer;
Layer transition_block_1_layer;
Layer transition_block_2_layer;
Layer transition_square_layer;*/

/*static uint8_t WEATHER_ICONS[] = {
	RESOURCE_ID_IMAGE_CLEAR_DAY,
	RESOURCE_ID_ICON_CLEAR_NIGHT,
	RESOURCE_ID_ICON_RAIN,
	RESOURCE_ID_ICON_SNOW,
	RESOURCE_ID_ICON_SLEET,
	RESOURCE_ID_ICON_WIND,
	RESOURCE_ID_ICON_FOG,
	RESOURCE_ID_ICON_CLOUDY,
	RESOURCE_ID_ICON_PARTLY_CLOUDY_DAY,
	RESOURCE_ID_ICON_PARTLY_CLOUDY_NIGHT,
	RESOURCE_ID_ICON_ERROR,
};
*/
static const int DIGIT_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_DIGIT_0,
  RESOURCE_ID_IMAGE_DIGIT_1,
  RESOURCE_ID_IMAGE_DIGIT_2,
  RESOURCE_ID_IMAGE_DIGIT_3,
  RESOURCE_ID_IMAGE_DIGIT_4,
  RESOURCE_ID_IMAGE_DIGIT_5,
  RESOURCE_ID_IMAGE_DIGIT_6,
  RESOURCE_ID_IMAGE_DIGIT_7,
  RESOURCE_ID_IMAGE_DIGIT_8,
  RESOURCE_ID_IMAGE_DIGIT_9
};

static const int DIGIT_IMAGE_RESOURCE_IDS_INVERT[] = {
  RESOURCE_ID_IMAGE_DIGIT_0_INVERT,
  RESOURCE_ID_IMAGE_DIGIT_1_INVERT,
  RESOURCE_ID_IMAGE_DIGIT_2_INVERT,
  RESOURCE_ID_IMAGE_DIGIT_3_INVERT,
  RESOURCE_ID_IMAGE_DIGIT_4_INVERT,
  RESOURCE_ID_IMAGE_DIGIT_5_INVERT,
  RESOURCE_ID_IMAGE_DIGIT_6_INVERT,
  RESOURCE_ID_IMAGE_DIGIT_7_INVERT,
  RESOURCE_ID_IMAGE_DIGIT_8_INVERT,
  RESOURCE_ID_IMAGE_DIGIT_9_INVERT
};

BmpContainer background_image;

#define TOTAL_TIME_DIGITS 4
static BmpContainer imgTime_BLACK[TOTAL_TIME_DIGITS];
static int imgTimeResourceIds_BLACK[TOTAL_TIME_DIGITS];

static BmpContainer imgTime_WHITE[TOTAL_TIME_DIGITS];
static int imgTimeResourceIds_WHITE[TOTAL_TIME_DIGITS];

BmpContainer imgWord_BLACK;
BmpContainer imgWord_WHITE;
BmpContainer imgColon_BLACK;
BmpContainer imgColon_WHITE;
BmpContainer imgWeather;

int currentWord;
//int currentMinute;
int currentM1;
int currentM2;
//int currentHour;
int currentH1;
int currentH2;
int currentDay;
int currentWeather;

int previousWord;
//int previousMinute;
int previousHour;
int previousDay;
int previousWeather;

GFont fontDate;
GFont fontAbbr;
GFont fontMonth; //29
GFont fontYear;	//29
ResHandle resMonth;	//29
ResHandle resYear;	//29
ResHandle resDate;
ResHandle resAbbr;

/*=========Bools============*/
bool isDayTransition = true;
bool isWordTransition = false;
bool isTransitioning = true;
bool httpFailed;
bool Initializing = true;
//Day Transitions
/*
Layer DayGraphic
TextLayer text_numday_layer;
TextLayer text_monthday_layer;
*/
/* Need to figure out how to properly rotate a square into a diamond
https://github.com/pebble/pebblekit/blob/master/Pebble/demos/feature_image_transparent_rotate/src/feature_image_transparent_rotate.c
just make a square, and then use that code to rotate it 45 degrees (or however much I want)

bam that easy.

void square_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  graphics_context_set_stroke_color(ctx, GColorWhite); // TODO: Needed?
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(12,0,120,50), 0, GCornersAll);
}*/
	
	//Weather Stuff
static int our_latitude, our_longitude;
static bool located = false;

void SetWordImage()
{	
	if (currentWord == 0)//midnight
	{
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_MIDNIGHT,&imgWord_BLACK);
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_MIDNIGHT_INVERT,&imgWord_WHITE);
	}		
	else if (currentWord == 1) //early morning
	{
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_EARLY_MORNING,&imgWord_BLACK);
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_EARLY_MORNING_INVERT,&imgWord_WHITE);
	}
	else if(currentWord == 2) //morning
	{	
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_MORNING,&imgWord_BLACK);
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_MORNING_INVERT,&imgWord_WHITE);
	}
	else if(currentWord == 3)//lunchtime
	{
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_LUNCHTIME,&imgWord_BLACK);
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_LUNCHTIME_INVERT,&imgWord_WHITE);
	}
	else if(currentWord == 4)//afternoon
	{
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_AFTERNOON,&imgWord_BLACK);
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_AFTERNOON_INVERT,&imgWord_WHITE);
	}
	else if(currentWord == 5)//evening
	{
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_EVENING,&imgWord_BLACK);
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_EVENING_INVERT,&imgWord_WHITE);
	}
	else if(currentWord == 6)//night
	{
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_NIGHT,&imgWord_BLACK);
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_NIGHT_INVERT,&imgWord_WHITE);
	}
	else if(currentWord == 7)//Unknown
	{
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_BLANK,&imgWord_BLACK);
		bmp_init_container(RESOURCE_ID_IMAGE_TIME_BLANK,&imgWord_WHITE);
	}
}

void SetWeatherImage()
{	
	if (!isTransitioning)
	{
		if (currentWeather == 0 || currentWeather == 1) //sun
		{
			bmp_init_container(RESOURCE_ID_IMAGE_WEATHER_SUN,&imgWeather);
		}
		else if(currentWeather == 2) //rain
		{	
			bmp_init_container(RESOURCE_ID_IMAGE_WEATHER_RAIN,&imgWeather);
		}
		else if(currentWeather == 3 )//snow
		{
			bmp_init_container(RESOURCE_ID_IMAGE_WEATHER_SNOW,&imgWeather);
		}
		else if(currentWeather >= 4 && currentWeather < 10)//cloud
		{
			bmp_init_container(RESOURCE_ID_IMAGE_WEATHER_CLOUD,&imgWeather);
		}
		else if (currentWeather >= 10)
		{
			bmp_init_container(RESOURCE_ID_IMAGE_WEATHER_UNKNOWN,&imgWeather);
		}
	}
	else
	{
		//decypher the dates, but for now just set them all to sunny
		bmp_init_container(RESOURCE_ID_IMAGE_SQUARE_SUN,&dayTransition.day.imgWeather);
		/*for (int i = 0; i < 4; i++)
		{
			if (!dayTransition.day[i].isSplit)
			{
				bmp_init_container(RESOURCE_ID_IMAGE_SQUARE_SUN,&dayTransition.day[i].imgWeather);
			}
			else
			{
				//tops
				bmp_init_container(RESOURCE_ID_IMAGE_SQUARE_SUN_INVERT,&dayTransition.day[i].imgWeatherSplitTop_WHITE);
				bmp_init_container(RESOURCE_ID_IMAGE_SQUARE_SUN,&dayTransition.day[i].imgWeatherSplitTop_BLACK);
								
				//bottom
				bmp_init_container(RESOURCE_ID_IMAGE_SQUARE_SUN_INVERT,&dayTransition.day[i].imgWeatherSplitBottom_WHITE);
				bmp_init_container(RESOURCE_ID_IMAGE_SQUARE_SUN,&dayTransition.day[i].imgWeatherSplitBottom_BLACK);			
			}
        }*/
	}
}


void SetTimeImage()
{	
	bmp_init_container(DIGIT_IMAGE_RESOURCE_IDS[currentH1],&imgTime_BLACK[0]);
	bmp_init_container(DIGIT_IMAGE_RESOURCE_IDS[currentH2],&imgTime_BLACK[1]);
	bmp_init_container(DIGIT_IMAGE_RESOURCE_IDS[currentM1],&imgTime_BLACK[2]);
	bmp_init_container(DIGIT_IMAGE_RESOURCE_IDS[currentM2],&imgTime_BLACK[3]);

	bmp_init_container(DIGIT_IMAGE_RESOURCE_IDS_INVERT[currentH1],&imgTime_WHITE[0]);
	bmp_init_container(DIGIT_IMAGE_RESOURCE_IDS_INVERT[currentH2],&imgTime_WHITE[1]);
	bmp_init_container(DIGIT_IMAGE_RESOURCE_IDS_INVERT[currentM1],&imgTime_WHITE[2]);
	bmp_init_container(DIGIT_IMAGE_RESOURCE_IDS_INVERT[currentM2],&imgTime_WHITE[3]);

	bmp_init_container(RESOURCE_ID_IMAGE_DIGIT_COLON,&imgColon_BLACK);
	bmp_init_container(RESOURCE_ID_IMAGE_DIGIT_COLON_INVERT,&imgColon_WHITE);
	
}

void RemoveAndDeIntBmp(char* bitmap)
{
	if (strcmp(bitmap,WORD) == 0 || strcmp(bitmap,ALL) == 0)
	{
		layer_remove_from_parent(&layerWord_WHITE.layer);
		layer_remove_from_parent(&layerWord_BLACK.layer);
		bmp_deinit_container(&imgWord_WHITE);
		bmp_deinit_container(&imgWord_BLACK);
	}
	
	if (strcmp (bitmap,WEATHER) == 0 || strcmp(bitmap,ALL) == 0)
	{
		if (!isTransitioning)
		{
			layer_remove_from_parent(&layerWeather.layer);
			bmp_deinit_container(&imgWeather);
		}
		else //is transitioning
		{
			layer_remove_from_parent(&dayTransition.day.layerWeather.layer);
			bmp_deinit_container(&dayTransition.day.imgWeather);
			/*for (int i = 0; i < 4; i++)
			{
				if (i != 2) //gets special de-inted with the closing.
				{
					if (!dayTransition.day[i].isSplit)
					{
						layer_remove_from_parent(&dayTransition.day[i].layerWeather.layer);
						bmp_deinit_container(&dayTransition.day[i].imgWeather);
					}
					else
					{
						layer_remove_from_parent(&dayTransition.day[i].layerWeatherSplitTop_WHITE.layer);
						bmp_deinit_container(&dayTransition.day[i].imgWeatherSplitTop_WHITE);
						layer_remove_from_parent(&dayTransition.day[i].layerWeatherSplitBottom_WHITE.layer);
						bmp_deinit_container(&dayTransition.day[i].imgWeatherSplitBottom_WHITE);
						layer_remove_from_parent(&dayTransition.day[i].layerWeatherSplitTop_BLACK.layer);
						bmp_deinit_container(&dayTransition.day[i].imgWeatherSplitTop_BLACK);
						layer_remove_from_parent(&dayTransition.day[i].layerWeatherSplitBottom_BLACK.layer);
						bmp_deinit_container(&dayTransition.day[i].imgWeatherSplitBottom_BLACK);
					}
				}
			}*/
		}
	}

	if (strcmp (bitmap,TIME) == 0 || strcmp(bitmap,ALL) == 0)
	{
		layer_remove_from_parent(&layerTimeH1_WHITE.layer);
		layer_remove_from_parent(&layerTimeH2_WHITE.layer);
		layer_remove_from_parent(&layerTimeM1_WHITE.layer);
		layer_remove_from_parent(&layerTimeM2_WHITE.layer);

		layer_remove_from_parent(&layerColon_WHITE.layer);
		layer_remove_from_parent(&layerColon_BLACK.layer);

		layer_remove_from_parent(&layerTimeH1_BLACK.layer);
		layer_remove_from_parent(&layerTimeH2_BLACK.layer);
		layer_remove_from_parent(&layerTimeM1_BLACK.layer);
		layer_remove_from_parent(&layerTimeM2_BLACK.layer);


		bmp_deinit_container(&imgTime_WHITE[0]);
		bmp_deinit_container(&imgTime_WHITE[1]);
		bmp_deinit_container(&imgTime_WHITE[2]);
		bmp_deinit_container(&imgTime_WHITE[3]);

		bmp_deinit_container(&imgColon_WHITE);
		bmp_deinit_container(&imgColon_BLACK);

		bmp_deinit_container(&imgTime_BLACK[0]);
		bmp_deinit_container(&imgTime_BLACK[1]);
		bmp_deinit_container(&imgTime_BLACK[2]);
		bmp_deinit_container(&imgTime_BLACK[3]);
	}
}

//for bitmap only (black and white)
//bitmap = layer that we want to manipulate
void SetBitmap(char* bitmap)
{
	if (!Initializing && !isTransitioning)
	{
		RemoveAndDeIntBmp(bitmap);
	}
	if (strcmp(bitmap,WORD) == 0)
	{    
		SetWordImage();			
		//black background, takes the white and makes them black, all else are not drawn
		bitmap_layer_init(&layerWord_WHITE,wordFrame);
		bitmap_layer_set_bitmap(&layerWord_WHITE,&imgWord_WHITE.bmp);
		bitmap_layer_set_background_color(&layerWord_WHITE,GColorClear);
		bitmap_layer_set_compositing_mode(&layerWord_WHITE, GCompOpClear);
		layer_add_child(&window.layer, &layerWord_WHITE.layer);
		layer_mark_dirty(&layerWord_WHITE.layer);

		// takes the white and doesn't draw it if it itercets with a black?
		bitmap_layer_init(&layerWord_BLACK,wordFrame);
		bitmap_layer_set_bitmap(&layerWord_BLACK, &imgWord_BLACK.bmp); //or .layer?
		bitmap_layer_set_background_color(&layerWord_BLACK,GColorClear);
		bitmap_layer_set_compositing_mode(&layerWord_BLACK, GCompOpOr);
		layer_add_child(&window.layer, &layerWord_BLACK.layer);
		layer_mark_dirty(&layerWord_BLACK.layer);			
	}
	else if (strcmp(bitmap,WEATHER) == 0)
	{
		SetWeatherImage();
		if(!isTransitioning)
		{
			bitmap_layer_init(&layerWeather,weatherFrame);
			bitmap_layer_set_bitmap(&layerWeather,&imgWeather.bmp);
			bitmap_layer_set_background_color(&layerWeather,GColorClear);
			bitmap_layer_set_compositing_mode(&layerWeather, GCompOpOr);
			layer_add_child(&window.layer, &layerWeather.layer);
			layer_mark_dirty(&layerWeather.layer);		
			if (previousWeather != currentWeather)
			{
				previousWeather = currentWeather;
				//vibes_short_pulse(); //just a little notification that the weather has changed.
			}
		}
		else
		{
			bitmap_layer_init(&dayTransition.day.layerWeather, dayTransition.day.imgRect);
			bitmap_layer_set_bitmap(&dayTransition.day.layerWeather,&imgWeather.bmp);
			bitmap_layer_set_background_color(&dayTransition.day.layerWeather,GColorClear);
			bitmap_layer_set_compositing_mode(&dayTransition.day.layerWeather, GCompOpAssign);
			layer_add_child(&window.layer, &dayTransition.day.layerWeather.layer);
			layer_mark_dirty(&dayTransition.day.layerWeather.layer);	
			
			/*for (int i = 0; i < 4; i++)
			{
				if (!dayTransition.day[i].isSplit)
				{
					//vibes_short_pulse();
					bitmap_layer_init(&dayTransition.day[i].layerWeather, dayTransition.day[i].imgRect);
					bitmap_layer_set_bitmap(&dayTransition.day[i].layerWeather,&imgWeather.bmp);
					bitmap_layer_set_background_color(&dayTransition.day[i].layerWeather,GColorClear);
					bitmap_layer_set_compositing_mode(&dayTransition.day[i].layerWeather, GCompOpAssign);
					layer_add_child(&window.layer, &dayTransition.day[i].layerWeather.layer);
					layer_mark_dirty(&dayTransition.day[i].layerWeather.layer);		
					
					if (dayTransition.day[i].previousWeather != dayTransition.day[i].currentWeather)
					{
						dayTransition.day[i].previousWeather = dayTransition.day[i].currentWeather;
					}
				}
				else
				{
				//TOP WHITE
					bitmap_layer_init(&dayTransition.day[i].layerWeatherSplitTop_WHITE, dayTransition.day[i].imgRect);
					bitmap_layer_set_bitmap(&dayTransition.day[i].layerWeatherSplitTop_WHITE,&dayTransition.day[i].imgWeatherSplitTop_WHITE.bmp);
					bitmap_layer_set_background_color(&dayTransition.day[i].layerWeatherSplitTop_WHITE,GColorClear);
					bitmap_layer_set_compositing_mode(&dayTransition.day[i].layerWeatherSplitTop_WHITE, GCompOpClear);
					layer_add_child(&window.layer, &dayTransition.day[i].layerWeatherSplitTop_WHITE.layer);
					layer_mark_dirty(&dayTransition.day[i].layerWeatherSplitTop_WHITE.layer);
				//TOP BLACK
					bitmap_layer_init(&dayTransition.day[i].layerWeatherSplitTop_BLACK, dayTransition.day[i].imgRect);
					bitmap_layer_set_bitmap(&dayTransition.day[i].layerWeatherSplitTop_BLACK,&dayTransition.day[i].imgWeatherSplitTop_BLACK.bmp);
					bitmap_layer_set_background_color(&dayTransition.day[i].layerWeatherSplitTop_BLACK,GColorClear);
					bitmap_layer_set_compositing_mode(&dayTransition.day[i].layerWeatherSplitTop_BLACK, GCompOpOr);
					layer_add_child(&window.layer, &dayTransition.day[i].layerWeatherSplitTop_BLACK.layer);
					layer_mark_dirty(&dayTransition.day[i].layerWeatherSplitTop_BLACK.layer);	
				//TOP PREV/CUR
					if (dayTransition.day[i].previousWeatherSplitTop != dayTransition.day[i].currentWeatherSplitTop)
					{
						dayTransition.day[i].previousWeatherSplitTop = dayTransition.day[i].currentWeatherSplitTop;
					}
				//BOTTOM WHITE
					bitmap_layer_init(&dayTransition.day[i].layerWeatherSplitBottom_WHITE, dayTransition.day[i].imgRect);
					bitmap_layer_set_bitmap(&dayTransition.day[i].layerWeatherSplitBottom_WHITE,&dayTransition.day[i].imgWeatherSplitBottom_WHITE.bmp);
					bitmap_layer_set_background_color(&dayTransition.day[i].layerWeatherSplitBottom_WHITE,GColorClear);
					bitmap_layer_set_compositing_mode(&dayTransition.day[i].layerWeatherSplitBottom_WHITE, GCompOpClear);
					layer_add_child(&window.layer, &dayTransition.day[i].layerWeatherSplitBottom_WHITE.layer);
					layer_mark_dirty(&dayTransition.day[i].layerWeatherSplitBottom_WHITE.layer);
				//BOTTOM BLACK
					bitmap_layer_init(&dayTransition.day[i].layerWeatherSplitBottom_BLACK, dayTransition.day[i].imgRect);
					bitmap_layer_set_bitmap(&dayTransition.day[i].layerWeatherSplitBottom_BLACK,&dayTransition.day[i].imgWeatherSplitBottom_BLACK.bmp);
					bitmap_layer_set_background_color(&dayTransition.day[i].layerWeatherSplitBottom_BLACK,GColorClear);
					bitmap_layer_set_compositing_mode(&dayTransition.day[i].layerWeatherSplitBottom_BLACK, GCompOpOr);
					layer_add_child(&window.layer, &dayTransition.day[i].layerWeatherSplitBottom_BLACK.layer);
					layer_mark_dirty(&dayTransition.day[i].layerWeatherSplitBottom_BLACK.layer);		
				//BOTTOM PREV/CUR
					if (dayTransition.day[i].previousWeatherSplitBottom != dayTransition.day[i].currentWeatherSplitBottom)
					{
						dayTransition.day[i].previousWeatherSplitBottom = dayTransition.day[i].currentWeatherSplitBottom;
					}
				}
			}*/
		}
	}
	/*else if (strcmp(bitmap,DATE) == 0){}*/
	else if (strcmp(bitmap,TIME) == 0)
	{
		SetTimeImage();			
		//do 2 times (1 white, 1 black)
		//do 4 times (1 for each digit) 
		//8 times total (yuck)
	//White 1
		bitmap_layer_init(&layerTimeH1_WHITE,timeHourTensFrame);
		bitmap_layer_set_bitmap(&layerTimeH1_WHITE, &imgTime_WHITE[0].bmp); //or .layer?
		bitmap_layer_set_background_color(&layerTimeH1_WHITE,GColorClear);
		bitmap_layer_set_compositing_mode(&layerTimeH1_WHITE, GCompOpClear);
		layer_add_child(&window.layer, &layerTimeH1_WHITE.layer);
		layer_mark_dirty(&layerTimeH1_WHITE.layer);
	//White 2		
		bitmap_layer_init(&layerTimeH2_WHITE,timeHourOnesFrame);
		bitmap_layer_set_bitmap(&layerTimeH2_WHITE, &imgTime_WHITE[1].bmp); //or .layer?
		bitmap_layer_set_background_color(&layerTimeH2_WHITE,GColorClear);
		bitmap_layer_set_compositing_mode(&layerTimeH2_WHITE, GCompOpClear);
		layer_add_child(&window.layer, &layerTimeH2_WHITE.layer);
		layer_mark_dirty(&layerTimeH2_WHITE.layer);
	//White 3
		bitmap_layer_init(&layerTimeM1_WHITE,timeMinuteTensFrame);
		bitmap_layer_set_bitmap(&layerTimeM1_WHITE, &imgTime_WHITE[2].bmp); //or .layer?
		bitmap_layer_set_background_color(&layerTimeM1_WHITE,GColorClear);
		bitmap_layer_set_compositing_mode(&layerTimeM1_WHITE, GCompOpClear);
		layer_add_child(&window.layer, &layerTimeM1_WHITE.layer);
		layer_mark_dirty(&layerTimeM1_WHITE.layer);
	//White 4	
		bitmap_layer_init(&layerTimeM2_WHITE,timeMinuteOnesFrame);
		bitmap_layer_set_bitmap(&layerTimeM2_WHITE, &imgTime_WHITE[3].bmp); //or .layer?
		bitmap_layer_set_background_color(&layerTimeM2_WHITE,GColorClear);
		bitmap_layer_set_compositing_mode(&layerTimeM2_WHITE, GCompOpClear);
		layer_add_child(&window.layer, &layerTimeM2_WHITE.layer);
		layer_mark_dirty(&layerTimeM2_WHITE.layer);
	//Colon White
		bitmap_layer_init(&layerColon_WHITE,timeColonFrame);
		bitmap_layer_set_bitmap(&layerColon_WHITE, &imgColon_WHITE.bmp); //or .layer?
		bitmap_layer_set_background_color(&layerColon_WHITE,GColorClear);
		bitmap_layer_set_compositing_mode(&layerColon_WHITE, GCompOpClear);
		layer_add_child(&window.layer, &layerColon_WHITE.layer);
		layer_mark_dirty(&layerColon_WHITE.layer);

	//Colon Black
		bitmap_layer_init(&layerColon_BLACK,timeColonFrame);
		bitmap_layer_set_bitmap(&layerColon_BLACK, &imgColon_BLACK.bmp); //or .layer?
		bitmap_layer_set_background_color(&layerColon_BLACK,GColorClear);
		bitmap_layer_set_compositing_mode(&layerColon_BLACK, GCompOpOr);
		layer_add_child(&window.layer, &layerColon_BLACK.layer);
		layer_mark_dirty(&layerColon_BLACK.layer);
	//Black 1		
		bitmap_layer_init(&layerTimeH1_BLACK,timeHourTensFrame);
		bitmap_layer_set_bitmap(&layerTimeH1_BLACK, &imgTime_BLACK[0].bmp); //or .layer?
		bitmap_layer_set_background_color(&layerTimeH1_BLACK,GColorClear);
		bitmap_layer_set_compositing_mode(&layerTimeH1_BLACK, GCompOpOr);
		layer_add_child(&window.layer, &layerTimeH1_BLACK.layer);
		layer_mark_dirty(&layerTimeH1_BLACK.layer);
	//Black 2
		bitmap_layer_init(&layerTimeH2_BLACK,timeHourOnesFrame);
		bitmap_layer_set_bitmap(&layerTimeH2_BLACK, &imgTime_BLACK[1].bmp); //or .layer?
		bitmap_layer_set_background_color(&layerTimeH2_BLACK,GColorClear);
		bitmap_layer_set_compositing_mode(&layerTimeH2_BLACK, GCompOpOr);
		layer_add_child(&window.layer, &layerTimeH2_BLACK.layer);
		layer_mark_dirty(&layerTimeH2_BLACK.layer);
	//Black 3	
		bitmap_layer_init(&layerTimeM1_BLACK,timeMinuteTensFrame);
		bitmap_layer_set_bitmap(&layerTimeM1_BLACK, &imgTime_BLACK[2].bmp); //or .layer?
		bitmap_layer_set_background_color(&layerTimeM1_BLACK,GColorClear);
		bitmap_layer_set_compositing_mode(&layerTimeM1_BLACK, GCompOpOr);
		layer_add_child(&window.layer, &layerTimeM1_BLACK.layer);
		layer_mark_dirty(&layerTimeM1_BLACK.layer);
	//Black 4
		bitmap_layer_init(&layerTimeM2_BLACK,timeMinuteOnesFrame);
		bitmap_layer_set_bitmap(&layerTimeM2_BLACK, &imgTime_BLACK[3].bmp); //or .layer?
		bitmap_layer_set_background_color(&layerTimeM2_BLACK,GColorClear);
		bitmap_layer_set_compositing_mode(&layerTimeM2_BLACK, GCompOpOr);
		layer_add_child(&window.layer, &layerTimeM2_BLACK.layer);
		layer_mark_dirty(&layerTimeM2_BLACK.layer);
	}
}

void request_weather();

void failed(int32_t cookie, int http_status, void* context) {
	if(cookie == 0 || cookie == WEATHER_HTTP_COOKIE) {
		currentWeather = 11;
		SetBitmap(WEATHER);
		//weather_layer_set_icon(&weather_layer, WEATHER_ICON_NO_WEATHER);
		//text_layer_set_text(&weather_layer.temp_layer, "---°");
	}
	link_monitor_handle_failure(http_status);
	httpFailed = true;
	//Re-request the location and subsequently weather on next minute tick
	located = false;
}

void success(int32_t cookie, int http_status, DictionaryIterator* received, void* context) {
	if(cookie != WEATHER_HTTP_COOKIE) return;
	Tuple* icon_tuple = dict_find(received, WEATHER_KEY_ICON);
	int icon = icon_tuple->value->int8;
	if(icon >= 0 && icon < 10) {
		currentWeather = icon;
	}
	else
	{
		currentWeather = 11;
	}
	SetBitmap(WEATHER);
	Tuple* temperature_tuple = dict_find(received, WEATHER_KEY_TEMPERATURE );
	if(temperature_tuple) {
		
	}
	httpFailed = false;
	link_monitor_handle_success();
}

void location(float latitude, float longitude, float altitude, float accuracy, void* context) {
	// Fix the floats
	our_latitude = latitude * 10000;
	our_longitude = longitude * 10000;
	located = true;
	request_weather();
}

void reconnect(void* context) {
	located = false;
	request_weather();
}

void request_weather();

void GetAndSetCurrentWord(PblTm* currentTime)
{
    switch (currentTime->tm_hour)
    {
        case 0:
            currentWord = 0; //midnight
            break;
        case 3:
        case 4:
        case 5:
            currentWord = 1; //early morning
            break;
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            currentWord = 2; //morning
            break;
        case 11:
		case 12:
            currentWord = 3;//lunchtime
            break;
		case 13:
        case 14:
        case 15:
        case 16:
		case 17:
            currentWord = 4;//afternoon
            break;
        case 18:
        case 19:
		case 20:
            currentWord = 5;//evening
            break;
        case 21:
        case 22:
        case 23:
        case 24:
		case 1:
		case 2:
            currentWord = 6;//night
            break;
    }

    if (previousWord != currentWord)
    {
		SetBitmap(WORD);
		previousWord = currentWord;
    }
    
}
/*void FormatTime(PblTm* currentTime)
{
	formattedTime = [YYYY]-[MM]-[DD]T[HH]:[MM]:[SS]
}
formattedTime = [YYYY]-[MM]-[DD]T[HH]:[MM]:[SS]*/

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)t;
  (void)ctx;
	
//	if (getHTTP)
//	{
//		getHTTP = false;
	//}
	if (!isTransitioning)
	{
		static char dateText[] = "00 00";
		string_format_time(dateText, sizeof(dateText), "%m/%d", t->tick_time);
		text_layer_set_text(&layerDate, dateText);
		
		static char dateWordText[] = " Xxx";
		string_format_time(dateWordText, sizeof(dateWordText), " %a", t->tick_time);
		
		//uppercase it, should be by itself, but fuck it.
		for (int i = 0; dateWordText[i] != 0; i++) {
			if (dateWordText[i] >= 'a' && dateWordText[i] <= 'z') {
				dateWordText[i] -= 0x20;
			}
		}
		
		text_layer_set_text(&layerDateWord, dateWordText);
		
		static char timeText[] = "00:00";
		char *timeFormat;
		timeFormat = clock_is_24h_style() ? "%R" : "%I:%M";
		string_format_time(timeText, sizeof(timeText), timeFormat, t->tick_time);


		// Kludge to handle lack of non-padded hour format string
		// for twelve hour clock.
		if (!clock_is_24h_style() && (timeText[0] == '0'))
		{
			memmove(timeText, &timeText[1], sizeof(timeText) - 1);
		}
		//not right, should be by itself but don't want to pass around w/e

		int hour = t->tick_time->tm_hour;
		if (previousHour != hour)
		{
			currentDay = 	t->tick_time->tm_mday;
			previousHour = hour;
			GetAndSetCurrentWord(t->tick_time);
			//if within check for weather()
		}

		if (previousDay != currentDay){}

		if (!clock_is_24h_style()) 
		{
			hour = hour % 12;
			if (hour == 0) 
			{
				hour = 12;
			}	
		}	
		int value = hour %100;

		currentH1 = value / 10;
		currentH2 = value % 10;
		value = t->tick_time->tm_min %100;
		currentM1 = value / 10;
		currentM2 = value % 10;

		SetBitmap(TIME);
		
		if(!located || !(t->tick_time->tm_min % 60))
		{
			//Every 60 minutes, request updated weather			
			request_weather();
		}
		else if (httpFailed)
		{
			//Every minute, ping the phone
			link_monitor_ping();
		}
		
	}
}

void RefreshAll()
{
    SetBitmap(TIME);
	SetBitmap(WORD);
	SetBitmap(WEATHER);
}


void Watchface()//DISPLAYS THE "WATCH PART" OR, NO TRANSITIONS ARE RUNNING
{
	if (!isTransitioning)
	{
		window_set_background_color(&window, GColorClear);
		
		layer_remove_from_parent(&background_image.layer.layer);
		bmp_deinit_container(&background_image);
		
		bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND,  &background_image);
		layer_add_child(&window.layer,&background_image.layer.layer);
			
		text_layer_init(&layerDate, window.layer.frame);
		text_layer_set_text_color(&layerDate, GColorWhite);
		text_layer_set_background_color(&layerDate, GColorClear);
		layer_set_frame(&layerDate.layer,dateFrame);
		text_layer_set_font(&layerDate, fontDate);
		layer_add_child(&window.layer, &layerDate.layer);
	
		text_layer_init(&layerDateWord, window.layer.frame);
		text_layer_set_text_color(&layerDateWord, GColorWhite);
		text_layer_set_background_color(&layerDateWord, GColorClear);
		layer_set_frame(&layerDateWord.layer,wordDateFrame);
		text_layer_set_font(&layerDateWord, fontAbbr);
		layer_add_child(&window.layer, &layerDateWord.layer);	
	
		//DIGIT_IMAGE_RESOURCE_IDS
		for (int i = 0; i < TOTAL_TIME_DIGITS; i++) {
			bmp_init_container(RESOURCE_ID_IMAGE_DIGIT_0, &imgTime_BLACK[i]);
			imgTimeResourceIds_BLACK[i] = -1;
		}
	
		for (int i = 0; i < TOTAL_TIME_DIGITS; i++) {
			bmp_init_container(RESOURCE_ID_IMAGE_DIGIT_0_INVERT, &imgTime_WHITE[i]);
			imgTimeResourceIds_WHITE[i] = -1;
		}
		
		if (!Initializing)
		{
			RefreshAll();
		}
	}
}


void WordTransitionIntro()
{
	
}


void ClosingAnimationStopped(Animation* animation, bool finished, void* context)
{
 	layer_remove_from_parent(&dayTransition.layerDayTrans.layer);
	layer_remove_from_parent(&dayTransition.layerTextYear.layer);
	layer_remove_from_parent(&dayTransition.layerTextMonth.layer);
	//special case de-inting
	layer_remove_from_parent(&dayTransition.day.layerDay.layer);
	layer_remove_from_parent(&dayTransition.day.layerWord.layer);
	if (!dayTransition.day.isSplit)
	{
		layer_remove_from_parent(&dayTransition.day.layerWeather.layer);
		bmp_deinit_container(&dayTransition.day.imgWeather);
	}
	else
	{
		layer_remove_from_parent(&dayTransition.day.layerWeatherSplitTop_WHITE.layer);
		bmp_deinit_container(&dayTransition.day.imgWeatherSplitTop_WHITE);
		layer_remove_from_parent(&dayTransition.day.layerWeatherSplitBottom_WHITE.layer);
		bmp_deinit_container(&dayTransition.day.imgWeatherSplitBottom_WHITE);
		layer_remove_from_parent(&dayTransition.day.layerWeatherSplitTop_BLACK.layer);
		bmp_deinit_container(&dayTransition.day.imgWeatherSplitTop_BLACK);
		layer_remove_from_parent(&dayTransition.day.layerWeatherSplitBottom_BLACK.layer);
		bmp_deinit_container(&dayTransition.day.imgWeatherSplitBottom_BLACK);
	}
	layer_remove_from_parent(&dayTransition.invertLayer.layer);
	isTransitioning = false;
	Initializing = false;
	Watchface();
}

void ClosingAnimationStarted(Animation* animation, bool finished, void* context)
{
	/*for (int i = 0; i < 4; i++)
	{
		if (i != 2)
		{
			layer_remove_from_parent(&dayTransition.day[i].layerDay.layer);
			layer_remove_from_parent(&dayTransition.day[i].layerWord.layer);
		}
	}
	RemoveAndDeIntBmp(WEATHER);*/
}
		
							   
void DayTransitionClosing(AppContextRef ctx)
{
	//setup closing animation?
	property_animation_init_layer_frame(&dayTransition.closingAnimation, &dayTransition.layerDayTrans.layer, &dayTransition.endRect,&dayTransition.startRect);
	animation_set_duration( &dayTransition.closingAnimation.animation, closingAnimationDuration);
	animation_set_delay(&dayTransition.closingAnimation.animation, closingAnimationDelay);
	animation_set_handlers(&dayTransition.closingAnimation.animation, (AnimationHandlers){
		
		.stopped = (AnimationStoppedHandler)ClosingAnimationStopped,
		.started = (AnimationStartedHandler)ClosingAnimationStarted
	}, (void *)ctx);
	
	animation_schedule(&dayTransition.closingAnimation.animation);
}
						   
void SlidingAnimationStopped(Animation* animation, bool finished, void* context) 
{
	DayTransitionClosing(context);	
}
						   
void SetupSlidingFrames()
{
		//vibes_short_pulse();
		dayTransition.day.wordRect = DAY_3_WORD_FRAME;
		dayTransition.day.dayRect = DAY_3_DAY_FRAME;
		dayTransition.day.imgRect = DAY_3_IMAGE_FRAME;
		//for now it'll be the same day on all of them, but we'll have to implement maths and offsets later
		static char wordText[] = " Xxx";
		static char dayText[] = "00";

		string_format_time(dayText, sizeof(dayText), "%d", dayTransition.tick.tick_time); //WORDS
		string_format_time(wordText, sizeof(wordText), " %a", dayTransition.tick.tick_time); //NUMBERS
		//uppercase it, should be by itself, but fuck it.
		for (int index = 0; wordText[index] != 0; index++) {
			if (wordText[index] >= 'a' && wordText[index] <= 'z') {
				wordText[index] -= 0x20;
			}
		}				
		
		//layers init
		text_layer_init(&dayTransition.day.layerWord, dayTransition.day.wordRect);
		text_layer_set_text_color(&dayTransition.day.layerWord, GColorWhite);
		text_layer_set_background_color(&dayTransition.day.layerWord, GColorClear);
		text_layer_set_font(&dayTransition.day.layerWord, fontYear); //just to test
		text_layer_set_text_alignment(&dayTransition.day.layerWord, GTextAlignmentCenter);
		layer_add_child(&window.layer, &dayTransition.day.layerWord.layer);
		text_layer_set_text(&dayTransition.day.layerWord,wordText);
		
		text_layer_init(&dayTransition.day.layerDay, dayTransition.day.dayRect);
		text_layer_set_text_color(&dayTransition.day.layerDay, GColorWhite);
		text_layer_set_background_color(&dayTransition.day.layerDay, GColorClear);
		text_layer_set_font(&dayTransition.day.layerDay, fontDate); //just to test
		text_layer_set_text_alignment(&dayTransition.day.layerDay, GTextAlignmentCenter);
		layer_add_child(&window.layer, &dayTransition.day.layerDay.layer);			
		text_layer_set_text(&dayTransition.day.layerDay,dayText);		
		
		dayTransition.day.isSplit = false;//DEBUG	
	
	/*for (int i = 0; i < 4; i++)
	{
		if (i == 0)
		{
			//vibes_short_pulse();
			dayTransition.day[i].wordRect = DAY_1_WORD_FRAME;
			dayTransition.day[i].dayRect = DAY_1_DAY_FRAME;
			dayTransition.day[i].imgRect = DAY_1_IMAGE_FRAME;
		}	
		else if (i == 1)
		{
			//vibes_short_pulse();
			dayTransition.day[i].wordRect = DAY_2_WORD_FRAME;
			dayTransition.day[i].dayRect = DAY_2_DAY_FRAME;
			dayTransition.day[i].imgRect = DAY_2_IMAGE_FRAME;
		}
		else if (i == 2)
		{
			//vibes_short_pulse();
			dayTransition.day[i].wordRect = DAY_3_WORD_FRAME;
			dayTransition.day[i].dayRect = DAY_3_DAY_FRAME;
			dayTransition.day[i].imgRect = DAY_3_IMAGE_FRAME;
		}
		else if (i == 3)
		{
			//vibes_short_pulse();
			dayTransition.day[i].wordRect = DAY_4_WORD_FRAME;
			dayTransition.day[i].dayRect = DAY_4_DAY_FRAME;
			dayTransition.day[i].imgRect = DAY_4_IMAGE_FRAME;
		}
		
		//for now it'll be the same day on all of them, but we'll have to implement maths and offsets later
		static char wordText[] = " Xxx";
		static char dayText[] = "00";

		string_format_time(dayText, sizeof(dayText), "%d", dayTransition.tick.tick_time); //WORDS
		string_format_time(wordText, sizeof(wordText), " %a", dayTransition.tick.tick_time); //NUMBERS
		//uppercase it, should be by itself, but fuck it.
		for (int index = 0; wordText[index] != 0; index++) {
			if (wordText[index] >= 'a' && wordText[index] <= 'z') {
				wordText[index] -= 0x20;
			}
		}				
		
		//layers init
		text_layer_init(&dayTransition.day[i].layerWord, dayTransition.day[i].wordRect);
		text_layer_set_text_color(&dayTransition.day[i].layerWord, GColorWhite);
		text_layer_set_background_color(&dayTransition.day[i].layerWord, GColorClear);
		text_layer_set_font(&dayTransition.day[i].layerWord, fontYear); //just to test
		text_layer_set_text_alignment(&dayTransition.day[i].layerWord, GTextAlignmentCenter);
		layer_add_child(&window.layer, &dayTransition.day[i].layerWord.layer);
		text_layer_set_text(&dayTransition.day[i].layerWord,wordText);
		
		text_layer_init(&dayTransition.day[i].layerDay, dayTransition.day[i].dayRect);
		text_layer_set_text_color(&dayTransition.day[i].layerDay, GColorWhite);
		text_layer_set_background_color(&dayTransition.day[i].layerDay, GColorClear);
		text_layer_set_font(&dayTransition.day[i].layerDay, fontDate); //just to test
		text_layer_set_text_alignment(&dayTransition.day[i].layerDay, GTextAlignmentCenter);
		layer_add_child(&window.layer, &dayTransition.day[i].layerDay.layer);			
		text_layer_set_text(&dayTransition.day[i].layerDay,dayText);		
		
		dayTransition.day[i].isSplit = false;//DEBUG	
	}*/
}							   

void OpeningAnimationStopped(Animation* animation, bool finished, void* context) {
	dayTransition.layerTextMonthFrame = LAYER_TEXT_MONTH_FRAME;	
	//we need to remove this layer, and redraw it, but black BG. so the inverter layer will work right.
	layer_remove_from_parent(&dayTransition.layerDayTrans.layer);
	bitmap_layer_init(&dayTransition.layerDayTrans, dayTransition.endRect);
	bitmap_layer_set_background_color(&dayTransition.layerDayTrans, GColorBlack);
	layer_add_child(&window.layer, &dayTransition.layerDayTrans.layer);
	
	text_layer_init(&dayTransition.layerTextMonth, dayTransition.layerTextMonthFrame);
    text_layer_set_text_color(&dayTransition.layerTextMonth, GColorWhite);
    text_layer_set_background_color(&dayTransition.layerTextMonth, GColorClear);
    text_layer_set_font(&dayTransition.layerTextMonth, fontMonth);
	text_layer_set_text_alignment(&dayTransition.layerTextMonth, GTextAlignmentCenter);
    layer_add_child(&window.layer, &dayTransition.layerTextMonth.layer);
	static char monthText[] = "00";
	string_format_time(monthText, sizeof(monthText), "%m", dayTransition.tick.tick_time);
	if (monthText[0] == '1')
	{
		//need to determine how to properly get just the first digit.
		text_layer_set_text(&dayTransition.layerTextMonth, (monthText));
	}
	else
	{
		//text_layer_set_text(&dayTransition.layerTextMonth, monthText);
		text_layer_set_text(&dayTransition.layerTextMonth, "6");
	}
	
	dayTransition.layerTextYearFrame = LAYER_TEXT_YEAR_FRAME;
	text_layer_init(&dayTransition.layerTextYear, dayTransition.layerTextYearFrame);
    text_layer_set_text_color(&dayTransition.layerTextYear, GColorWhite);
    text_layer_set_background_color(&dayTransition.layerTextYear, GColorClear);
    text_layer_set_font(&dayTransition.layerTextYear, fontYear);
	text_layer_set_text_alignment(&dayTransition.layerTextYear, GTextAlignmentCenter);
    layer_add_child(&window.layer, &dayTransition.layerTextYear.layer);
	static char yearText[] = "0000";
	string_format_time(yearText, sizeof(yearText), "%Y", dayTransition.tick.tick_time);
	text_layer_set_text(&dayTransition.layerTextYear, yearText);
	SetupSlidingFrames();
	SetBitmap(WEATHER);
	
	inverter_layer_init(&dayTransition.invertLayer, dayTransition.endRect);
	layer_add_child(&window.layer,&dayTransition.invertLayer.layer);
	
	//DayTransitionClosing(context);	
	/*
	for (int i = 0; i < 4; i++)
	{
		//perhaps i should just have all the texts on 1 layer, and just write to those frames? dunno if possible, need to check
		//incase not, doing it the complax way.
		if (i == 0)
		{
			property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWord.layer,&dayTransition.day[i].wordRect,&DAY_0_WORD_FRAME);
			property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerDay.layer,&dayTransition.day[i].dayRect,&DAY_0_DAY_FRAME);
			if (!&dayTransition.day[i].isSplit)
			{
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeather.layer,&dayTransition.day[i].imgRect,&DAY_0_IMAGE_FRAME);
			}
			else
			{
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitTop_WHITE.layer,&dayTransition.day[i].imgRect,&DAY_0_IMAGE_FRAME);
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitTop_BLACK.layer,&dayTransition.day[i].imgRect,&DAY_0_IMAGE_FRAME);
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitBottom_WHITE.layer,&dayTransition.day[i].imgRect,&DAY_0_IMAGE_FRAME);
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitBottom_BLACK.layer,&dayTransition.day[i].imgRect,&DAY_0_IMAGE_FRAME);
			}
		}
		else if (i == 1)
		{
			property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWord.layer,&dayTransition.day[i].wordRect,&DAY_1_WORD_FRAME);
			property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerDay.layer,&dayTransition.day[i].dayRect,&DAY_1_DAY_FRAME);
			if (!&dayTransition.day[i].isSplit)
			{
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeather.layer,&dayTransition.day[i].imgRect,&DAY_1_IMAGE_FRAME);
			}
			else
			{
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitTop_WHITE.layer,&dayTransition.day[i].imgRect,&DAY_1_IMAGE_FRAME);
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitTop_BLACK.layer,&dayTransition.day[i].imgRect,&DAY_1_IMAGE_FRAME);
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitBottom_WHITE.layer,&dayTransition.day[i].imgRect,&DAY_1_IMAGE_FRAME);
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitBottom_BLACK.layer,&dayTransition.day[i].imgRect,&DAY_1_IMAGE_FRAME);
			}
		}
		else if (i == 2)
		{
			property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWord.layer,&dayTransition.day[i].wordRect,&DAY_2_WORD_FRAME);
			property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerDay.layer,&dayTransition.day[i].dayRect,&DAY_2_DAY_FRAME);
			if (!&dayTransition.day[i].isSplit)
			{
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeather.layer,&dayTransition.day[i].imgRect,&DAY_2_IMAGE_FRAME);
			}
			else
			{
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitTop_WHITE.layer,&dayTransition.day[i].imgRect,&DAY_2_IMAGE_FRAME);
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitTop_BLACK.layer,&dayTransition.day[i].imgRect,&DAY_2_IMAGE_FRAME);
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitBottom_WHITE.layer,&dayTransition.day[i].imgRect,&DAY_2_IMAGE_FRAME);
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitBottom_BLACK.layer,&dayTransition.day[i].imgRect,&DAY_2_IMAGE_FRAME);
			}
		}
		else if (i == 3)
		{
			property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWord.layer,&dayTransition.day[i].wordRect,&DAY_3_WORD_FRAME);
			property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerDay.layer,&dayTransition.day[i].dayRect,&DAY_3_DAY_FRAME);
			if (!&dayTransition.day[i].isSplit)
			{
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeather.layer,&dayTransition.day[i].imgRect,&DAY_3_IMAGE_FRAME);
			}
			else
			{
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitTop_WHITE.layer,&dayTransition.day[i].imgRect,&DAY_3_IMAGE_FRAME);
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitTop_BLACK.layer,&dayTransition.day[i].imgRect,&DAY_3_IMAGE_FRAME);
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitBottom_WHITE.layer,&dayTransition.day[i].imgRect,&DAY_3_IMAGE_FRAME);
				property_animation_init_layer_frame(&dayTransition.day[i].slideLeftAnimation, &dayTransition.day[i].layerWeatherSplitBottom_BLACK.layer,&dayTransition.day[i].imgRect,&DAY_3_IMAGE_FRAME);
			}
			//only set handlers on the last frame.
			animation_set_handlers(&dayTransition.day[i].slideLeftAnimation.animation, (AnimationHandlers){
			.stopped = (AnimationStoppedHandler)SlidingAnimationStopped
			}, context);
		}
		animation_set_duration(&dayTransition.day[i].slideLeftAnimation.animation, slideAnimationDuration);
		animation_set_delay(&dayTransition.day[i].slideLeftAnimation.animation, slideAnimationDelay);
		animation_schedule(&dayTransition.openingAnimation.animation);
	}*/
}

void DayTransition(AppContextRef ctx)
{
	isTransitioning = true;
		//setup
	dayTransition.startRect = START_RECT;
	dayTransition.endRect = END_RECT;
		
	bitmap_layer_init(&dayTransition.layerDayTrans, dayTransition.startRect);
	bitmap_layer_set_background_color(&dayTransition.layerDayTrans, GColorWhite);
	layer_add_child(&window.layer, &dayTransition.layerDayTrans.layer);
	
	//opening bar animation
	property_animation_init_layer_frame(&dayTransition.openingAnimation, &dayTransition.layerDayTrans.layer,&dayTransition.startRect,&dayTransition.endRect);
	animation_set_duration(&dayTransition.openingAnimation.animation, openAnimationDuration);
	animation_set_delay(&dayTransition.openingAnimation.animation, openAnimationDelay);
  	animation_set_handlers(&dayTransition.openingAnimation.animation, (AnimationHandlers){
	.stopped = (AnimationStoppedHandler)OpeningAnimationStopped
	}, (void *)ctx);
	animation_schedule(&dayTransition.openingAnimation.animation);
	get_time(&dayTransition.tm);
    dayTransition.tick.tick_time = &dayTransition.tm;
	/*
	//request_weather();
	//get weather info (may have to do @ very, very, start)
	//appear dates
	//slide dates
	//
	
//end
//deint layers
//wait until nothing scheduled animation wise
//deint(openingLayer);
	
	//vibes_double_pulse();
	*/
	//psleep(2500); //should cause it to wait until after the minimize animation
}

void WordTransition()
{
	if (!isDayTransition)
	{
		//end
		isWordTransition = false;
		isTransitioning = false;
		Watchface();
	}
}

void handle_init(AppContextRef ctx) {
    (void) ctx;

    window_init(&window, "PEBSONA4");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);

    resource_init_current_app(&APP_RESOURCES);
    bmp_init_container(RESOURCE_ID_IMAGE_TIME_BLANK,  &background_image);
	layer_add_child(&window.layer,&background_image.layer.layer);
    
	//Fonts
	resDate = resource_get_handle(RESOURCE_ID_FONT_DAYS_26);
	fontDate = fonts_load_custom_font(resDate);
	resAbbr = resource_get_handle(RESOURCE_ID_FONT_DAYS_13);
	fontAbbr = fonts_load_custom_font(resAbbr);
	resYear = resource_get_handle(RESOURCE_ID_FONT_DAYS_13);
	fontYear = fonts_load_custom_font(resYear);
	resMonth = resource_get_handle(RESOURCE_ID_FONT_DAYS_42);
	fontMonth = fonts_load_custom_font(resMonth);
	
    //SETUP INIT STUFF
	//previousMinute = 99;
    previousHour = 99;
    previousWord = 99;
    previousDay = 99;
	previousWeather = 99;
	//currentMinute = 88;
	//currentHour = 88;
	currentWord = 99;
	currentDay = 88;
	currentWeather = 11; //uNKNOWN

	DayTransition(ctx);
	http_register_callbacks((HTTPCallbacks){.failure=failed,.success=success,.reconnect=reconnect,.location=location}, (void*)ctx);
	//psleep(3000);//incase too fast
	/*
	//shouldn't act until return from day transition right?'
	//Watchface();
		
	PblTm tm;
    PebbleTickEvent t;
	//run into transition	
	// Refresh time
	get_time(&tm);
    t.tick_time = &tm;
    t.units_changed = SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT | DAY_UNIT;

	handle_minute_tick(ctx, &t);*/
	//(Leads into BG Display)	
}

void handle_deinit(AppContextRef ctx) 
{
	(void)ctx;
		bmp_deinit_container(&background_image);
		bmp_deinit_container(&imgWeather);
		bmp_deinit_container(&imgWord_BLACK);
		bmp_deinit_container(&imgWord_WHITE);
		bmp_deinit_container(&imgColon_BLACK);
		bmp_deinit_container(&imgColon_WHITE);
		for (int i = 0; i < TOTAL_TIME_DIGITS; i++)
		{
			bmp_deinit_container(&imgTime_BLACK[i]);
			bmp_deinit_container(&imgTime_WHITE[i]);
		}
		fonts_unload_custom_font(fontDate);
		fonts_unload_custom_font(fontAbbr);
		fonts_unload_custom_font(fontYear);
		fonts_unload_custom_font(fontMonth);
}

void pbl_main(void *params)
{
    PebbleAppHandlers handlers =
    {
        .init_handler = &handle_init,
        .deinit_handler = &handle_deinit,
        .tick_info =
        {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
        },
			.messaging_info = {
			.buffer_sizes = {
			.inbound = 124,
			.outbound = 124,
				}
			}
    };

    app_event_loop(params, &handlers);
}

	
void request_weather() {
	if(!located) {
		http_location_request();
		return;
	}
	// Build the HTTP request
	DictionaryIterator *body;
	//HTTPResult result = http_out_get("http://masamuneshadow.herobo.com/api/weather_current.php", WEATHER_HTTP_COOKIE, &body);
	//HTTPResult result = http_out_get("http://pwdb.kathar.in/pebble/weather3.php", WEATHER_HTTP_COOKIE, &body);	
	HTTPResult result = http_out_get("http://www.zone-mr.net/api/weather.php", WEATHER_HTTP_COOKIE, &body);
	if(result != HTTP_OK) {
		currentWeather = 11;
		SetBitmap(WEATHER);
		return;
	}
	dict_write_int32(body, WEATHER_KEY_LATITUDE, our_latitude);
	dict_write_int32(body, WEATHER_KEY_LONGITUDE, our_longitude);
	dict_write_cstring(body, WEATHER_KEY_UNIT_SYSTEM, UNIT_SYSTEM);
	//Get formatted Time(formattedTime);
	//dict_write_cstring(body, WEATHER_KEY_TIME, formattedTime);
	//HTTP_TIME_KEY
	// Send it.
	if(http_out_send() != HTTP_OK) {
		currentWeather = 11;
		SetBitmap(WEATHER);
		return;
	}
}

//get weather @ 8
//get weather at 6 pm?
//if mornweather == pmweather use non-split image.