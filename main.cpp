#include <cstdio>
#define NUMBER_OF_DAYS 7
#define NUMBER_OF_CLASSES 2
#define NUMBER_OF_PERIODS 3

const char* day_names[NUMBER_OF_DAYS] = {"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
const char* period_times[NUMBER_OF_PERIODS][2] = {{"7:30", "9:00"}, {"9:30", "11:00"}, {"11:30", "13:00"}};
typedef struct time_segment time_segment;
typedef struct day day;

using namespace std;

day initialize_day();
int time_to_int(const char *ch);

struct time_segment {
    int st;
    int fn;
};

struct day {
    time_segment periods[NUMBER_OF_PERIODS];
};

int main() {

    return 0;
}

day initialize_day(){
    int i = 0;
    day temp;
    for (i = 0; i < NUMBER_OF_PERIODS; i++) {
        temp.periods[i].st = time_to_int(period_times[i][0]);
        temp.periods[i].fn = time_to_int(period_times[i][1]);
    }
    return temp;
}

int time_to_int(const char *ch){
    int h, m;
    sscanf(ch, "%d:%d", &h, &m);
    return h * 60 + m;
}
