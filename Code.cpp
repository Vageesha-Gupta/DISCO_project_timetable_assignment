#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>

using namespace std;

// Class representing a Course
class Course {
public:
    string name;//unique identifier ID for a course
    int constraint;  // Constraint to be satisfied (initially set to 1)
    bool is_cdc;// true if cdc false if elective
    // Constructor to initialize a Course object
    Course(string n){
        this->name=n;
        this->constraint=1;
        if(name[2]=='C' ) is_cdc = true;
        else is_cdc = false;
    }
};

// Class representing a Professor
class Professor {
public:
    string name;
    string category;
    vector<Course> preference_list;
    vector<Course> assigned_courses;
    int filled;
    int max;
    
    // Constructor to initialize a Professor object
    Professor(string n, string cat, vector<Course> pref) : name(n), category(cat), preference_list(pref) {
        if(category == "x1" || category == "X1") {max = 0.5; filled = 0;}
        else if(category == "x2" || category == "X2"){max = 1.0; filled = 0;}
        else if(category == "x3" || category == "X3"){max = 1.5; filled = 0;}
    }
};
//CHECK Function
bool checkpref(Professor p,Course c){
    for(Course a : p.preference_list){
        if(a.name==c.name){
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------------------------
// Function to read professors' details from the input file
vector<Professor> readProfessorsFromFile( vector<Course>& courses) {
   
    vector<Professor> professors;
    

   
   
        // Read the number of FD CDCs, HD CDCs, FD electives, HD electives
        int num_fd_cdc, num_hd_cdc, num_fd_electives, num_hd_electives;
        cin >> num_fd_cdc >> num_hd_cdc >> num_fd_electives >> num_hd_electives;
        cout << num_fd_cdc<<" " << num_hd_cdc <<" "<< num_fd_electives<<" " << num_hd_electives<<endl;
        // Add FD CDCs
        for (int i = 1; i <= num_fd_cdc; ++i) {
            string course_name = "FDC" + to_string(i);
            // cout << course_name;
            Course fd_course(course_name);
            // cout << fd_course.constraint;
            courses.push_back(fd_course);
        }

        // Add HD CDCs
        for (int i = 1; i <= num_hd_cdc; ++i) {
            string course_name = "HDC" + to_string(i);
            Course hd_course(course_name);
            courses.push_back(hd_course);
        }

        // Add FD electives
        for (int i = 1; i <= num_fd_electives; ++i) {
            string course_name = "FDE" + to_string(i);
            Course fd_course(course_name);
            courses.push_back(fd_course);
        }

        // Add HD electives
        for (int i = 1; i <= num_hd_electives; ++i) {
            string course_name = "HDE" + to_string(i);
            Course hd_course(course_name);
            courses.push_back(hd_course);
        }
        
        // Read the number of professors
        int num_professors;
        cin >> num_professors;
        cout << num_professors << endl;
        
        // Read each professor's details
        for (int i = 0; i < num_professors; ++i) {
            string name, category;
            int num_prefs;
            cin >> name >> category >> num_prefs;
            cout << name<<" " << category<<" " << num_prefs<<endl;
            vector<Course> prefs;
            // Read each course in professor's preference list

            for (int j = 0; j < num_prefs; ++j) {
                string courseName;  
                cin >> courseName;
                // cout << endl << courseName;
                bool exist = false;
                for(Course c1 : courses){
                    if(c1.name == courseName){
                        prefs.push_back(c1);
                        exist=true;
                        cout << c1.name << " added \n";
                    }
                    
                    
                }
                if(!exist){
                        cout << "Invalid Course"<<endl;
                }
            }
            // Create Professor object and add to professors vector
            Professor prof(name, category, prefs);
            professors.push_back(prof); 
        }
        
        
    
    
    return professors;
}

bool checkCDCSat(vector<Course>& courses){
    //checks whether the cdc have been sufficiently allotted
    bool done = false;
    for(auto& c : courses){
        if(c.is_cdc && c.constraint == 0){
            done = true;
        }
        else {
            done = false;
            break;
        }
    }
   return done;
}

//-----------------------------------------------------------------------------------------
// Function to assign courses to professors based on preferences and constraints
void assignCourses(vector<Professor>& professors, vector<Course>& courses) {
    // Randomize professors' preference lists to diversify assignment possibilities

    random_device rd;
    mt19937 g(rd());
    shuffle(professors.begin(), professors.end(), g);
    
    int c =0;
    while(!checkCDCSat(courses)){// TO DO, CHECK FOR INFINITE
    // Loop through each professor
        for (auto& prof : professors) {
            // Iterate through professor's preference list while category not satisfied
            //TO DO constraint earlier
            if (prof.filled < prof.max) {
                for (auto& course : prof.preference_list) {
                    // Find the course in the courses vector
                        if (course.constraint > 0 && course.is_cdc) {
                            // if (prof.filled < prof.max) {
                                if(prof.max == 0.5){
                                    //CHECK if another prof can take other half if exist put course
                                    for (auto& prof2 : professors){
                                        if(prof.name != prof2.name && prof2.max - prof2.filled >= 0.5 && checkpref(prof2,course)){
                                                prof2.assigned_courses.push_back(course);
                                                course.constraint = 0;
                                                prof.assigned_courses.push_back(course);
                                                prof.filled += 0.5;
                                                prof2.filled += 0.5;

                                                break;
                                            }
                                    }
                                    // if(course.constraint != 0 ){
                                    //     prof.assigned_courses.push_back(course);
                                    //     course.constraint = 0;
                                    //     prof.filled += 1;
                                    // }

                                    //if not give entire course
                                }
                                else if( prof.max-prof.filled > 0.5 ){
                                        prof.assigned_courses.push_back(course);
                                        course.constraint = 0;
                                        prof.filled += 1;
                                }
                                
                            //}
                        }
                        if(prof.filled == prof.max) break;
                }
            }
        }
        c++;
        if(c>1000){
            break;
        }
    }
    
    //electives
    for (auto& prof : professors) {
        if(prof.filled < prof.max){
        // Iterate through professor's preference list
        for (auto& course : prof.preference_list) {//
                if ( course.constraint > 0 && !course.is_cdc) {
                    if (prof.max - prof.filled == 1 ) {
                        prof.assigned_courses.push_back(course);
                        course.constraint -= 1;
                    }
                    else if(course.constraint == 0.5){
                        for (auto& prof2 : professors){
                                    if(prof.name != prof2.name && prof2.max - prof2.filled >= 0.5){
                                        if(checkpref(prof2,course)){
                                            prof2.assigned_courses.push_back(course);
                                            course.constraint -= 1;
                                            prof.assigned_courses.push_back(course);
                                            prof.filled += 0.5;
                                            prof2.filled += 0.5;
                                            break;
                                        }
                                    }
                                    
                        }
                    }
                }
            }
        }
    }

}

// Function to print the final assignment of courses to professors
void printAssignment( vector<Professor>& professors) {
    for ( auto& prof : professors) {
        cout << prof.name << " is assigned courses: ";
        for ( auto& course : prof.assigned_courses) {
            cout << course.name << " ";
        }
        cout << endl;
    }
}




int main() {
    string input_file = "input.txt";  // Replace with your input file
    freopen("input.txt","r",stdin);
    freopen("output.txt","w",stdout);
    vector<Course> courses;
    vector<Professor> professors = readProfessorsFromFile(courses);
    
    for(int i=0; i< 5;i++){
    assignCourses(professors, courses);
    printAssignment(professors);

    cout<<endl;
    cout<<endl;}
    
    return 0;
}
