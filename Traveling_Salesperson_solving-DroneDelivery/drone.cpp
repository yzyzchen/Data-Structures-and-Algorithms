// Project Identifier: 1761414855B69983BD8035097EFBD312EB0527F0
#include <getopt.h>
#include <cmath>
#include <limits>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <stdio.h>
#include <string.h>

using namespace std;

//PROGRAMMER: YUZHOU CHEN
//PROGRAM NAME: DRONE DELIVERY
//DATE: 04/18/2024

enum class LocationType {Normal, Medical, Border, Empty};
// enum class OutputType {MST, FASTTSP, OPTTSP};


class Loc{
    private:
        int x, y;
        LocationType type;
        //default ctor
        Loc(): x {0}, y {0}, type{LocationType::Empty}{};
        //value ctor
        Loc(int x_in, int y_in, bool isMST){
            x = x_in;
            y = y_in;
            if(isMST){
                if(x < 0 && y < 0) type = LocationType::Medical;
                else if((x < 0 && y == 0) || (y < 0 && x == 0) || (x == 0 && y == 0)) type = LocationType::Border;
                else type = LocationType::Normal;
            }
        };

    friend class Drone;
};

class Drone{
    public:
        void getOptions(int argc, char** argv);
        void gen();
    private:
        char mode_type;
        size_t vec_length;
        vector<Loc> Locs;

        //MST
        double total_dis = 0;
        vector<bool> prime_K;
        vector<double> prime_Dis;
        vector<size_t> prime_Parent;
        void gen_MST();

        //OPTTSP
        double curr_dis = 0;
        double curr_best = 0;
        // double temp_best = numeric_limits<double>::infinity();
        vector<size_t> OP_path;
        vector<size_t> OP_best_path;
        vector<size_t> MST_these;
        void gen_Perms(size_t permLength);
        bool promising(size_t permLength);
        void OPTTSP_print();

        //FASTTSP
        vector<pair<size_t, double>> FAST_path;
        void gen_FASTTSP();
        void FAST_print();


        //return a euclidiean distance, which is a double
        double get_dis(Loc &A, Loc &B);
        void readLoc();
        
};

void Drone::gen()
{
    readLoc();
    if (mode_type == 'A') {
        gen_MST();
        // MST_print();
    }
    else if (mode_type == 'B') {
        gen_FASTTSP();
        FAST_print();
    }
    else {

        //initialize
        gen_FASTTSP();
        OP_path.resize(vec_length);
        OP_best_path.resize(vec_length);
        for (size_t i = 0; i < vec_length; i++) OP_best_path[i] = FAST_path[i].first;
        for (size_t i = 0; i < vec_length; i++) OP_path[i] = i;
        gen_Perms(1);
        OPTTSP_print();
    }
};

//for last loop, do not need to check anything
void Drone::gen_MST(){
    //initialize
    prime_K.resize(vec_length, false);
    prime_Dis.resize(vec_length, numeric_limits<double>::infinity());
    prime_Parent.reserve(vec_length);

    size_t curr = 0, next = 0, count = 0;
    double dis, total_dis = 0;
    prime_Dis[curr] = 0;
    //each loop only change one false to true
    while (count < vec_length){
        next = 0;
        //set curr to true -------> step 2
        prime_K[curr] = true;
        total_dis += prime_Dis[curr];
        // cout << total_dis << " " << endl;
        //looping
        for (size_t i = 1; i < vec_length; i++){
            if (!prime_K[i]){
                if (next == 0) next = i;
                //update prime_dis, parent -------> step 3
                dis = get_dis(Locs[curr], Locs[i]);
                if (dis < prime_Dis[i]) {
                    prime_Dis[i] = dis;
                    prime_Parent[i] = curr;
                }
                //update smallest -------> step 1
                if (prime_Dis[i] < prime_Dis[next]) next = i;
            }
        }
        // //if no distance was found (next was not updated) (and not the last loop)
        // if (next == 0) {
        //     cerr << "Cannot construct MST" << endl;
        //     exit(0);
        // }

        curr = next;
        count++;
    }
    //print the output
    cout << std::fixed << std::setprecision(2);
    std::cout << total_dis << endl;
    // double total = 0;////////
    //start from index 1
    for (size_t i = 1; i < vec_length; ++i) {
        size_t kid = i;
        size_t parent = prime_Parent[i];
        // total += get_dis(Locs[kid], Locs[parent]);////////
        if (kid < parent) cout << kid << " " << parent << endl;
        else cout << parent << " " << kid << endl;
        // cout << total << endl; ////////
    }

};

