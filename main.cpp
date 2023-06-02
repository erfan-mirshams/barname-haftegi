#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#define NUMBER_OF_DAYS 7
#define NUMBER_OF_CLASSES 2
#define NUMBER_OF_PERIODS 3
#define MAX_CLOCK_LENGTH 20
#define MINUTES_IN_HOUR 60
#define all(x) (x).begin(), (x).end()

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
day *derive_day_from_name(string name, vector<day> *days);
int find_course_index(string name, vector<course> *courses);
course create_course(string name);
void match_available_periods(course *current_course, day *current_day, int st, int fn);
bool course_cmp(course *a, course *b);
bool teacher_cmp(teacher *a, teacher *b);
bool time_segment_cmp(time_segment *a, time_segment *b);
bool are_periods_in_same_time(time_segment a, time_segment b);
time_segment *find_matching_period(course *current_course, time_segment* current_period);
dars make_dars(teacher *instructor, course *subject, time_segment *period1, time_segment* period2);
bool has_dars(kelas current_kelas, course *subject);
string int_to_time(int m);
void print_dars_in_klass(course *course_ptr, kelas current_kelas);

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
    teacher* instructor;
    course* subject;
    time_segment* period1;
    time_segment* period2;
};

struct kelas{
    map<time_segment*, bool> matched;
    vector<dars> doroos;
};


