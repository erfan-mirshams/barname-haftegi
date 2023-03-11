#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#define NUMBER_OF_DAYS 7
#define NUMBER_OF_CLASSES 2
#define NUMBER_OF_PERIODS 3

using namespace std;

const string day_names[NUMBER_OF_DAYS] = {"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
const string period_times[NUMBER_OF_PERIODS][2] = {{"7:30", "9:00"}, {"9:30", "11:00"}, {"11:30", "13:00"}};
typedef struct time_segment time_segment;
typedef struct day day;
typedef struct course course;
typedef struct teacher teacher;
typedef struct dars dars;
typedef struct kelas kelas;

day initialize_day();
int time_to_int(const string ch);
day derive_day_from_name(string name, vector<day> days);
int find_course_index(string name, vector<course> courses);
course create_course(string name);
void match_available_periods(course &current_course, day &current_day, int st, int fn);
bool course_cmp(const course* &a, const course* &b);
bool teacher_cmp(const teacher* &a, const teacher* &b);
bool time_segment_cmp(const time_segment* &a, const time_segment* &b);

struct time_segment{
    int st;
    int fn;
    vector<teacher*> available_teachers;
    vector<course*> available_courses;
};

struct teacher{
    string name;
    vector<time_segment*> available_periods;
    vector<course*> available_courses;

};

struct course{
    string name;
    vector<time_segment*> available_periods;
    vector<teacher*> available_teachers;
};

struct day{
    time_segment periods[NUMBER_OF_PERIODS];
};

struct dars{
    teacher instructor;
    course subject;
    time_segment period;
};

struct kelas{
    vector<dars> doroos;
};


int main(){
    vector<day> days;
    int i, j, k;
    for(i = 0; i < NUMBER_OF_DAYS; i++){
        days.push_back(initialize_day());
    }

    vector<course> courses;

    int teacher_cnt;
    cin >> teacher_cnt;

    vector<teacher> teachers(teacher_cnt);
    for(i = 0; i < teacher_cnt; i++){
        cin >> teachers[i].name;
        int day_cnt;
        string day_name;
        cin >> day_cnt;
        for(j = 0; j < day_cnt; j++){
            cin >> day_name;
            day temp_day = derive_day_from_name(day_name, days);
            for(int k = 0; k < NUMBER_OF_PERIODS; k++){
               teachers[i].available_periods.push_back(&temp_day.periods[k]);
               temp_day.periods[k].available_teachers.push_back(&teachers[i]);
            }
        }
        int course_cnt;
        string course_name;
        int course_index;
        cin >> course_cnt;
        for(int j = 0; j < course_cnt; j++){
            cin >> course_name;
            course_index = find_course_index(course_name, courses);
            teachers[i].available_courses.push_back(&courses[course_index]);
            courses[course_index].available_teachers.push_back(&teachers[i]);
        }
    }

    int course_cnt;
    cin >> course_cnt;
    for(i = 0; i < course_cnt; i++){
        string course_name;
        int course_index, st_time, fn_time;
        day day1, day2;
        string day1_name, day2_name, st_period, fn_period;
        cin >> course_name;
        course_index = find_course_index(course_name, courses);
        cin >> day1_name >> day2_name;
        day1 = derive_day_from_name(day1_name, days);
        day2 = derive_day_from_name(day2_name, days);
        cin >> st_period >> fn_period;
        st_time = time_to_int(st_period);
        fn_time = time_to_int(fn_period);
        match_available_periods(courses[course_index], day1, st_time, fn_time);
        match_available_periods(courses[course_index], day1, st_time, fn_time);
    }

    for(i = 0; i < NUMBER_OF_DAYS; i++){
        for_each(days[i].periods, days[i].periods + NUMBER_OF_PERIODS, [&](time_segment x){sort(x.available_courses.begin(), x.available_courses.end(), course_cmp);});
    }

    for(teacher current_teacher : teachers){
        sort(current_teacher.available_courses.begin(), current_teacher.available_courses.end(), course_cmp);
        sort(current_teacher.available_periods.begin(), current_teacher.available_periods.end(), time_segment_cmp);
    }

    for(course current_course : courses){
        sort(current_course.available_teachers.begin(), current_course.available_teachers.end(), teacher_cmp);
        sort(current_course.available_periods.begin(), current_course.available_periods.end(), time_segment_cmp);
    }
    return 0;
}

day initialize_day(){
    int i;
    day temp;
    for(i = 0; i < NUMBER_OF_PERIODS; i++){
        temp.periods[i].st = time_to_int(period_times[i][0]);
        temp.periods[i].fn = time_to_int(period_times[i][1]);
    }
    return temp;
}

day derive_day_from_name(string name, vector<day> days){
    int i;
    for(int i = 0; i < NUMBER_OF_DAYS; i++){
        if(!name.compare(day_names[i])){
            return days[i];
        }
    }
    // does not handle invalid input
}

int find_course_index(string name, vector<course> &courses){
    int i;
    for(i = 0; i < (int)courses.size(); i++){
        if(!courses[i].name.compare(name)){
            return i;
        }
    }
    courses.push_back(create_course(name));
    return (int)courses.size() - 1;
}

void match_available_periods(course &current_course, day &current_day, int st, int fn){
    int i;
    for(i = 0; i < NUMBER_OF_PERIODS; i++){
        if(current_day.periods[i].st >= st && current_day.periods[i].fn <= fn){
            current_course.available_periods.push_back(&current_day.periods[i]);
            current_day.periods[i].available_courses.push_back(&current_course);
        }
    }
}

bool course_cmp(const course* &a, const course* &b){
    return ((a -> name).compare(b -> name) < 0);
}

bool teacher_cmp(const teacher* &a, const teacher* &b){
    int sza = (int) a -> available_periods.size(), szb = (int) b -> available_periods.size();
    if(a == b){
        return (a -> name.compare(b -> name) < 0);
    }
    return (a < b);
}

bool time_segment_cmp(const time_segment* &a, const time_segment* &b){
    if(a -> st == b -> st){
        return (a -> fn < b -> fn);
    }
    return (a -> st < b -> st);
}

course create_course(string name){
    course dummy;
    dummy.name = name;
    return dummy;
}

int time_to_int(const string ch){
    int h, m;
    sscanf(ch.c_str(), "%d:%d", &h, &m);
    return h * 60 + m;
}