// void Drone::MST_print(){
//     //print the output
//     cout << std::fixed << std::setprecision(2);
//     std::cout << total_dis << endl;
//     // double total = 0;////////
//     //start from index 1
//     for (size_t i = 1; i < vec_length; ++i) {
//         size_t kid = i;
//         size_t parent = prime_Parent[i];
//         // total += get_dis(Locs[kid], Locs[parent]);////////
//         if (kid < parent) cout << kid << " " << parent << endl;
//         else cout << parent << " " << kid << endl;
//         // cout << total << endl; ////////
//     }
// };

// void Drone::FASTTP_print(){};
void Drone::OPTTSP_print(){
    //print the output
    cout << std::fixed << std::setprecision(2);
    std::cout << curr_best << endl;
    // double total = 0;////////
    //start from index 1
    // if (OP_best_path.empty()){
    // for (size_t i = 0; i < FAST_path.size() - 1; ++i) {
    //     cout << FAST_path[i].first << " ";
    // }
    // }
    // else{
    for (size_t i = 0; i < OP_best_path.size(); ++i) {
        cout << OP_best_path[i] << " ";
    }
    // }
};


void Drone::gen_Perms(size_t permLength){
    //permutations
    if (permLength == OP_path.size()) {
        //make compelete path
        double closing_dis = get_dis(Locs[OP_path[0]], Locs[OP_path[permLength - 1]]);
        curr_dis += closing_dis;
        //if new total better
        if (curr_dis < curr_best){
            //update new total and best path
            curr_best = curr_dis;
            OP_best_path = OP_path;
            // cout << curr_best << endl;
        }
        //backtracking, is this needed?
        curr_dis -= closing_dis;
        return; 
    }  // if ..complete path

    if (!promising(permLength)) {
        return;
    }  // if ..not promising, then prune it

    for (size_t i = permLength; i < OP_path.size(); ++i) {
        //temporarily swap the elements
        swap(OP_path[permLength], OP_path[i]);
        curr_dis += get_dis(Locs[OP_path[permLength]], Locs[OP_path[permLength - 1]]);
        gen_Perms(permLength + 1);
        //after checking, go backtracking
        curr_dis -= get_dis(Locs[OP_path[permLength]], Locs[OP_path[permLength - 1]]);
        swap(OP_path[permLength], OP_path[i]);
    }  // for ..unpermuted elements
};

bool Drone::promising(size_t permLength){
    if (OP_path.size() - permLength < 5) {
        return true;
    }
    //MST the rest
    // OP_gen_MST(permLength);
    double total_edge = 0;
    //MST length
    size_t length = vec_length - permLength;
    //resize prim table
    prime_K.resize(length, false);
    prime_Dis.resize(length, numeric_limits<double>::infinity());

    size_t curr = 0, next = 0, count = 0;
    double dis;
    prime_Dis[curr] = 0;
    //each loop only change one false to true
    while (count < length){
        next = 0;
        //set curr to true -------> step 2
        prime_K[curr] = true;
        total_edge += prime_Dis[curr];

        // cout << total_dis << " " << endl;
        //looping//////////////MST_these[i] is the indexes of Locs
        for (size_t i = 1; i < length; i++){
            if (!prime_K[i]){
                if (next == 0) next = i;
                //update prime_dis, parent -------> step 3
                dis = get_dis(Locs[OP_path[permLength + curr]], Locs[OP_path[permLength + i]]);
                if (dis < prime_Dis[i]) prime_Dis[i] = dis;
                //update smallest -------> step 1
                if (prime_Dis[i] < prime_Dis[next]) next = i;
            }
        }
        curr = next;
        count++;
    }

    // //calculate the OP first and last
    double armA = numeric_limits<double>::infinity(), armB = numeric_limits<double>::infinity();
    for (size_t i = 0; i < length; i++){
        //link first
        double temp = get_dis(Locs[0], Locs[OP_path[permLength + i]]);
        if (temp < armA) armA = temp;
        // first = min(first, get_dis(Locs[OP_path[0]], Locs[OP_path[MST_these[i]]]));
        //link second
        double temp2 = get_dis(Locs[OP_path[permLength - 1]], Locs[OP_path[permLength + i]]);
        if (temp2 < armB) armB = temp2;
        // last = min(last, get_dis(Locs[OP_path[permLength - 1]], Locs[OP_path[MST_these[i]]]));
    }


    //clear prim vectors
    // MST_these.clear();
    prime_K.clear();
    prime_Dis.clear();
    // prime_Parent.clear();


    //////////////////////////debug////////////////////////////////debug////////
    // string is = "";
    // bool is_promisiong = ((total_edge + armA + armB + curr_dis) < curr_best);
    // if (is_promisiong) is = "true";
    // else is = "false";

    // for (size_t i = 0; i < OP_path.size(); ++i)
    // cerr << setw(2) << OP_path[i] << ' ';
    // cerr << setw(4) << permLength << setw(10) << curr_dis;
    // cerr << setw(10) << armA << setw(10) << armB;
    // cerr << setw(10) << total_edge << setw(10) << total_edge + armA + armB + curr_dis << "  " << is << '\n';
    return ((total_edge + armA + armB + curr_dis) < curr_best);
};