int main(){
    vector<day> days;
    days.reserve(NUMBER_OF_DAYS);
    int i, j, k;
    for(i = 0; i < NUMBER_OF_DAYS; i++){
        days.push_back(initialize_day());
    }

    map<string, int> dars_bi_dard;

    vector<course> courses;

    int teacher_cnt;
    cin >> teacher_cnt;

    vector< vector<int> > temp_course_index(teacher_cnt);

    int course_reserve_cap = 0;

    vector<teacher> teachers;
    teachers.reserve(teacher_cnt);
    for(i = 0; i < teacher_cnt; i++){
        teacher temp_teacher;
        cin >> temp_teacher.name;
        teachers.push_back(temp_teacher);
        int day_cnt;
        string day_name;
        cin >> day_cnt;
        for(j = 0; j < day_cnt; j++){
            cin >> day_name;
            day *temp_day = derive_day_from_name(day_name, &days);
            for(k = 0; k < NUMBER_OF_PERIODS; k++){
               teachers[i].available_periods.push_back(&(temp_day -> periods[k]));
               (*temp_day).periods[k].available_teachers.push_back(&teachers[i]);
            }
        }
        int course_cnt;
        string course_name;
        int course_index;
        cin >> course_cnt;
        course_reserve_cap += course_cnt;
        for(j = 0; j < course_cnt; j++){
            cin >> course_name;
            course_index = find_course_index(course_name, &courses);
            temp_course_index[i].push_back(course_index);
            courses[course_index].available_teachers.push_back(&teachers[i]);
        }
    }

    int course_cnt;
    cin >> course_cnt;
    courses.reserve(course_cnt + course_reserve_cap);

    for(i = 0; i < teacher_cnt; i++){
        for(int course_index : temp_course_index[i]){
            teachers[i].available_courses.push_back(&courses[course_index]);
        }
        temp_course_index[i].clear();
    }

    for(i = 0; i < course_cnt; i++){
        string course_name;
        int course_index, st_time, fn_time;
        day *day1, *day2;
        string day1_name, day2_name, st_period, fn_period;
        cin >> course_name;
        dars_bi_dard[course_name] = 1;
        course_index = find_course_index(course_name, &courses);
        cin >> day1_name >> day2_name;
        day1 = derive_day_from_name(day1_name, &days);
        day2 = derive_day_from_name(day2_name, &days);
        cin >> st_period >> fn_period;
        st_time = time_to_int(st_period);
        fn_time = time_to_int(fn_period);
        match_available_periods(&courses[course_index], day1, st_time, fn_time);
        match_available_periods(&courses[course_index], day2, st_time, fn_time);
    }

    for(i = 0; i < NUMBER_OF_DAYS; i++){
        for(j = 0; j < NUMBER_OF_PERIODS; j++){
            sort(all(days[i].periods[j].available_courses), course_cmp);
        }
    }

    for(i = 0; i < (int)teachers.size(); i++){
        sort(all(teachers[i].available_courses), course_cmp);
        sort(all(teachers[i].available_periods), time_segment_cmp);
    }

    for(i = 0; i < (int)courses.size(); i++){
        sort(all(courses[i].available_teachers), teacher_cmp);
        sort(all(courses[i].available_periods), time_segment_cmp);
    }

    kelas kelases[NUMBER_OF_CLASSES];

    for(i = 0; i < NUMBER_OF_CLASSES; i++){
        for(j = 0; j < NUMBER_OF_DAYS; j++){
            for(k = 0; k < NUMBER_OF_PERIODS; k++){
                time_segment *period_1_it = &days[j].periods[k];
                bool found_subject = false;
                for(course *x : days[j].periods[k].available_courses){
                    if(has_dars(kelases[i], x)){
                        continue;
                    }
                    time_segment *period_2_it = find_matching_period(x, period_1_it);
                    if (kelases[i].matched[period_1_it] || kelases[i].matched[period_2_it]) {
                        continue;
                    }
                    for (teacher *y : x -> available_teachers) {
                        vector<time_segment*>::iterator it1 = find(all(y -> available_periods), period_1_it);
                        vector<time_segment*>::iterator it2 = find(all(y -> available_periods), period_2_it);
                        if(it1 == (y -> available_periods).end() || it2 == (y -> available_periods).end()){
                            continue;
                        }
                        vector<teacher*>::iterator teach_it1 = find(all((*it1) -> available_teachers), y);
                        vector<teacher*>::iterator teach_it2 = find(all((*it2) -> available_teachers), y);
                        if(teach_it1 == (*it1) -> available_teachers.end() || teach_it2 == (*it2) -> available_teachers.end()){
                            continue;
                        }
                        kelases[i].doroos.push_back(make_dars(y, x, *it1, *it2));
                        kelases[i].matched[period_1_it] = true;
                        kelases[i].matched[period_2_it] = true;
                        (*it1) -> available_teachers.erase(teach_it1);
                        (*it2) -> available_teachers.erase(teach_it2);
                        it2 -= (it1 < it2);
                        y -> available_periods.erase(it1);
                        y -> available_periods.erase(it2);
                        found_subject = true;
                        break;
                    }
                    if(found_subject){
                        break;
                    }
                }
            }
        }
    }

    vector<course*> sorted_course_ptr;
    for(i = 0; i < (int)courses.size(); i++){
        sorted_course_ptr.push_back(&courses[i]);
    }
    sort(all(sorted_course_ptr), course_cmp);
    for(course *course_ptr : sorted_course_ptr){
        if(!dars_bi_dard[course_ptr -> name]){
            continue;
        }
        cout << course_ptr -> name << endl;
        for(i = 0; i < NUMBER_OF_CLASSES; i++){
            print_dars_in_klass(course_ptr, kelases[i]);
        }
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

day *derive_day_from_name(string name, vector<day> *days){
    int i;
    for(i = 0; i < NUMBER_OF_DAYS; i++){
        if(!name.compare(day_names[i])){
            return &(*days)[i];
        }
    }
    // does not handle invalid input
    return NULL;
}

int find_course_index(string name, vector<course> *courses){
    int i;
    for(i = 0; i < (int)courses -> size(); i++){
        if(!(*courses)[i].name.compare(name)){
            return i;
        }
    }
    courses -> push_back(create_course(name));
    return (int)courses -> size() - 1;
}

time_segment *find_matching_period(course *current_course, time_segment* current_period){
    for(time_segment* x : current_course -> available_periods){
        if(x == current_period){
            continue;
        }
        if(are_periods_in_same_time(*x, *current_period)){
            return x;
        }
    }
    return NULL;
}

bool are_periods_in_same_time(time_segment a, time_segment b){
    return (a.st == b.st && a.fn == b.fn);
}

void match_available_periods(course *current_course, day *current_day, int st, int fn){
    int i;
    for(i = 0; i < NUMBER_OF_PERIODS; i++){
        if(current_day -> periods[i].st >= st && current_day -> periods[i].fn <= fn){
            current_course -> available_periods.push_back(&(current_day -> periods[i]));
            current_day -> periods[i].available_courses.push_back(current_course);
        }
    }
}

bool course_cmp(course *a,course *b){
    return ((a -> name).compare(b -> name) < 0);
}

bool teacher_cmp(teacher *a, teacher *b){
    int sza = (int) a -> available_periods.size(), szb = (int) b -> available_periods.size();
    return (sza == szb) ? (a -> name.compare(b -> name) < 0) : (sza < szb);
}

bool time_segment_cmp(time_segment *a, time_segment *b){
    return (a -> st == b -> st) ? (a -> fn < b -> fn) : (a -> st < b -> st);
}

course create_course(string name){
    course dummy;
    dummy.name = name;
    return dummy;
}

int time_to_int(const string ch){
    int h, m;
    sscanf(ch.c_str(), "%d:%d", &h, &m);
    return h * MINUTES_IN_HOUR + m;
}

string int_to_time(int m){
    char ch[MAX_CLOCK_LENGTH];
    sprintf(ch, "%02d:%02d", m / MINUTES_IN_HOUR, m % MINUTES_IN_HOUR);
    return ch;
}

bool has_dars(kelas current_kelas, course *subject){
    return find_if(all(current_kelas.doroos), [&](dars x){return x.subject == subject;}) != current_kelas.doroos.end();
}

dars make_dars(teacher *instructor, course *subject, time_segment *period1, time_segment* period2){
    dars temp;
    temp.instructor = instructor;
    temp.subject = subject;
    temp.period1 = period1;
    temp.period2 = period2;
    return temp;
}

void print_dars_in_klass(course *course_ptr, kelas current_kelas){
    vector<dars>::iterator it = find_if(all(current_kelas.doroos), [&](dars x){return x.subject == course_ptr;});
    if(it == current_kelas.doroos.end()){
        cout << "Not Found" << endl;
        return;
    }
    cout << it -> instructor -> name << ": " << int_to_time(it -> period1 -> st) << " " << int_to_time(it -> period1 -> fn) << endl;
}
