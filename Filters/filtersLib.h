#ifndef FILTERSLIB_H_
#define FILTERSLIB_H_

/*DEFINE*/
/*Filter and Signal parameters*/
//maximum number
#define MAX_SIGNALS 10
#define MAX_FILTERS 10

//min and maximum frequency
#define FREQ_MIN 0          // Constant Signal
#define FREQ_MAX 100        // 100 Hz

//n-order filter
#define MAX_ORDER 1         //First Order (actually) 

/*Task Parameters*/
//Index
#define GIDX (MAX_SIGNALS + 1) //Graphic Task
#define UIDX (MAX_SIGNALS + 2) //User Task

//Period
#define FILTER_PERIOD   10     //[ms]
#define GRAPHIC_PERIOD  10     //[ms]
#define USER_PERIOD     30     //[ms]

//Priority  | 1 (low) - 99 (high)
#define FILTER_PRIO     3
#define GRAPHIC_PRIO    2
#define USER_PRIO       1

//Math constants
#define PI 3.14159

//Discrete Time Constants
#define N_SAMPLE_PERIOD 100 //n samples in a period

/*Graphics Parameters*/
#define WIDTH 1024
#define HEIGHT 768

//Title
#define TITLE_WIDTH 50
#define TITLE_HEIGHT 30

//Information Tab height
#define INFO_HEIGHT 68
#define INFO_SIGNAL_WIDTH 500
#define INFO_FILTER_WIDTH (INFO_SIGNAL_WIDTH + 200)

//axis limit
#define XLIM 1.0

//Colors
#define BLACK           0
#define BLUE            1
#define GREEN           2
#define CYAN            3
#define RED             4
#define MAGENTA         5
#define BROWN           6
#define LIGHT_GRAY      7
#define DARK_GRAY       8
#define LIGHT_BLUE      9
#define LIGHT_GREEN     10
#define LIGHT_CYAN      11
#define LIGHT_RED       12
#define LIGHT_MAGENTA   13
#define YELLOW          14
#define WHITE           15

/*ENUMS*/
enum Signal_Type 
{
    sinusoid,
    square,
    sawtooth,
    triang
};

enum Filter_Type
{
    LOW_PASS,
    HIGH_PASS,
    BAND_PASS,
    BAND_STOP
};

/*STRUCTURES*/
struct Signal
{
    //General Attributes of Signal
    double amplitude;
    double frequency;   //[Hz]
    double phase;       //[rad]
    enum Signal_Type signal_type;

    //Discrete Time Parameter
    double Ts;          //[s]
    int    k;           //k-th sample

    //Graphic Attribute
    double t;           //[s]
    double y;
    //double y[MAX_ORDER + 1]; TO DO
    int    color;
};

struct Filter
{
    //General Attributes of Filter
    double f_cut;   //[Hz]
    double gain;
    enum Filter_Type filter_type;

    //Graphic Attribute
    double y_filterd;
    //double y_filterd[MAX_ORDER + 1]; TO DO
    int color;
};

/*Global Resources*/
struct Signal signals[MAX_SIGNALS];
struct Filter filters[MAX_FILTERS];

/*FUNCTION SIGNATURES*/
//Functions
double sign(double x);
double lowPassFilter(double y_k_1, double x_k_1, double a, double Ts);
double highPassFilter(double x_k, double x_k_1, double y_k_1, double a, double Ts);
void plotPoint(BITMAP* window, double time, double y, int color);
void signalRealization(int idx);
void printSignal(struct Signal signal);
void printFilter(struct Filter filter);

//Init
void init();
void set_Ts(int idx);
void init_signal(int idx);
void init_filter(int idx);
void clear_reset(BITMAP* window, int idx);
void draw_oscilloscope(BITMAP* osc, BITMAP* window);
void draw_information(BITMAP* info, BITMAP* window);

//Keyboard Interpreter
void keyboard_interp();

/*TASKS*/
void *filterTask(void* arg);
void *graphicTask(void* arg);
void *userTask(void* arg);

#endif /* FILTERSLIB_H_ */