void Drone::gen_FASTTSP(){
    cout << std::fixed << std::setprecision(2);
    //initialize, do not forget to complete the circle
    FAST_path.reserve(vec_length + 1);
    FAST_path.push_back(pair<size_t, double>(0, 0));
    FAST_path.push_back(pair<size_t, double>(1, get_dis(Locs[0], Locs[1])));
    FAST_path.push_back(pair<size_t, double>(0, get_dis(Locs[0], Locs[1])));

    //start the loop
    for (size_t i = 2; i < vec_length; i++){
        size_t min_index = 1;
        double link_adjacent = std::numeric_limits<double>::infinity(),
                link_left, link_right, 
                min_left = std::numeric_limits<double>::infinity(), 
                min_right = std::numeric_limits<double>::infinity();
        double min_link = std::numeric_limits<double>::infinity();

        //loop and try to insert the path
        for (size_t j = 1; j <= i; j++){
            //link adjacent nodes
            link_left = get_dis(Locs[i], Locs[FAST_path[j - 1].first]);
            link_right = get_dis(Locs[i], Locs[FAST_path[j].first]);
            link_adjacent = link_left + link_right - FAST_path[j].second;
            //if it is min, store locally min_index
            if (link_adjacent < min_link) {
                min_link = link_adjacent;
                min_left = link_left;
                min_right = link_right;
                min_index = j;//new node should be insert into 
            }
        }
        //insert and store the dis to left
        FAST_path.insert(FAST_path.begin() + min_index, pair<size_t, double>(i, min_left));
        //change the dis to the right
        FAST_path[min_index + 1].second = min_right;
    }

    //calculate curr_best
    for (size_t i = 1; i < vec_length + 1 ;i++){
        curr_best += FAST_path[i].second;
    }
};

void Drone::FAST_print(){
    cout << curr_best << endl;
    for (size_t i = 0; i < FAST_path.size() - 1; ++i) {
        cout << FAST_path[i].first << " ";
    }
};


void Drone::readLoc(){
    int x_in, y_in;
    cin >> vec_length;
    Locs.reserve(vec_length);
    if (mode_type == 'A'){
        for (size_t i = 0; i < vec_length; i++){
            cin >> x_in >> y_in;
            Locs.push_back(Loc(x_in, y_in, true));
        }
    }
    else{
        for (size_t i = 0; i < vec_length; i++){
            cin >> x_in >> y_in;
            Locs.push_back(Loc(x_in, y_in, false));
        }
    }
};

double Drone::get_dis(Loc &A, Loc &B){
    if (mode_type == 'A'){
        if ((A.type == LocationType::Normal && B.type == LocationType::Medical) || 
            (A.type == LocationType::Medical && B.type == LocationType::Normal)) return numeric_limits<double>::infinity();
        else {
            double x_bar = static_cast<double>(A.x) - static_cast<double>(B.x);
            double y_bar = static_cast<double>(A.y) - static_cast<double>(B.y);
            return sqrt(x_bar * x_bar + y_bar * y_bar);
        }
    }
    else {
        double x_bar = static_cast<double>(A.x) - static_cast<double>(B.x);
        double y_bar = static_cast<double>(A.y) - static_cast<double>(B.y);
        return sqrt(x_bar * x_bar + y_bar * y_bar);
    }
};

void Drone::getOptions(int argc, char** argv){
    int option_index = 0, option = 0;
    string search_type;
    struct option longOpts[] = {{"mode", required_argument, nullptr, 'm'},
                                {"help", no_argument, nullptr, 'h'},
                                {nullptr, 0, nullptr, '\0'} };

    while ((option = getopt_long(argc, argv, "m:h", longOpts, &option_index)) != -1){
        switch (option) {
            case 'm':
                search_type = optarg;
                if (search_type == "MST") mode_type = 'A';
                else if (search_type == "FASTTSP") mode_type = 'B';
                else if (search_type == "OPTTSP") mode_type = 'C';
                else if (search_type == "") {
                    cerr << "Error: No mode specified" << endl;
                    exit(1);
                }
                else {
                    cerr << "Error: Invalid command line option" << endl;
                    exit(1);
                }
                break;

            case 'h':
                cerr << "This program simulates a drone delivery service.\n"
                << "There are two types of drones and \n"
                << "three types of clientsThe program will aim to find the\n"
                << "shortest route to service all locations across\n"
                << "campus.\n";
                exit(0);

            default: 
                cerr << "Error: Invalid command line option" << endl;
                exit(1);
                break;
        }
    }
};


int main(int argc, char** argv) 
{
    std::ios_base::sync_with_stdio(false);
    Drone Drone;
    Drone.getOptions(argc, argv);
    Drone.gen();
    return 0;
}
